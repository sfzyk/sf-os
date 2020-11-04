#include <i386/timer.h>
#include <kernel/sync.h>
/*smbody: ?? haven't been used*/
//extern struct timer_opts* timer;

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

/* tsc timer_opts struct */
struct timer_opts timer_pit = {
	.name = "pit",
	.mark_offset = mark_offset_pit, 
	.get_offset = get_offset_pit,
	.monotonic_clock = monotonic_clock_pit,
	.delay = delay_pit,
};

void __delay(unsigned long loops)
{
	timer_pit->delay(loops);
}

inline void __const_udelay(unsigned long xloops)
{
	int d0;
	xloops *= 4;
	__asm__("mull %0"
		:"=d" (xloops), "=&a" (d0)
		:"1" (xloops),"0" (cpu_data[_smp_processor_id()].loops_per_jiffy * (HZ/4)));
        __delay(++xloops);
}

void __udelay(unsigned long usecs)
{
	__const_udelay(usecs * 0x000010c7);  /* 2**32 / 1000000 (rounded up) */
}

void __ndelay(unsigned long nsecs)
{
	__const_udelay(nsecs * 0x00005);  /* 2**32 / 1000000000 (rounded up) */
}