#ifndef __HOOKS_H__
#define __HOOKS_H__

#include <i386/irq.h>

extern void init_ISA_irqs(void);
extern void apic_intr_init(void);
extern void smp_intr_init(void);
extern irqreturn_t timer_interrupt(int irq, void *dev_id, struct pt_regs *regs);

/* these are the defined hooks */
extern void intr_init_hook(void);
extern void pre_intr_init_hook(void);
extern void pre_setup_arch_hook(void);
extern void trap_init_hook(void);
extern void time_init_hook(void);
extern void mca_nmi_hook(void);

#endif