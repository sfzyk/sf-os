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
extern struct zone* zone_table;
struct pglist_data global_mem_node;

pte_t * page_desc_start;
free_page(struct page*);

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
   

    pgd_t * pgd = pgd_base + pgd_start_index;
    pte_t * pte = pte_base;


    int pfn = 0;
    for(;pgd_start_index < PTRS_PER_PGD;pgd++,pgd_start_index++){
        unsigned int address = pfn* PAGE_SIZE + page_offset ;

        for(int pte_ofs=0;pte_ofs<PTRS_PER_PTE;pte++, pte_ofs++,pfn++ ){

            if(is_kernel_text(address)){
                set_pte(pte, pfn_pte(pfn, _PAGE_KERNEL_EXEC));
            }else{
                set_pte(pte, pfn_pte(pfn, _PAGE_KERNEL));
            }

        }
        
        pte_t * page_table_tmp  =  (((unsigned long )pte_base&(~page_mask))|0x27) - page_offset; /*  () matters!  */
        set_pgd(pgd, _pgd(page_table_tmp));// set swappage dir
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
    *
    */
    for(unsigned int i = 0;i< MAX_NR_ZONES;i++){
        zone_table[i] = global_mem_node.node_zones[i];
    }


    /*
    * calulate the page status
    */
    int usedpages = 0; 
    for(int i =0 ;i< MAX_NR_ZONES;i ++){
        usedpages += zones_size[i];
    }

    global_mem_node.node_spanned_pages = global_mem_node.node_present_pages = usedpages ;


    node_alloc_mem_map(&global_mem_node, &zones_size);
}


/*
*
*  init page descriptor 
*/
void node_alloc_mem_map(struct pglist_data* pgt, unsigned int *zone_size){
    mem_map= pgt->node_mem_map = page_desc_start;
    unsigned int page_desc_size = max_pfn * sizeof(struct page) ;
    unsigned int reverse_pfn = kaddr_to_pfn((void *)page_desc_start + page_desc_size);

    unsigned count = 0;
    unsigned zones_limit=0;

    for(int i=0; i<MAX_NR_ZONES; i++){
        /*
        * init for zone also 
        * 
        **/
        pgt->node_zones[i].zone_mem_map = mem_map + count;
        pgt->node_zones[i].pglist = pgt; // pointer to self
        pgt->node_zones[i].spanned_pages = zone_size[i];

        zones_limit += zone_size[i];
        while(count <zones_limit){
            mem_map[count] = (struct page){ 0 };
            mem_map[count].flags |= (i << NODEZONE_SHIFT);
            count +=1;
        }
    }

    
    for(int i=0;i<max_pfn;i++){ 

        if(i < reverse_pfn){
            SetPageReserved(&mem_map[i] );
            atomic_set(&mem_map[i]._count, -1);
        }    

        mem_map[i].private = -1;
        free_page(&mem_map[i]);
    }
}