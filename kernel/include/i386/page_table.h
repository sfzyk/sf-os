#ifndef __PAGE_TABLE_H__
#define __PAGE_TABLE_H__
#include <i386/page.h>

typedef struct { unsigned long pgprot; } pgprot_t;
#define __pgprot(x)	((pgprot_t) { (x) } )

/*
 * The 4MB page is guessing..  Detailed in the infamous "Chapter H"
 * of the Pentium details, but assuming intel did the straightforward
 * thing, this bit set in the page directory entry just means that
 * the page directory entry points directly to a 4MB-aligned block of
 * memory. 
 */
#define _PAGE_BIT_PRESENT	0
#define _PAGE_BIT_RW		1
#define _PAGE_BIT_USER		2
#define _PAGE_BIT_PWT		3
#define _PAGE_BIT_PCD		4
#define _PAGE_BIT_ACCESSED	5
#define _PAGE_BIT_DIRTY		6
#define _PAGE_BIT_PSE		7	/* 4 MB (or 2MB) page, Pentium+, if present.. */
#define _PAGE_BIT_GLOBAL	8	/* Global TLB entry PPro+ */
#define _PAGE_BIT_UNUSED1	9	/* available for programmer */
#define _PAGE_BIT_UNUSED2	10
#define _PAGE_BIT_UNUSED3	11
#define _PAGE_BIT_NX		63

#define _PAGE_PRESENT	0x001
#define _PAGE_RW	0x002
#define _PAGE_USER	0x004
#define _PAGE_PWT	0x008
#define _PAGE_PCD	0x010
#define _PAGE_ACCESSED	0x020
#define _PAGE_DIRTY	0x040
#define _PAGE_PSE	0x080	/* 4 MB (or 2MB) page, Pentium+, if present.. */
#define _PAGE_GLOBAL	0x100	/* Global TLB entry PPro+ */
#define _PAGE_UNUSED1	0x200	/* available for programmer */
#define _PAGE_UNUSED2	0x400
#define _PAGE_UNUSED3	0x800

#define _PAGE_FILE	0x040	/* set:pagecache unset:swap */
#define _PAGE_PROTNONE	0x080	/* If not present */

#define _PAGE_NX	0 
 
#define _PAGE_KERNEL \
	(_PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED | _PAGE_NX)
#define _PAGE_KERNEL_EXEC \
	(_PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED)


#define PAGE_NONE \
	__pgprot(_PAGE_PROTNONE | _PAGE_ACCESSED)
#define PAGE_SHARED \
	__pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED)

#define PAGE_SHARED_EXEC \
	__pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_COPY_NOEXEC \
	__pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED | _PAGE_NX)
#define PAGE_COPY_EXEC \
	__pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_COPY \
	PAGE_COPY_NOEXEC
#define PAGE_READONLY \
	__pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED | _PAGE_NX)
#define PAGE_READONLY_EXEC \
	__pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED)

 

typedef struct {unsigned long pgd;}pgd_t;
typedef struct {unsigned long pte;}pte_t;
typedef unsigned long pte;


#define _pte(x)  ((pte_t){(x)})
#define _pgd(x)  ((pgd_t){(x)})

#define PGDIR_SHIFT (22)
#define PTRS_PER_PGD (1<<10)
#define PTRS_PER_PTE (1<<10)

#define pgd_index(address) (((address)>>PGDIR_SHIFT)&(PTRS_PER_PGD - 1))


#define pfn_pte(pfn, v) _pte(pfn<<PAGE_SHIFT|v) 

#define set_pte(a,b) (*a = b)
#define set_pgd(a,b) (*a = b)

#define laod_cr3(pgdir) \
    __asm__ volatile("movl %0,%%cr3"::"r"(__pa(pgdir)))
#endif 
