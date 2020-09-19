 
/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */

ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;
 
	__kernel_physical_address_start = . ;

	.boottext :
	{
		arch/i386/boot.o (.multiboot)
		arch/i386/boot.o (.text)
	}
	
	.bootrodata :
	{
		arch/i386/boot.o (.rodata)
	}

	.bootdata :
	{
		arch/i386/boot.o (.data)
	}

	.bootbss :
	{
		arch/i386/boot.o (.bss)
		arch/i386/boot.o (COMMON)
	}

	_boot_end = . ;

	. += 0xc0000000 ; 
	__kernel_virtual_address_start = . ;


	.text BLOCK(4K) :  AT(_boot_end) ALIGN(4K)
	{
		*(.text)
	}
	
	 
	.rodata BLOCK(4K) : AT(((LOADADDR(.text)+SIZEOF(.text) + 0xfff) & 0xcffff000)) ALIGN(4K)
	{
		*(.rodata)
	}


	.data BLOCK(4K) :  AT(((LOADADDR(.rodata)+SIZEOF(.rodata) + 0xfff) & 0xcffff000)) ALIGN(4K) 
	{
		*(.data)
	}

	
	.bss BLOCK(4K) : AT( (LOADADDR(.data) + SIZEOF(.data)+ 0xfff)&0xcffff000 ) ALIGN(4K)  
	{
		*(.bss.pagedir)   /* page table must aligned at 256 byte at least : please see i386 manual */
		*(.bss)
		*(COMMON)	
	}
	
	 
	__kernel_virtual_address_end = . ;

	page_start = .;  
	/*temperal page table just two table for first 8M memory */
	/* The compiler may produce other sections, put them in the proper place in
	   in this file, if you'd like to include them in the final kernel. */
}
