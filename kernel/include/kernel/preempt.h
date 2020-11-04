#ifndef __PREEMPT_H__
#define __PREEMPT_H__

/* smbody: adding #ifdef for temporal preempt*/
#ifdef CONFIG_PREEMPT
#define preempt_disable() \
do { \
	inc_preempt_count(); \
	barrier(); \
} while (0)

#define preempt_enable() \
do { \
	preempt_enable_no_resched(); \
	preempt_check_resched(); \
} while (0)
#else

#define preempt_disable()
#define preempt_enable()

#endif

#endif