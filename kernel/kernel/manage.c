#include <i386/irq.h>
#include <kernel/errno.h>
#include <kernel/signal.h>
#include <i386/config.h>

int setup_irq(unsigned int irq, struct irqaction * new)
{
	struct irq_desc *desc = irq_desc + irq;
	struct irqaction *old, **p;
	unsigned long flags;
	int shared = 0;

	if (desc->handler == &no_irq_type)
		return -ENOSYS;
	/*
	 * Some drivers like serial.c use request_irq() heavily,
	 * so we have to be careful not to interfere with a
	 * running system.
	 */
	if (new->flags & SA_SAMPLE_RANDOM) {
		/*
		 * This function might sleep, we want to call it first,
		 * outside of the atomic block.
		 * Yes, this might clear the entropy pool if the wrong
		 * driver is attempted to be loaded, without actually
		 * installing a new handler, but is this really a problem,
		 * only the sysadmin is able to do this.
		 */
		//rand_initialize_irq(irq);
	}

	/*
	 * The following block of code has to be executed atomically
	 */
	spin_lock_irqsave(&desc->lock,flags);
	p = &desc->action;
	if ((old = *p) != NULL) {
		/* Can't share interrupts unless both agree to */
		if (!(old->flags & new->flags & SA_SHIRQ)) {
			spin_unlock_irqrestore(&desc->lock,flags);
			return -EBUSY;
		}

		/* add new interrupt at end of irq queue */
		do {
			p = &old->next;
			old = *p;
		} while (old);
		shared = 1;
	}

	*p = new;

	if (!shared) {
		desc->depth = 0;
		desc->status &= ~(IRQ_DISABLED | IRQ_AUTODETECT |
				  IRQ_WAITING | IRQ_INPROGRESS);
		if (desc->handler->startup)
			desc->handler->startup(irq);
		else
			desc->handler->enable(irq);
	}
	spin_unlock_irqrestore(&desc->lock,flags);

	new->irq = irq;
//	register_irq_proc(irq);
	new->dir = NULL;
//	register_handler_proc(irq, new);

	return 0;
}