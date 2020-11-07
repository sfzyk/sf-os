#include <kernel/linkage.h>
#include <i386/irq.h>
#include <i386/io.h>
#include <i386/config.h>
#include <kernel/signal.h>

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
	.set_affinity = NULL
};

fastcall int handle_IRQ_event(unsigned int irq, struct pt_regs *regs,
				struct irqaction *action)
{
	int ret, retval = 0, status = 0;

	if (!(action->flags & SA_INTERRUPT))
		local_irq_enable();

	do {
		ret = action->handler(irq, action->dev_id, regs);
		if (ret == IRQ_HANDLED)
			status |= action->flags;
		retval |= ret;
		action = action->next;
	} while (action);
/*
	if (status & SA_SAMPLE_RANDOM)
		add_interrupt_randomness(irq);
*/
	local_irq_disable();

	return retval;
}

fastcall unsigned int __do_IRQ(unsigned int irq, struct pt_regs *regs)
{
	irq_desc_t *desc = irq_desc + irq;
	struct irqaction * action;
	unsigned int status;

//	kstat_this_cpu.irqs[irq]++;
	if (desc->status & IRQ_PER_CPU) {
		irqreturn_t action_ret;

		/*
		 * No locking required for CPU-local interrupts:
		 */
		desc->handler->ack(irq);
		action_ret = handle_IRQ_event(irq, regs, desc->action);
		/*
		if (!noirqdebug)
			note_interrupt(irq, desc, action_ret);
		*/
		desc->handler->end(irq);
		return 1;
	}

	spin_lock(&desc->lock);
	desc->handler->ack(irq);

	/*
	* smbody: todo: IRQ is disabled for whatever reason
	*/
	return 1;
}