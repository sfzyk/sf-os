#include <kernel/multiboot.h>
#include <stdio.h>
#include <i386/page_flag.h>
/*
    start_pfn : 

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
    





}