#include <kernel/multiboot.h>
#include <stdio.h>
#include <i386/page_flag.h>
#include <i386/page_table.h>
#include <i386/tlbflush.h>

extern pgd_t global_page_dir[1024];
extern pte_t  page_start;
struct page* mem_map;
void zone_sizes_init(void);
void init_page_table(pgd_t *pgd_base,pte_t* pte_base);

/*
    start_pfn 

*/
int start_pfn,max_pfn,max_low_pfn;

static inline void find_pfn_layout(multiboot_info_t *mdb){
    size_t mem_map_len = mdb->mmap_length ;
    multiboot_memory_map_t * mmap_pointer = mdb->mmap_addr;
    while(mem_map_len){
        if(mmap_pointer->type == MULTIBOOT_MEMORY_AVAILABLE){
            if(max_pfn > PFN_DOWN(mmap_pointer->addr + mmap_pointer->size))
                max_pfn = PFN_DOWN(mmap_pointer->addr+mmap_pointer->size);
        }
        mem_map_len -= mmap_pointer->size + 4; 
        mmap_pointer=(multiboot_memory_map_t *)(((char *)mmap_pointer )+ mmap_pointer->size + 4);
    }

}

void mm_page_init(multiboot_info_t* mdb){
    /* pfn staff */
    find_pfn_layout(mdb);

    init_page_table(global_page_dir, &page_start);

    laod_cr3(global_page_dir);

    __flush_tlb();

    zone_sizes_init();


    return ;
}