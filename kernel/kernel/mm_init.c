#include <i386/page.h>
#include <i386/mm.h>
#include <i386/dma.h>
#include <i386/page_table.h>
#include <i386/page_flag.h>
#include <i386/atomic.h>


extern int start_pfn,max_pfn,max_low_pfn;
extern pte_t page_start;
extern unsigned int __kernel_virtual_address_end;
extern unsigned int __kernel_physical_address_start;
extern struct page* mem_map;
extern struct zone*  __attribute__((used)) zone_table[(MAX_NR_ZONES *MAX_NUMNODES)+1]; 
struct pglist_data  __attribute__((used)) global_mem_node;

pte_t * page_desc_start;
free_page(struct page*);

/*
* kernel text include kernel data for now 
*
*
*/
int is_kernel_text(unsigned int address){
    return address > PAGE_OFFSET + __kernel_physical_address_start && address < __kernel_virtual_address_end;
}

/*
*   setup page table 
*   pgd_base:page global dirtecory 
*   pte_base:page table entry base
*   memory management has not been done, so be it.
*
*/
void init_page_table(pgd_t *pgd_base,pte_t* pte_base){
   
    int pgd_start_index = pgd_index(PAGE_OFFSET);
   

    pgd_t * pgd = pgd_base + pgd_start_index;
    pte_t * pte = pte_base;


    int pfn = 0;
    for(;pgd_start_index < PTRS_PER_PGD;pgd++,pgd_start_index++){
        unsigned int address = pfn* PAGE_SIZE + PAGE_OFFSET ;

        for(int pte_ofs = 0;pte_ofs < PTRS_PER_PTE;pte++, pte_ofs++,pfn++ ){

            if(is_kernel_text(address)){
                set_pte(pte, pfn_pte(pfn, _PAGE_KERNEL_EXEC));
            }else{
                set_pte(pte, pfn_pte(pfn, _PAGE_KERNEL));
            }

        }
        
        pte_t * page_table_tmp  =  (((unsigned long )pte_base&(PAGE_MASK))|_PAGE_RW|_PAGE_PRESENT) - PAGE_OFFSET; /*  () matters!  */
        set_pgd(pgd, _pgd(page_table_tmp));// set swappage dir
        pte_base = pte;
    }
    
    page_desc_start = pte;
}


/*
* just in case zone size changed 
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
        zones_size[HIGH_ZONE] = high - low;
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
    /*
    *
    */
}

/*
*  init page descriptor 
*/
void node_alloc_mem_map(struct pglist_data* pgt, unsigned int *zone_size){
    mem_map= pgt->node_mem_map = page_desc_start;
    unsigned int page_desc_size = max_pfn * sizeof(struct page) ;

    unsigned int kernel_reverse_pfn_end = (__pa((unsigned int)(&__kernel_virtual_address_end)))>> PAGE_SHIFT;
    unsigned int kernel_page_reverse_pfn = kaddr_to_pfn((void *)page_desc_start + page_desc_size);

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

    for(unsigned int i = 0;i< MAX_NR_ZONES;i++){
        zone_table[i] = &global_mem_node.node_zones[i];
    }
    
    for(int i=0;i<max_pfn;i++){ 

        mem_map[i].private = 0;
        if((i<= kernel_page_reverse_pfn && i > (0x01000000 >> PAGE_SHIFT))|| (i<kernel_reverse_pfn_end)){ /*reversed  page: kernel data &kernel code & < 1M */
            SetPageReserved(&mem_map[i]);
            atomic_set(&mem_map[i]._count, 0); /* cant free a reversed page */
        }else{
            atomic_set(&mem_map[i]._count, -1);
            free_page(&mem_map[i]);
        }
    }
}

/*init for zonelist */
void init_build_zonelist(pg_data_t* mem_node){
    
    for(int i =0 ;i<GFP_ZONETYPES;i++){
        int DMA = 0,HIGH = 0;
        DMA = __GFP_DMA & i;
        HIGH = __GFP_HIGH &i;
        int p = 0;
        while(DMA|HIGH){
            if(DMA){
                mem_node->node_zonelists[i].zones[p] = &mem_node->node_zones[DMA_ZONE];
                DMA =0 ;
            }
            else if(HIGH){
               mem_node->node_zonelists[i].zones[p] = &mem_node->node_zones[HIGH_ZONE];
               HIGH =0;
            }
            p++;
        }
        mem_node->node_zonelists[i].zones[p] = &mem_node->node_zones[NORMAL_ZONE];
    }
}