#include <i386/timer.h>
#include <kernel/sync.h>
#include <kernel/time.h>
#include <i386/config.h>
#include <i386/irq.h>
#include <kernel/signal.h>

#include <kernel/tty.h>

extern spinlock_t i8259A_lock;

DEFINE_SPINLOCK(rtc_lock);
DEFINE_SPINLOCK(i8253_lock);

unsigned long tick_usec = TICK_USEC; 		/* USER_HZ period (usec) */
unsigned long tick_nsec = TICK_NSEC;		/* ACTHZ period (nsec) */

static struct init_timer_opts* timers[] = {
	&timer_pit_init,
	(void*)0,
};

static char clock_override[10] ;

unsigned long get_cmos_time(void)
{
	unsigned long retval;

	spin_lock(&rtc_lock);

	retval = mach_get_cmos_time();

	spin_unlock(&rtc_lock);

	return retval;
}

struct timer_opts* select_timer(void)
{
	int i = 0;
	
	/* find most preferred working timer */
	while (timers[i]) {
		if (timers[i]->init)
			if (timers[i]->init(clock_override) == 0)
				return timers[i]->opts;
		++i;
	}
		
//	panic("select_timer: Cannot find a suitable timer\n");
	return NULL;
}
/*
static inline void update_times(void)
{
	unsigned long ticks;

	ticks = jiffies - wall_jiffies;
	if (ticks) {
		wall_jiffies += ticks;
		update_wall_time(ticks);
	}
	calc_load(ticks);
}
*/

void do_timer(struct pt_regs *regs)
{
	jiffies_64++;
//	update_times();
}

/*smbody: used for print*/
unsigned long long last = 0;

irqreturn_t timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	/*
	 * Here we are in the timer irq handler. We just have irqs locally
	 * disabled but we don't know if the timer_bh is running on the other
	 * CPU. We need to avoid to SMP race with it. NOTE: we don' t need
	 * the irq version of write_lock because as just said we have irq
	 * locally disabled. -arca
	 */
	//write_seqlock(&xtime_lock);

	cur_timer->mark_offset();
	
	//do_timer_interrupt(irq, NULL, regs);
	do_timer(regs);
	/*smbody: used for print*/
	if(jiffies_64 - last >= 1000){
		terminal_initialize();
		last = jiffies_64;
	}else if(last > jiffies_64)
		last = jiffies_64;
	

	//write_sequnlock(&xtime_lock);
	return IRQ_HANDLED;
}



static struct irqaction irq0  = { timer_interrupt, SA_INTERRUPT, CPU_MASK_NONE, "timer", NULL, NULL};

void time_init_hook(void)
{
	setup_irq(0, &irq0);
}
