#include <kernel/segment.h>
#include <kernel/linkage.h>
#include <i386/irq_vectors.h>

EBX		= 0x00
ECX		= 0x04
EDX		= 0x08
ESI		= 0x0C
EDI		= 0x10
EBP		= 0x14
EAX		= 0x18
DS		= 0x1C
ES		= 0x20
ORIG_EAX	= 0x24
EIP		= 0x28
CS		= 0x2C
EFLAGS		= 0x30
OLDESP		= 0x34
OLDSS		= 0x38

#define SAVE_ALL \
	cld; \
	pushl %es; \
	pushl %ds; \
	pushl %eax; \
	pushl %ebp; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx; \
	movl $(__USER_DS), %edx; \
	movl %edx, %ds; \
	movl %edx, %es;

# hard interrupt
.data
ENTRY(interrupt)
.text

vector = 0
ENTRY(irq_entries_start)
.rept NR_IRQS
    ALIGN
1: pushl $vector-256
    jmp common_interrupt
.data
    .long 1b
.text
vector=vector+1
.endr

    ALIGN
common_interrupt:
    SAVE_ALL
    movl %esp, %eax
    call do_IRQ
    jmp ret_from_intr


ENTRY(divide_error)
    pushl $0               # 0x24
    pushl $do_divide_error # 0x20 ? POP TO ES 
    jmp error_code

ENTRY(overflow)
	pushl $0
	pushl $do_overflow
	jmp error_code

ENTRY(bounds)
	pushl $0
	pushl $do_bounds
	jmp error_code

ENTRY(invalid_op)
	pushl $0
	pushl $do_invalid_op
	jmp error_code

ENTRY(coprocessor_segment_overrun)
	pushl $0
	pushl $do_coprocessor_segment_overrun
	jmp error_code

ENTRY(invalid_TSS)
	pushl $do_invalid_TSS
	jmp error_code

ENTRY(segment_not_present)
	pushl $do_segment_not_present
	jmp error_code

ENTRY(stack_segment)
	pushl $do_stack_segment
	jmp error_code

ENTRY(general_protection)
	pushl $do_general_protection
	jmp error_code

ENTRY(alignment_check)
	pushl $do_alignment_check
	jmp error_code

ENTRY(page_fault)
	pushl $do_page_fault
	jmp error_code

ret_from_exception: # to do preempt_stop ?
    cli # preemption
ret_from_intr:
    movl $-8096,%ebp
    andl %esp, %ebp
    movl EFLAGS(%esp), %eax
    movb CS(%esp), %al 
    testl $0x00020003, %eax
    jnz resume_userspace
    jmp resume_kernel 
resume_kernel:
    # cli   /* smbody: never disable interrupt without preempt*/
    
    # cmpl $0, 0x14(%esp) ;??? 
    jz need_resched # 如果要调度， 那么恢复的这个内核路径就一定不是中断处理程序
    jmp restore_all

resume_userspace: # todo a lots work for pending work 
    jmp restore_all

restore_all:
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    addl $4, %esp 
    iret 

need_resched:
    # movl $x(%ebp), %ecx 
    # testb $(1 << TIF_NEED_RESCHED), %cl
    jmp restore_all #  没有需要抢占的任务
    # //抢占的关键代码
    # // todo 

error_code:
  
    pushl %ds  # 0x1c
    pushl %eax # 0x18
    pushl %ebp # 0x14
    pushl %edi # 0x10
    pushl %esi # 0xc
    pushl %edx # 0x8

    xorl %eax, %eax
    decl %eax

    pushl %ecx # 0x4
    pushl %ebx # 0
    cld 
    movl %es, %ecx 
    movl ES(%esp), %edi
    movl ORIG_EAX(%esp), %edx # orign error code 
    #smbody: why not set  (%esp + ORIG_EAX) = -1 ??????
    movl %eax, ORIG_EAX(%esp)
    movl %ecx, ES(%esp)

    movl $(__USER_DS), %ecx
    movl %ecx,%ds
    movl %ecx,%es
    movl %esp, %eax
    call *%edi
    jmp ret_from_exception


