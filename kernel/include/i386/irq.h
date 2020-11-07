#ifndef __IRQ_H__
#define __IRQ_H__

#include <i386/smp.h>
#include <kernel/linkage.h>
#include <i386/ptrace.h>
#include <i386/irq_vectors.h>

#include <kernel/types.h>
#include <kernel/util.h>
#include <i386/atomic.h>
#include <kernel/sync.h>

typedef int irqreturn_t;
typedef struct { DECLARE_BITMAP(bits, NR_CPUS); } cpumask_t;

#define CPU_MASK_ALL							\
(cpumask_t) { {								\
	[0 ... BITS_TO_LONGS(NR_CPUS)-2] = ~0UL,			\
	[BITS_TO_LONGS(NR_CPUS)-1] = CPU_MASK_LAST_WORD			\
} }

#define CPU_MASK_NONE							\
(cpumask_t) { {								\
	[0 ... BITS_TO_LONGS(NR_CPUS)-1] =  0UL				\
} }

#define CPU_MASK_CPU0							\
(cpumask_t) { {								\
	[0] =  1UL							\
} }

#define IRQ_NONE	(0)
#define IRQ_HANDLED	(1)
#define IRQ_RETVAL(x)	((x) != 0)

struct irqaction {
	irqreturn_t (*handler)(int, void *, struct pt_regs *);
	unsigned long flags;
	cpumask_t mask;
	const char *name;
	void *dev_id;
	struct irqaction *next;
	int irq;
	struct proc_dir_entry *dir;
};

extern irqreturn_t no_action(int cpl, void *dev_id, struct pt_regs *regs);
extern int request_irq(unsigned int,
		       irqreturn_t (*handler)(int, void *, struct pt_regs *),
		       unsigned long, const char *, void *);
extern void free_irq(unsigned int, void *);

extern void disable_irq_nosync(unsigned int irq);
extern void disable_irq(unsigned int irq);
extern void enable_irq(unsigned int irq);

struct hw_interrupt_type {
	const char * typename;
	unsigned int (*startup)(unsigned int irq);
	void (*shutdown)(unsigned int irq);
	void (*enable)(unsigned int irq);
	void (*disable)(unsigned int irq);
	void (*ack)(unsigned int irq);
	void (*end)(unsigned int irq);
	void (*set_affinity)(unsigned int irq, cpumask_t dest);
};

typedef struct hw_interrupt_type  hw_irq_controller;

typedef struct irq_desc {
	hw_irq_controller *handler;
	void *handler_data;
	struct irqaction *action;	/* IRQ action list */
	unsigned int status;		/* IRQ status */
	unsigned int depth;		/* nested irq disables */
	unsigned int irq_count;		/* For detecting broken interrupts */
	unsigned int irqs_unhandled;
	spinlock_t lock;
} irq_desc_t; /* smbody: should be aligned*/

extern int setup_irq(unsigned int irq, struct irqaction * new);

/*
 * IRQ line status.
 */
#define IRQ_INPROGRESS	1	/* IRQ handler active - do not enter! */
#define IRQ_DISABLED	2	/* IRQ disabled - do not enter! */
#define IRQ_PENDING	4	/* IRQ pending - replay on enable */
#define IRQ_REPLAY	8	/* IRQ has been replayed but not acked yet */
#define IRQ_AUTODETECT	16	/* IRQ is being autodetected */
#define IRQ_WAITING	32	/* IRQ not yet seen - for autodetection */
#define IRQ_LEVEL	64	/* IRQ level triggered */
#define IRQ_MASKED	128	/* IRQ masked - shouldn't be seen again */
#define IRQ_PER_CPU	256	/* IRQ is per CPU */

extern irq_desc_t irq_desc [NR_IRQS];
extern hw_irq_controller no_irq_type;  /* needed in every arch ? */
extern atomic_t irq_err_count;
/*smbody: didn't find extern, added by myself*/
extern long interrupt[NR_IRQS];

extern fastcall unsigned int __do_IRQ(unsigned int irq, struct pt_regs *regs);

/* For spinlocks etc */
#define local_irq_save(x)	__asm__ __volatile__("pushfl ; popl %0 ; cli":"=g" (x): /* no input */ :"memory")

#define local_irq_restore(x) do {typecheck(unsigned long,x); __asm__ __volatile__("pushl %0 ; popfl": /* no output */ :"g" (x):"memory", "cc");} while (0)

#define spin_lock_irqsave(lock, flags)	_spin_lock_irqsave(lock, flags)
#define spin_unlock_irqrestore(lock, flags)	_spin_unlock_irqrestore(lock, flags)

/* smbody : delete __acquire(lock); */
#define _spin_lock_irqsave(lock, flags) \
do {	\
	local_irq_save(flags); \
	preempt_disable(); \
	_raw_spin_lock(lock); \
} while (0)

/* smbody : delete __release(lock); */
#define _spin_unlock_irqrestore(lock, flags) \
do { \
	_raw_spin_unlock(lock); \
	local_irq_restore(flags); \
	preempt_enable(); \
} while (0)

#define local_irq_disable() 	__asm__ __volatile__("cli": : :"memory")
#define local_irq_enable()	__asm__ __volatile__("sti": : :"memory")

#endif