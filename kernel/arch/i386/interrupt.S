#include <i386/segment.h>

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

ENTRY(divide_error)
    pushl $0               # 0x24
    pushl $do_divide_error # 0x20
    jmp error_code

ret_from_exception: # to do preempt_stop ?
ret_from intr:
    

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
    movl $(__USER_DS), %ecx
    movl %esp, %eax
    call *%edi
    jmp ret_from_exception