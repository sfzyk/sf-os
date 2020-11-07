#include <i386/timer.h>
#include <kernel/time.h>

struct timespec xtime __attribute__ ((aligned (16)));
struct timespec wall_to_monotonic __attribute__ ((aligned (16)));

unsigned long long jiffies_64 = INITIAL_JIFFIES;

struct timer_opts *cur_timer = &timer_none;

/* smbody: should be put into .init segment*/
void time_init(void)
{
    xtime.tv_sec = get_cmos_time();
	xtime.tv_nsec = (INITIAL_JIFFIES % HZ) * (NSEC_PER_SEC / HZ);
	set_normalized_timespec(&wall_to_monotonic,
		-xtime.tv_sec, -xtime.tv_nsec);

	cur_timer = select_timer();
	//printk(KERN_INFO "Using %s for high-res timesource\n",cur_timer->name);

	time_init_hook();
}

unsigned long long get_jiffies_64(void)
{
	unsigned long seq;
	unsigned long long ret;
	ret = jiffies_64;
/*
	do {
		seq = read_seqbegin(&xtime_lock);
		
	} while (read_seqretry(&xtime_lock, seq));
	*/
	return ret;
}