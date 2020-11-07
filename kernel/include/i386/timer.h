#ifndef __TIMER_H__
#define __TIMER_H__

#include <kernel/sync.h>
#include <i386/io.h>
#include <i386/i8259.h>

# define HZ		1000		/* Internal kernel timer frequency */
# define USER_HZ	100		/* .. some user interfaces are in "ticks" */
# define CLOCKS_PER_SEC		(USER_HZ)	/* like times() */

#ifndef HZ
#define HZ 100
#endif

#define CLOCK_TICK_RATE 1193182 /* Underlying HZ */

#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (-300*HZ))

extern unsigned long tick_usec;
extern unsigned long tick_nsec;

#define TICK_SIZE (tick_nsec / 1000)

/* LATCH is used in the interval timer and ftape setup. */
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)	/* For divider */

struct timer_opts {
	char* name;
	void (*mark_offset)(void);
	unsigned long (*get_offset)(void);
	unsigned long long (*monotonic_clock)(void);
	void (*delay)(unsigned long);
};

struct init_timer_opts {
	int (*init)(char *override);
	struct timer_opts *opts;
};

extern struct timer_opts* select_timer(void);
void setup_pit_timer(void);

extern struct timer_opts timer_none;
extern struct timer_opts timer_pit;
extern struct init_timer_opts timer_pit_init;

extern struct timer_opts *cur_timer;

static inline int do_timer_overflow(int count)
{
	int i;

	spin_lock(&i8259A_lock);
	/*
	 * This is tricky when I/O APICs are used;
	 * see do_timer_interrupt().
	 */
	i = inb(0x20);
	spin_unlock(&i8259A_lock);
	
	/* assumption about timer being IRQ0 */
	if (i & 0x01) {
		/*
		 * We cannot detect lost timer interrupts ... 
		 * well, that's why we call them lost, don't we? :)
		 * [hmm, on the Pentium and Alpha we can ... sort of]
		 */
		count -= LATCH;
	} 
	return count;
}


#endif