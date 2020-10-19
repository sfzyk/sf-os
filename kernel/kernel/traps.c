#include <i386/ptrace.h>
#include <i386/thread.h>
#include <i386/current.h>
#define DO_VM86_ERROR_INFO(trapnr, signr, str, name, sicode, siaddr)



 


static void do_trap(int trapnr, int signr, char *str, struct pt_regs * regs, long error_code, siginfo_t *info)
{
	 
	if (!(regs->xcs & 3))
		goto kernel_trap;

	trap_signal: {
		struct task_struct *tsk = current;
		tsk->thread.error_code = error_code;
		tsk->thread.trap_no = trapnr;
		if (info)
			force_sig_info(signr, info, tsk);
		else
			force_sig(signr, tsk);
		return;
	}

	kernel_trap: {
		if (!fixup_exception(regs))
			die(str, regs, error_code);
		return;
	}

	vm86_trap: {
		int ret = handle_vm86_trap((struct kernel_vm86_regs *) regs, error_code, trapnr);
		if (ret) goto trap_signal;
		return;
	}
}