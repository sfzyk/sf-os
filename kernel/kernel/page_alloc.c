#include <i386/page_flag.h>
#include <i386/mm.h> 
#include <i386/percpu.h>
#include <i386/smp.h>
#include <i386/config.h>
static DEFINE_PER_CPU(struct page_state, page_state ) = { 0}; 


static inline unsigned long page_order(struct page* pg){
    return pg->private;
}

static inline void set_page_order(struct page* pg, int order){
    pg->private = order;
    __SetPagePrivate(pg);
}

/*
*   z : zone page in 
*   pg: page 
    low : required order 
    high: final order 
*
*/
static inline struct page * expand(struct zone * z, struct page * pg, int low, int high, struct free_area *fa){
    unsigned long size = 1<< high ;
    while (high >= low ){
        fa -- ;
        size >>= 1 ;
        list_add_tail(&fa->head, &pg[size].lru);     /****** tips  point  *******/
        fa->nr_free += 1;
        z->free_pages += size;
        set_page_order(&pg[size],size);
    }
    return pg;
}

static struct page * __rmqueue(struct zone * z ,int order ){

    int o= 0;
    for(o=order;o<MAX_ORDER;o++){
        if(list_empty(&(z->free_area[o].head)))
            continue ;
        struct page * pg = list_entry( z->free_area[o].head.next, struct page, lru);
        list_del(&pg->lru);

        z->free_area[o].nr_free --;
        z->free_pages -= 1UL << order ;
        return expand(z ,pg, order, o , &z->free_area[o]);
    }
    return NULL;
}

/*
return allocated number 
and add entry to list tail 
*/
static int rmqueue_bulk(struct zone* z, int order , int count ,struct list_head *list ){
    struct page * pg ;
    int allocated = 0 ;
    for(int i=0; i<count; i++){
        pg  = __rmqueue(z ,order);
        if(pg == NULL){
            break;
        }

        list_add_tail(list, & pg->lru);
        allocated ++ ;
    }
    return allocated;
}

/*
eg cold or hot buffer 
todo : 
    mod pagestate 

zone : page reside in 
*/
static struct page*  buffered_rmqueue(struct zone *zone, int order,int gfp_flags){
    unsigned long flags;
    struct page * pg;
    int cold = !!(gfp_flags && __GFP_COLD) ;
    if(order == 0){// time to use buffered page 
        struct per_cpu_pages *pcp ;
        pcp = &zone->pageset[get_cpu()].pcp[cold];

        if(pcp->count <= pcp->low ) { //buffer not enough 
            pcp->count += rmqueue_bulk(zone, 0, pcp->batch, &pcp->list);
        }

        if(pcp->count){
            pg = list_entry(&pcp->list->next,struct page , lru );
            pcp->count--; 
        }
        put_cpu();
    }

    if(pg==NULL){
        pg = __rmqueue(zone, order );
    }

    if(pg != NULL){
        /* todo :
        prepare new page 
        */
    }
    return pg;
}


/********************************************************************************************************************************
 *              
 * 
 * 
/* heart of buddy alloctor */ 
/* todo: rebalance and more scan */
struct page * __alloc_pages(unsigned int gfp_mask, unsigned int order, struct zonelist *zonelist){
    struct page * pg;
    struct zone** zones;
    struct zone * z;
    int class_zoneidx; 
    zones = zonelist->zones; 
    class_zoneidx = zones[0] - zones[0]->pglist->node_zones; 


    for(int i =0;(z=zones[i]!=0);i++ ){

        pg = buffered_rmqueue(z,order,gfp_mask);

        if(pg){
            goto got_pg;
        }

    }
    if(!pg){
        goto nopg;
    }

    got_pg:
        return pg;
    nopg:
        

}
