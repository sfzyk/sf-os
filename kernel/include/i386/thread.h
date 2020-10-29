#ifndef __THREAD_H__
#define __THREAD_H__
#include <kernel/list.h>
#include <i386/atomic.h>

#define THREAD_SIZE (4096)  
typedef unsigned int pid,uid_t,gid_t;
typedef unsigned int tgid;
typedef unsigned int cputime_t;
typedef unsigned int cpumask_t;

enum pid_type
{
	PIDTYPE_PID,
	PIDTYPE_TGID,
	PIDTYPE_PGID,
	PIDTYPE_SID,
	PIDTYPE_MAX
};

struct pid
{
	/* Try to keep pid_chain in the same cacheline as nr for find_pid */
	int nr;
	struct list_head pid_chain;
	/* list of pids with the same nr, only one of them is in the hash */
	struct list_head pid_list;
};

typedef struct prio_array{
    int nr_active;
    unsigned long bitmap[5];
    struct list_head head [140];
}prio_array_t;


struct thread_struct {
/* cached TLS descriptors. */
	// struct desc_struct tls_array[GDT_ENTRY_TLS_ENTRIES];
	unsigned long	esp0;
	unsigned long	sysenter_cs;
	unsigned long	eip;
	unsigned long	esp;
	unsigned long	fs;
	unsigned long	gs;
/* Hardware debugging registers */
	unsigned long	debugreg[8];  /* %%db0-7 debug registers */
/* fault info */
	unsigned long	cr2, trap_no, error_code;
// /* floating point info */
// 	union i387_union	i387;
// /* virtual 86 mode info */
// 	struct vm86_struct __user * vm86_info;
	unsigned long		screen_bitmap;
	unsigned long		v86flags, v86mask, saved_esp0;
	unsigned int		saved_fs, saved_gs;
/* IO permissions */
	unsigned long	*io_bitmap_ptr;
/* max allowed port in the bitmap, in bytes: */
	unsigned long	io_bitmap_max;
};

struct task_struct {
	volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	volatile long preempt_count;
	struct thread_info *thread_info;
	atomic_t usage;
	unsigned long flags;	/* per process flags, defined below */
	unsigned long ptrace;

	int lock_depth;		/* Lock depth */

	int prio, static_prio;
	struct list_head run_list;
	prio_array_t *array;

	unsigned long sleep_avg;
	unsigned long long timestamp, last_ran;
	int activated;

	unsigned long policy;
	cpumask_t cpus_allowed;
	unsigned int time_slice, first_time_slice;
	
	struct thread_struct thread;
	
	struct list_head tasks;
	/*
	 * ptrace_list/ptrace_children forms the list of my children
	 * that were stolen by a ptracer.
	 */
	struct list_head ptrace_children;
	struct list_head ptrace_list;

// struct mm_struct *mm, *active_mm;

/* task state */
	struct linux_binfmt *binfmt;
	long exit_state;
	int exit_code, exit_signal;
	int pdeath_signal;  /*  The signal sent when the parent dies  */
	/* ??? */
	unsigned long personality;
	unsigned did_exec:1;
	pid pid;
	pid tgid;
	/* 
	 * pointers to (original) parent process, youngest child, younger sibling,
	 * older sibling, respectively.  (p->father can be replaced with 
	 * p->parent->pid)
	 */
	struct task_struct *real_parent; /* real parent process (when being debugged) */
	struct task_struct *parent;	/* parent process */
	/*
	 * children/sibling forms the list of my children plus the
	 * tasks I'm ptracing.
	 */
	struct list_head children;	/* list of my children */
	struct list_head sibling;	/* linkage in my parent's children list */
	struct task_struct *group_leader;	/* threadgroup leader */

	/* PID/PID hash table linkage. */
	struct pid pids[PIDTYPE_MAX];

	struct completion *vfork_done;		/* for vfork() */

	unsigned long rt_priority;
	unsigned long it_real_value, it_real_incr;
	cputime_t it_virt_value, it_virt_incr;
	cputime_t it_prof_value, it_prof_incr;
	// struct timer_list real_timer;
	cputime_t utime, stime;
	unsigned long nvcsw, nivcsw; /* context switch counts */
	// struct timespec start_time;
/* mm fault and swap info: this can arguably be seen as either mm-specific or thread-specific */
	unsigned long min_flt, maj_flt;
/* process credentials */
	uid_t uid,euid,suid,fsuid;
	gid_t gid,egid,sgid,fsgid;
	struct group_info *group_info;

	// kernel_cap_t   cap_effective, cap_inheritable, cap_permitted;
	unsigned keep_capabilities:1;
	struct user_struct *user;
 
	struct fs_struct *fs;
/* open file information */
	struct files_struct *files;
/* namespace */
	struct namespace *namespace;
/* signal handlers */
	struct signal_struct *signal;
	struct sighand_struct *sighand;

};


struct thread_info {
	struct task_struct	*task;		/* main task structure */
	 
	unsigned long	flags;		/* low level flags */
	unsigned long	status;		/* thread-synchronous flags */
 
	unsigned long   previous_esp;   /* ESP of the previous stack in case
						   of nested (IRQ) stacks
						*/
    unsigned long magic;
};

#define THREAD_MAGIC (0x5f5f5f5f)


void thread_info();

#endif 