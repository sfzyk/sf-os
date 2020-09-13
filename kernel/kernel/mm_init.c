#include <i386/page.h>
#include <i386/mm.h>
#include <i386/dma.h>

extern int start_pfn,max_pfn,max_low_pfn;
struct pglist_data global_mem_node;

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

extern unsigned int page_start;
void node_alloc_mem_map(struct pglist_data* pgt){
    pgt->node_mem_map = page_start;



}