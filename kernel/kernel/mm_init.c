#include <i386/page.h>
#include <i386/mm.h>
#include <i386/dma.h>
#include <i386/page_table.h>
#include <i386/page_flag.h>
#include <i386/atomic.h>


extern int start_pfn,max_pfn,max_low_pfn;
extern pte_t page_start;
extern unsigned int __kernel_virtual_address_end;
extern struct page* mem_map;
struct pglist_data global_mem_node;
pte_t * page_desc_start;

/*
* kernel text include kernel data for now 
*
*
*/
int is_kernel_text(unsigned int address){
    return address > page_offset && address < __kernel_virtual_address_end;
}

/*
*   setup page table 
*   pgd_base:page global dirtecory 
*   pte_base:page table entry base
*   memory management has not been done, so be it.
*
*/
void init_page_table(pgd_t *pgd_base,pte_t* pte_base){
    int pgd_start_index = pgd_index(page_offset);
    pgd_t * pgd = pte_base + pgd_start_index;
    pte_t * pte = pte_base;

    int pfn = 0;
    for(;pgd_start_index < PTRS_PER_PGD;pgd++,pgd_start_index++){
        unsigned int address = pfn* PAGE_SIZE + page_offset ;

        for(int pte_ofs=0;pte_ofs<PTRS_PER_PTE;pte++, pte_ofs++){

            if(is_kernel_text(address)){
                set_pte(pte, pfn_pte(pfn, _PAGE_KERNEL_EXEC));
            }else{
                set_pte(pte, pfn_pte(pfn, _PAGE_KERNEL));
            }

        }
        set_pgd(pgd, _pgd(pte_base));// set swappage dir
        pte_base = pte;
    }
    page_desc_start = pte;
}


/*
* just in case zone size changed 
*
*/
void zone_sizes_init(void){
    unsigned long zones_size[MAX_NR_ZONES]= {0, 0, 0};
    unsigned int max_dma_pfn = virt_to_phys(MAX_DMA_ADDRESS) >> PAGE_SHIFT;
    unsigned int low,high;

    low = max_low_pfn;
    high = max_pfn;

    if( low< max_dma_pfn){
        zones_size[DMA_ZONE] = low; /* i dont know what to do if this case  */
    }else{
        zones_size[DMA_ZONE] = max_dma_pfn;
        zones_size[NORMAL_ZONE] = low - max_dma_pfn;
        zones_size[HIGH_ZONE] = high;
    }

    /*
    calulate the page status
    */
    int usedpages = 0;
    for(int i =0 ;i< MAX_NR_ZONES;i ++){
        usedpages += zones_size[i];
    }

    global_mem_node.node_spanned_pages = global_mem_node.node_present_pages = usedpages ;
    node_alloc_mem_map(&global_mem_node);
}


/*
*
*  magic function 
*/
void node_alloc_mem_map(struct pglist_data* pgt){
    mem_map= pgt->node_mem_map = page_desc_start;
    unsigned int page_desc_size = max_pfn * sizeof(struct page) ;
    unsigned int reverse_pfn = kaddr_to_pfn((void *)page_desc_start + page_desc_size);

    for(int i=0;i<max_pfn;i++){ 
        mem_map[i] = (struct page){ 0 };
        if(i < reverse_pfn){
            SetPageReserved( &mem_map[i] );
            atomic_set(&mem_map[i]._count, 1);
        }
        __free_page(&mem_map[i]);
    }
}