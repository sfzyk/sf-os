#ifndef _I386_CURRENT_H__
#define _I386_CURRENT_H__
#include <i386/thread.h>

struct task_struct;

static inline struct thread_info *current_thread_info(void)
{
	struct thread_info *ti;
	__asm__("andl %%esp,%0; ":"=r" (ti) : "0" (~(THREAD_SIZE - 1)));
	return ti;
}


static inline struct task_struct * get_current(void)
{
	return current_thread_info()->task;
}
 
#define current get_current()

#endif /* !(_I386_CURRENT_H) */
