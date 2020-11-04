#ifndef __TIMER_H__
#define __TIMER_H__

struct timer_opts {
	char* name;
	void (*mark_offset)(void);
	unsigned long (*get_offset)(void);
	unsigned long long (*monotonic_clock)(void);
	void (*delay)(unsigned long);
};

#endif