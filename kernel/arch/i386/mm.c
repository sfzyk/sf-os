#include <kernel/multiboot.h>
#include <stdio.h>
#include <i386/page_flag.h>
#include <i386/page_table.h>
#include <i386/tlbflush.h>
#include <kernel/list.h>


extern pgd_t global_page_dir[1024];
extern pte_t *page_start;
extern struct pglist_data global_mem_node;
struct page* mem_map;
struct zone* zone_table[1 <<(MAX_NR_ZONES *MAX_NUMNODES)+1]; // ? 
void zone_sizes_init(void);
void init_page_table(pgd_t *pgd_base,pte_t* pte_base);

/*
    start_pfn 

*/
int max_pfn,max_low_pfn,start_pfn;

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

    /*  
        decided here 
    */

   max_low_pfn = PFN_DOWN(MAX_LOW_MEM);
}


void zone_init_free_lists(struct pglist_data *pgdat)
{
	int order;
    struct zone * z= pgdat->node_zones;
    for( unsigned int i = 0;i< MAX_NR_ZONES;i++){
        struct zone* zone = z+i;
        for (order = 0; order < MAX_ORDER ; order++) {
            init_list(&zone->free_area[order].head);
            zone->free_area[order].nr_free = 0;
        }
    }
}

void mm_page_init(multiboot_info_t* mdb){
    /* pfn staff */
    find_pfn_layout(mdb);

    page_start = ((unsigned long)&page_start + (~PAGE_MASK) ) & (PAGE_MASK);

    init_page_table(global_page_dir, page_start);


    laod_cr3(global_page_dir);

    __flush_tlb();

    zone_init_free_lists(&global_mem_node);
    /*
    *  buddy system and page zone allocator init  
    */
    zone_sizes_init();

    init_build_zonelist(&global_mem_node);

    printf("page management have been completed!!\n");

    return ;
}
