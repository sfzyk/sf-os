#include <kernel/linkage.h>
#include <i386/irq.h>
#include <i386/io.h>

#include <stdio.h>

/*smbody: should be aligned*/
irq_desc_t irq_desc[NR_IRQS] = {
    [0 ... NR_IRQS - 1] = {
        .handler = &no_irq_type,
        .lock = SPIN_LOCK_UNLOCKED
    }
};

/*
 * Generic 'no controller' code
 */
static void end_none(unsigned int irq) { }
static void enable_none(unsigned int irq) { }
static void disable_none(unsigned int irq) { }
static void shutdown_none(unsigned int irq) { }
static unsigned int startup_none(unsigned int irq) { return 0; }

static void ack_none(unsigned int irq)
{
	/*
	 * 'what should we do if we get a hw irq event on an illegal vector'.
	 * each architecture has to answer this themself.
	 */
    /* smbody: bad irqn*/
	//ack_bad_irq(irq);
}

struct hw_interrupt_type no_irq_type = {
	.typename = 	"none",
	.startup = 	startup_none,
	.shutdown = 	shutdown_none,
	.enable = 	enable_none,
	.disable = 	disable_none,
	.ack = 		ack_none,
	.end = 		end_none,
	.set_affinity = ((void *)0)
};


fastcall unsigned int __do_IRQ(unsigned int irq, struct pt_regs *regs)
{
	irq_desc[irq].handler->ack(irq);

	unsigned char scancode = inb(0x60);
	printf("%d %c\n",irq,scancode);

	irq_desc[irq].handler->enable(irq);
}