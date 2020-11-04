#include <kernel/linkage.h>
#include <i386/ptrace.h>
#include <i386/atomic.h>

atomic_t irq_err_count;

fastcall unsigned int do_IRQ(struct pt_regs *regs){
    /* high bits used in ret_from_ code */
	unsigned int irq = regs->orig_eax & 0xff;
    printf("%ud",irq);
//    irq_enter();

    __do_IRQ(irq, regs);

//    irq_exit();
}