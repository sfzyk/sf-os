#include <i386/timer.h>
#include <kernel/time.h>
#include <i386/i8259.h>
#include <kernel/sync.h>
#include <i386/io.h>
#include <i386/irq.h>

extern spinlock_t i8253_lock;

static int count_p; /* counter in get_offset_pit() */

static int init_pit(char* override)
{
 	/* check clock override */
	 /*
 	if (override[0] && strncmp(override,"pit",3))
 		printk(KERN_ERR "Warning: clock= override failed. Defaulting to PIT\n");
	*/
	count_p = LATCH;
	return 0;
}

static void mark_offset_pit(void)
{
	/* nothing needed */
}

static unsigned long long monotonic_clock_pit(void)
{
	return 0;
}

static void delay_pit(unsigned long loops)
{
	int d0;
	__asm__ __volatile__(
		"\tjmp 1f\n"
		".align 16\n"
		"1:\tjmp 2f\n"
		".align 16\n"
		"2:\tdecl %0\n\tjns 2b"
		:"=&a" (d0)
		:"0" (loops));
}

static unsigned long get_offset_pit(void)
{
	int count;
	unsigned long flags;
	static unsigned long jiffies_p = 0;

	/*
	 * cache volatile jiffies temporarily; we have xtime_lock. 
	 */
	unsigned long jiffies_t;

	spin_lock_irqsave(&i8253_lock, flags);
	/* timer count may underflow right here */
	outb_p(0x00, PIT_MODE);	/* latch the count ASAP */

	count = inb_p(PIT_CH0);	/* read the latched count */

	/*
	 * We do this guaranteed double memory access instead of a _p 
	 * postfix in the previous port access. Wheee, hackady hack
	 */

	/*smbody:: use jiffies_64 instead*/
 	//jiffies_t = jiffies;
	jiffies_t = jiffies_64;

	count |= inb_p(PIT_CH0) << 8;
	
        /* VIA686a test code... reset the latch if count > max + 1 */
        if (count > LATCH) {
                outb_p(0x34, PIT_MODE);
                outb_p(LATCH & 0xff, PIT_CH0);
                outb(LATCH >> 8, PIT_CH0);
                count = LATCH - 1;
        }
	
	/*
	 * avoiding timer inconsistencies (they are rare, but they happen)...
	 * there are two kinds of problems that must be avoided here:
	 *  1. the timer counter underflows
	 *  2. hardware problem with the timer, not giving us continuous time,
	 *     the counter does small "jumps" upwards on some Pentium systems,
	 *     (see c't 95/10 page 335 for Neptun bug.)
	 */

	if( jiffies_t == jiffies_p ) {
		if( count > count_p ) {
			/* the nutcase */
			count = do_timer_overflow(count);
		}
	} else
		jiffies_p = jiffies_t;

	count_p = count;

	spin_unlock_irqrestore(&i8253_lock, flags);

	count = ((LATCH-1) - count) * TICK_SIZE;
	count = (count + LATCH/2) / LATCH;

	return count;
}

/* tsc timer_opts struct */
struct timer_opts timer_pit = {
	.name = "pit",
	.mark_offset = mark_offset_pit, 
	.get_offset = get_offset_pit,
	.monotonic_clock = monotonic_clock_pit,
	.delay = delay_pit,
};

struct init_timer_opts timer_pit_init = {
	.init = init_pit, 
	.opts = &timer_pit,
};

void setup_pit_timer(void)
{
	extern spinlock_t i8253_lock;
	unsigned long flags;

	spin_lock_irqsave(&i8253_lock, flags);
	outb_p(0x34,PIT_MODE);		/* binary, mode 2, LSB/MSB, ch 0 */
	//udelay(10);
	io_wait();
	outb_p(LATCH & 0xff , PIT_CH0);	/* LSB */
	//udelay(10);
	io_wait();
	outb(LATCH >> 8 , PIT_CH0);	/* MSB */
	spin_unlock_irqrestore(&i8253_lock, flags);
}