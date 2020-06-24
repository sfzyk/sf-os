#include <kernel/segment.h>
#include <i386/page.h>

.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.align 4096
pg0: 
.fill 2048,4,0 


# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	#change segment 
	lgdt cpu_gdt_descr
	mov $(__KERNEL_DS), %ecx 
	mov %ecx, %ds
	mov %ecx, %es
	mov %ecx, %fs
	mov %ecx, %gs
	mov %ecx, %ss
	ljmp $(__KERNEL_CS), $1f
1:
	movl $stack_top, %esp   
	pushl  %eax
	pushl  %ebx 
	#detected memory map

	# enable pageing 
	movl $pg0, %eax 
	movl $global_page_dir - 0xc0000000 ,%ebx
	leal 0x007(%eax), %edx  #page dir entry 
	movl %edx, (%ebx)
	movl %edx, 0xc00(%ebx) #that is 0xc00 not 0x300 costs me 3 hours, fx
1:
	movl $0x007, %eax 
	movl $pg0, %edi 
	cld 
	movl $1024, %ecx
2:
	stosl
	addl $0x1000, %eax 
	loop 2b

	movl $global_page_dir - 0xc0000000 ,%eax 
	movl %eax, %cr3 

	movl %cr0, %eax 
	orl  $0x80000000, %eax
	movl %eax, %cr0
	
	#init for idt table 
	call setup_ign_idt
	
	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
4:	hlt
	jmp 4b


# must be called after pageing enabled 
setup_ign_idt:
	movl $(__KERNEL_CS << 16), %eax 
	movl $ignore_int, %ebx 
	movw %bx, %ax 
	movw $0x8e00, %bx

	movl $256, %ecx 
	movl $idt_table, %edx
rep_set_idt:
	movl %eax, (%edx)
	movl %ebx, 4(%edx)
	addl $8,%edx
	loop rep_set_idt
	ret 

ignore_int:
	cld
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %es
	pushl %ds
	movl $(__KERNEL_DS),%eax
	movl %eax,%ds
	movl %eax,%es
	pushl 16(%esp)
	pushl 24(%esp)
	pushl 32(%esp)
	pushl 40(%esp)
	pushl $int_msg
	#call printk
	addl $(5*4),%esp
	popl %ds
	popl %es
	popl %edx
	popl %ecx
	popl %eax
	iret

.size _start, . - _start

.data
.global cpu_gdt_table
.global cpu_gdt_descr

int_msg:
	.asciz "Unknown interrupt or fault at EIP %p %p %p\n"
cpu_idt_descr:
	.word 256*8 - 1
	.long idt_table


cpu_gdt_descr:
	.word GDT_ENTRIES*8-1
	.long cpu_gdt_table


cpu_gdt_table:
	.quad 0x0000000000000000	/* NULL descriptor */
	.quad 0x0000000000000000	/* 0x0b reserved */
	.quad 0x0000000000000000	/* 0x13 reserved */
	.quad 0x0000000000000000	/* 0x1b reserved */
	.quad 0x0000000000000000	/* 0x20 unused */
	.quad 0x0000000000000000	/* 0x28 unused */
	.quad 0x0000000000000000	/* 0x33 TLS entry 1 */
	.quad 0x0000000000000000	/* 0x3b TLS entry 2 */
	.quad 0x0000000000000000	/* 0x43 TLS entry 3 */
	.quad 0x0000000000000000	/* 0x4b reserved */
	.quad 0x0000000000000000	/* 0x53 reserved */
	.quad 0x0000000000000000	/* 0x5b reserved */

	.quad 0x00cf9a000000ffff	/* 0x60 kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff	/* 0x68 kernel 4GB data at 0x00000000 */
	.quad 0x00cffa000000ffff	/* 0x73 user 4GB code at 0x00000000 */
	.quad 0x00cff2000000ffff	/* 0x7b user 4GB data at 0x00000000 */

	.quad 0x0000000000000000	/* 0x80 TSS descriptor */
	.quad 0x0000000000000000	/* 0x88 LDT descriptor */

	/* Segments used for calling PnP BIOS */
	.quad 0x00c09a0000000000	/* 0x90 32-bit code */
	.quad 0x00809a0000000000	/* 0x98 16-bit code */
	.quad 0x0080920000000000	/* 0xa0 16-bit data */
	.quad 0x0080920000000000	/* 0xa8 16-bit data */
	.quad 0x0080920000000000	/* 0xb0 16-bit data */
	/*
	 * The APM segments have byte granularity and their bases
	 * and limits are set at run time.
	 */
	.quad 0x00409a0000000000	/* 0xb8 APM CS    code */
	.quad 0x00009a0000000000	/* 0xc0 APM CS 16 code (16 bit) */
	.quad 0x0040920000000000	/* 0xc8 APM DS    data */

	.quad 0x0000000000000000	/* 0xd0 - unused */
	.quad 0x0000000000000000	/* 0xd8 - unused */
	.quad 0x0000000000000000	/* 0xe0 - unused */
	.quad 0x0000000000000000	/* 0xe8 - unused */
	.quad 0x0000000000000000	/* 0xf0 - unused */
	.quad 0x0000000000000000	/* 0xf8 - GDT entry 31: double-fault TSS */
