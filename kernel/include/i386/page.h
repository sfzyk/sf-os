#ifndef __I386_PAGE_H__
#define __I386_PAGE_H__

#define page_offset (0xc00000000)

#define PAGE_SHIFT 12 
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE-1))


#endif 