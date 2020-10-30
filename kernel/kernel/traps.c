#include <i386/ptrace.h>
#include <i386/thread.h>
#include <i386/current.h>
#include <kernel/signal.h>
#include <i386/linkage.h>
#include <i386/segment.h>

#define _set_gate(gate_addr,type,dpl,addr,seg) \
do { \
  int __d0, __d1; \
  __asm__ __volatile__ ("movw %%dx,%%ax\n\t" \
	"movw %4,%%dx\n\t" \
	"movl %%eax,%0\n\t" \
	"movl %%edx,%1" \
	:"=m" (*((long *) (gate_addr))), \
	 "=m" (*(1+(long *) (gate_addr))), "=&a" (__d0), "=&d" (__d1) \
	:"i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
	 "3" ((char *) (addr)),"2" ((seg) << 16)); \
} while (0)

extern struct desc_struct idt_table[256] __attribute__((__section__(".data")));
asmlinkage void divide_error(void);
asmlinkage void debug(void);
asmlinkage void nmi(void);
asmlinkage void int3(void);
asmlinkage void overflow(void);
asmlinkage void bounds(void);
asmlinkage void invalid_op(void);
asmlinkage void device_not_available(void);
asmlinkage void coprocessor_segment_overrun(void);
asmlinkage void invalid_TSS(void);
asmlinkage void segment_not_present(void);
asmlinkage void stack_segment(void);
asmlinkage void general_protection(void);
asmlinkage void page_fault(void);
asmlinkage void coprocessor_error(void);
asmlinkage void simd_coprocessor_error(void);
asmlinkage void alignment_check(void);
asmlinkage void spurious_interrupt_bug(void);

static void do_trap(int trapnr, int signr, char *str, struct pt_regs * regs, long error_code, siginfo_t *info)
{
	/* for now */

	struct task_struct *tsk = current;
	tsk->thread.error_code = error_code;
	tsk->thread.trap_no = trapnr;

	/*todo  send signal */
	// if (info)
	// 	force_sig_info(signr, info, tsk);
	// else
	// 	force_sig(signr, tsk);
	return;
 
}

#define DO_ERROR_INFO(trapnr, signr, str, name, sicode, siaddr) \
fastcall void do_##name(struct pt_regs * regs, long error_code) \
{ \
	siginfo_t info; \
	info.si_signo = signr; \
	info.si_errno = 0; \
	info.si_code = sicode; \
	info.si_addr =  siaddr; \
	do_trap(trapnr, signr, str, regs, error_code, &info); \
}

/*
	em... ... ...
*/
DO_ERROR_INFO(0, SIGFPE,  "divide error", divide_error, 0, regs->eip)
DO_ERROR_INFO( 4, SIGSEGV, "overflow", overflow, 0, regs->eip)
DO_ERROR_INFO( 5, SIGSEGV, "bounds", bounds, 0, regs->eip)
DO_ERROR_INFO( 6, SIGILL,  "invalid operand", ILL_ILLOPN,0, regs->eip)
DO_ERROR_INFO( 9, SIGFPE,  "coprocessor segment overrun", coprocessor_segment_overrun, 0, regs->eip)
DO_ERROR_INFO(10, SIGSEGV, "invalid TSS", invalid_TSS, 0, regs->eip)
DO_ERROR_INFO(11, SIGBUS,  "segment not present", segment_not_present, 0, regs->eip)
DO_ERROR_INFO(12, SIGBUS,  "stack segment", stack_segment, 0, regs->eip)
DO_ERROR_INFO(17, SIGBUS, "alignment check", alignment_check, 0, regs->eip)
 


void set_intr_gate(unsigned int n, void *addr)
{
	_set_gate(idt_table+n,14,0,addr,__KERNEL_CS);
}

/*
 * This routine sets up an interrupt gate at directory privilege level 3.
 */
static inline void set_system_intr_gate(unsigned int n, void *addr)
{
	_set_gate(idt_table+n, 14, 3, addr, __KERNEL_CS);
}

static void  set_trap_gate(unsigned int n, void *addr)
{
	_set_gate(idt_table+n,15,0,addr,__KERNEL_CS);
}

static void  set_system_gate(unsigned int n, void *addr)
{
	_set_gate(idt_table+n,15,3,addr,__KERNEL_CS);
}

static void  set_task_gate(unsigned int n, unsigned int gdt_entry)
{
	_set_gate(idt_table+n,5,0,0,(gdt_entry<<3));
}


void init_traps(){

	set_trap_gate(0,&divide_error);
	set_intr_gate(1,&debug);
	set_intr_gate(2,&nmi);
	set_system_intr_gate(3, &int3); /* int3-5 can be called from all */
	set_system_gate(4,&overflow);
	set_system_gate(5,&bounds);
	set_trap_gate(6,&invalid_op);
	set_trap_gate(7,&device_not_available);
	set_task_gate(8,GDT_ENTRY_DOUBLEFAULT_TSS);
	set_trap_gate(9,&coprocessor_segment_overrun);
	set_trap_gate(10,&invalid_TSS);
	set_trap_gate(11,&segment_not_present);
	set_trap_gate(12,&stack_segment);
	set_trap_gate(13,&general_protection);
	set_intr_gate(14,&page_fault);
	set_trap_gate(15,&spurious_interrupt_bug);
	set_trap_gate(16,&coprocessor_error);
	set_trap_gate(17,&alignment_check);
	set_trap_gate(19,&simd_coprocessor_error);
	// set_system_gate(SYSCALL_VECTOR,&system_call);
}
