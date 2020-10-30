#ifndef __SIGNAL_H__
#define __SIGNAL_H__


#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
/*
#define SIGLOST		29
*/
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31

/* These should not be considered constants from userland.  */
#define SIGRTMIN	32
#define SIGRTMAX	_NSIG

typedef struct siginfo {
	int si_signo;
	int si_errno;
	int si_code;
    int si_addr;

	union {
		// int _pad[SI_PAD_SIZE];

		/* kill() */
		struct {
			unsigned int  _pid;	/* sender's pid */
			unsigned int  _uid;	/* sender's uid */
		} _kill;

		/* POSIX.1b timers */
		struct {
			unsigned int  _tid;	/* timer id */
			int _overrun;		/* overrun count */
			// char _pad[sizeof( __ARCH_SI_UID_T) - sizeof(int)]; /* ? */
			unsigned int  _sigval;	/* same as below */
			int _sys_private;       /* not to be passed to user */
		} _timer;

		/* POSIX.1b signals */
		struct {
			unsigned int  _pid;	/* sender's pid */
			unsigned int  _uid;	/* sender's uid */
			unsigned int  _sigval;
		} _rt;

		/* SIGCHLD */
		struct {
			unsigned int _pid;	/* which child */
			unsigned int _uid;	/* sender's uid */
			int _status;		/* exit code */
			unsigned int  _utime;
			unsigned int  _stime;
		} _sigchld;

		/* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
		struct {
			void *_addr; /* faulting insn/memory ref. */
 
			int _trapno;	/* TRAP # which caused the signal */
 
			short _addr_lsb; /* LSB of the reported address */
			struct {
				void *_lower;
				void *_upper;
			} _addr_bnd;
		} _sigfault;

		// /* SIGPOLL */
		// struct {
		// 	__ARCH_SI_BAND_T _band;	/* POLL_IN, POLL_OUT, POLL_MSG */
		// 	int _fd;
		// } _sigpoll;

		/* SIGSYS */
		struct {
			void *_call_addr; /* calling user insn */
			int _syscall;	/* triggering system call number */
			unsigned int _arch;	/* AUDIT_ARCH_* of syscall */
		} _sigsys;
	} _sifields;
} siginfo_t;

#endif 