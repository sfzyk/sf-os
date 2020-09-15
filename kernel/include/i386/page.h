#ifndef __I386_PAGE_H__
#define __I386_PAGE_H__


#define page_offset (0xc0000000)
#define PAGE_SHIFT 12 
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE-1))

#ifndef __ASM__
    #include <i386/mm.h>
    #define PAGE_OFFSET		((unsigned long)page_offset)
    
    #define __pa(x)			((unsigned long)(x)-PAGE_OFFSET)
    #define __va(x)			((void *)((unsigned long)(x)+PAGE_OFFSET))
    #define pfn_to_kaddr(pfn)      __va((pfn) << PAGE_SHIFT)
    #define kaddr_to_pfn(kaddr)    ((__pa(kaddr)) >> PAGE_SHIFT)

    extern struct page* mem_map;
    #define pfn_to_page(pfn)	(mem_map + (pfn))
    #define page_to_pfn(page)	((unsigned long)((page) - mem_map))

    static inline unsigned long virt_to_phys(void * address){
        return __pa(address);
    }

#endif 
#endif 