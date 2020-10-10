#include <i386/page.h>
#include <i386/page_table.h>
#include <i386/page_flag.h>

pgd_t global_page_dir[1024] __attribute__((__section__(".bss.pagedir")))= { {0}, }; 


  unsigned long virt_to_phys(void * address){
    return __pa(address);
}


   void *lowmem_page_address(struct page *page)
{
    return __va(page_to_pfn(page) << PAGE_SHIFT);
}

void *page_address(struct page *page)
{
    unsigned long flags;
    void *ret;
    struct page_address_slot *pas;

    if (!PageHighMem(page))
        return lowmem_page_address(page);

}

 