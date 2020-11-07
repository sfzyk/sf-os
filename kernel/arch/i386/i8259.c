#include <i386/desc.h>
#include <kernel/sync.h>
#include <i386/io.h>
#include <i386/i8259.h>
#include <i386/irq.h>
#include <i386/config.h>
#include <i386/timer.h>

#include <kernel/hooks.h>

/* smbody */
//	8259A 中的寄存器：
//		ICW: Initialization Command Word,初始化命令寄存器
//		OCW: Operation Command Word,操作命令字,用于控制 8259A
//		IRR: Interrupt Request Register,中断请求寄存器,共 8bit,对应 IR0~IR7 八个中断管脚。当某个管脚的中断请求到来后,
//			若该管脚没有被屏蔽,IRR 中对应的 bit 被置1。表示 PIC 已经收到设备的中断请求,但还未提交给 CPU。
//		ISR: In Service Register,服务中寄存器,共 8bit,每 bit 意义同上。当 IRR 中的某个中断请求被发送给 CPU 后,ISR中对应的 bit 被置1,
//			表示中断已发送给 CPU,但 CPU 还未处理完。
//		IMR: Interrupt Mask Register,中断屏蔽寄存器,共 8bit,每 bit 意义同上。用于屏蔽中断。当某 bit 置1时,对应的中断管脚被屏蔽。
//	注：8259a通过ICW1的bit3位设置触发方式，0为边沿触发，1为电平触发

unsigned int cached_irq_mask = 0xffff;

DEFINE_SPINLOCK(i8259A_lock);

static void end_8259A_irq (unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)) &&
							irq_desc[irq].action)
		enable_8259A_irq(irq);
}

#define shutdown_8259A_irq	disable_8259A_irq

void mask_and_ack_8259A(unsigned int);

unsigned int startup_8259A_irq(unsigned int irq)
{ 
	enable_8259A_irq(irq);
	return 0; /* never anything pending */
}

static struct hw_interrupt_type i8259A_irq_type = {
	"XT-PIC",
	startup_8259A_irq,
	shutdown_8259A_irq,
	enable_8259A_irq,
	disable_8259A_irq,
	mask_and_ack_8259A,
	end_8259A_irq,
	NULL
};

void disable_8259A_irq(unsigned int irq)
{
	unsigned int mask = 1 << irq;
	unsigned long flags;

	spin_lock_irqsave(&i8259A_lock, flags);
	cached_irq_mask |= mask;
	if (irq & 8)
		outb(cached_slave_mask, PIC_SLAVE_IMR);
	else
		outb(cached_master_mask, PIC_MASTER_IMR);
	spin_unlock_irqrestore(&i8259A_lock, flags);
}

void enable_8259A_irq(unsigned int irq)
{
    unsigned int mask = ~(1 << irq);
    unsigned long flags;

    spin_lock_irqsave(&i8259A_lock,flags);
    cached_irq_mask &= mask;
    if(irq & 8)
        outb(cached_slave_mask, PIC_SLAVE_IMR);
    else
        outb(cached_master_mask, PIC_MASTER_IMR);
	
	/*smbody: enable irq action*/
	irq_desc[irq].status = IRQ_PER_CPU;

    spin_unlock_irqrestore(&i8259A_lock, flags);
}

static inline int i8259A_irq_real(unsigned int irq)
{
	int value;
	int irqmask = 1<<irq;

	if (irq < 8) {
		outb(0x0B,PIC_MASTER_CMD);	/* ISR register */
		value = inb(PIC_MASTER_CMD) & irqmask;
		outb(0x0A,PIC_MASTER_CMD);	/* back to the IRR register */
		return value;
	}
	outb(0x0B,PIC_SLAVE_CMD);	/* ISR register */
	value = inb(PIC_SLAVE_CMD) & (irqmask >> 8);
	outb(0x0A,PIC_SLAVE_CMD);	/* back to the IRR register */
	return value;
}

/*
 * Careful! The 8259A is a fragile beast, it pretty
 * much _has_ to be done exactly like this (mask it
 * first, _then_ send the EOI, and the order of EOI
 * to the two 8259s is important!
 */
void mask_and_ack_8259A(unsigned int irq)
{
	unsigned int irqmask = 1 << irq;
	unsigned long flags;

	spin_lock_irqsave(&i8259A_lock, flags);
	/*
	 * Lightweight spurious IRQ detection. We do not want
	 * to overdo spurious IRQ handling - it's usually a sign
	 * of hardware problems, so we only do the checks we can
	 * do without slowing down good hardware unnecesserily.
	 *
	 * Note that IRQ7 and IRQ15 (the two spurious IRQs
	 * usually resulting from the 8259A-1|2 PICs) occur
	 * even if the IRQ is masked in the 8259A. Thus we
	 * can check spurious 8259A IRQs without doing the
	 * quite slow i8259A_irq_real() call for every IRQ.
	 * This does not cover 100% of spurious interrupts,
	 * but should be enough to warn the user that there
	 * is something bad going on ...
	 */
	if (cached_irq_mask & irqmask)
		goto spurious_8259A_irq;
	cached_irq_mask |= irqmask;

handle_real_irq:
	if (irq & 8) {
		inb(PIC_SLAVE_IMR);	/* DUMMY - (do we need this?) */
		outb(cached_slave_mask, PIC_SLAVE_IMR);
		outb(0x60+(irq&7),PIC_SLAVE_CMD);/* 'Specific EOI' to slave */
		outb(0x60+PIC_CASCADE_IR,PIC_MASTER_CMD); /* 'Specific EOI' to master-IRQ2 */
	} else {
		inb(PIC_MASTER_IMR);	/* DUMMY - (do we need this?) */
		outb(cached_master_mask, PIC_MASTER_IMR);
		outb(0x60+irq,PIC_MASTER_CMD);	/* 'Specific EOI to master */
	}
	spin_unlock_irqrestore(&i8259A_lock, flags);
	return;

spurious_8259A_irq:
	/*
	 * this is the slow path - should happen rarely.
	 */
	if (i8259A_irq_real(irq))
		/*
		 * oops, the IRQ _is_ in service according to the
		 * 8259A - not spurious, go handle it.
		 */
		goto handle_real_irq;

	{
		static int spurious_irq_mask;
		/*
		 * At this point we can be sure the IRQ is spurious,
		 * lets ACK and report it. [once per IRQ]
		 */
		if (!(spurious_irq_mask & irqmask)) {
//			printk(KERN_DEBUG "spurious 8259A interrupt: IRQ%d.\n", irq);
			spurious_irq_mask |= irqmask;
		}
		atomic_inc(&irq_err_count);
		/*
		 * Theoretically we do not have to handle this IRQ,
		 * but in Linux this does not cause problems and is
		 * simpler for us.
		 */
		goto handle_real_irq;
	}
}

void init_8259A(int auto_eoi)
{
	unsigned long flags;

	spin_lock_irqsave(&i8259A_lock, flags);

	outb(0xff, PIC_MASTER_IMR);	/* mask all of 8259A-1 */
	outb(0xff, PIC_SLAVE_IMR);	/* mask all of 8259A-2 */

    /* smbody: use outb instead of  outb_p*/
	/*
	 * outb_p - this has to work on a wide range of PC hardware.
	 */
	outb_p(0x11, PIC_MASTER_CMD);	/* ICW1: select 8259A-1 init */
	outb_p(0x20 + 0, PIC_MASTER_IMR);	/* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27 */
	outb_p(1U << PIC_CASCADE_IR, PIC_MASTER_IMR);	/* 8259A-1 (the master) has a slave on IR2 */
	if (auto_eoi)	/* master does Auto EOI */
		outb_p(MASTER_ICW4_DEFAULT | PIC_ICW4_AEOI, PIC_MASTER_IMR);
	else		/* master expects normal EOI */
		outb_p(MASTER_ICW4_DEFAULT, PIC_MASTER_IMR);
	outb_p(0x11, PIC_SLAVE_CMD);	/* ICW1: select 8259A-2 init */
	outb_p(0x20 + 8, PIC_SLAVE_IMR);	/* ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2f */
	outb_p(PIC_CASCADE_IR, PIC_SLAVE_IMR);	/* 8259A-2 is a slave on master's IR2 */
	outb_p(SLAVE_ICW4_DEFAULT, PIC_SLAVE_IMR); /* (slave's support for AEOI in flat mode is to be investigated) */
	if (auto_eoi)
		/*
		 * in AEOI mode we just have to mask the interrupt
		 * when acking.
		 */
		i8259A_irq_type.ack = disable_8259A_irq;
	else
		i8259A_irq_type.ack = mask_and_ack_8259A;
/*smbody: to simplify implementation, don't use 8253 timer now*/
//	udelay(100);		/* wait for 8259A to initialize */
	io_wait();


	outb(cached_master_mask, PIC_MASTER_IMR); /* restore master IRQ mask */
	outb(cached_slave_mask, PIC_SLAVE_IMR);	  /* restore slave IRQ mask */

	spin_unlock_irqrestore(&i8259A_lock, flags);
}

/* smbody:should be puth into __init*/
void init_ISA_irqs (void)
{
	int i;

	init_8259A(0);

	for (i = 0; i < NR_IRQS; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;

		if (i < 16) {
			/*
			 * 16 old-style INTA-cycle interrupts:
			 */
			irq_desc[i].handler = &i8259A_irq_type;
		} else {
			/*
			 * 'high' PCI IRQs filled in on demand
			 */
			irq_desc[i].handler = &no_irq_type;
		}
	}
}

/* smbody:should be puth into __init*/
/*smbody: use mach-default configuration*/
void  init_IRQ(void)
{
    int i;
    
    init_ISA_irqs();
	/*smbody: enable keyboard interrupt*/
	enable_8259A_irq(0);
	enable_8259A_irq(1);

    for(i = 0; i < (NR_VECTORS - FIRST_EXTERNAL_VECTOR); i++){
        int vector = FIRST_EXTERNAL_VECTOR + i;
        if(i >= NR_IRQS)
            break;
        if(vector != SYSCALL_VECTOR)
            set_intr_gate(vector, interrupt[i]);
    }

	setup_pit_timer();

	intr_init_hook();
}


