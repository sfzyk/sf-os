#include "kernel/slab.h"
#include "kernel/slub.h"
#include "kernel/log2.h"
#include "kernel/list.h"
#include "i386/mm.h"
#include "i386/page.h"
#include "i386/page_alloc.h"
#include "i386/page_flag.h"

/*
*
*   init cache cache 
*
*/
extern struct pglist_data global_mem_node;

struct kmem_cache kmem_cache_cpu_cache = {(0)};
struct kmem_cache kmem_cache_cache ={(0)};
struct kmem_cache kmem_cache_node_cache = {(0)};


static void * kmem_getpages(struct kmem_cache* cachep, int flags){
    /*
    *
    * to do with staff flags
    *
    */
    int cache_o = cachep->cache_order;
    int slab_size = 1 << cache_o ;
    struct page * pg = __alloc_pages(0, cache_o, global_mem_node.node_zonelists);

    if(pg)
        for(int i =0 ;i<slab_size;i++){
            SetPageSlab(*(pg+i));
        }
    
    return (void *) pg;
}

/*
* get a new slab for cahep :
* int: cache_order ,int flags ? 
*/
static inline struct page* new_slab(struct kmem_cache *cachep, int flags){  
    struct page  * page = kmem_cache_alloc(cachep,flags);
    unsigned int *pg = page_address(page);

    unsigned int page_nums = 1 << cachep->cache_order;
    unsigned int pointer_size = sizeof(void *);
    unsigned int slab_byte_size = page_nums << PAGE_SHIFT;
    unsigned int slab_limit = pg + slab_byte_size;
    unsigned int  * p = pg + cachep->offset;
    /*
    *
    * 
    *  init free list
    * 
    */
    while(p + (pointer_size + cachep->size)*2 - cachep->offset< slab_limit){
        *p = p + cachep->object_size;
        p = *p;
    }

    *p = 0; /* the end of list */

    cachep->cpu_slab->freelist = pg + cachep->offset; /*  understand void ** */

    return (struct page*)pg;
}


static inline void init_cpu_slub(struct kmem_cache * kmem,int flags){
    /*
    *
    * todo : percpu memory allocation 
    * 
    * */
    kmem->cpu_slab = (struct kmem_cache_cpu *)kmem_cache_alloc(&kmem_cache_cpu_cache, flags); 
    kmem->cpu_slab->freelist = NULL; 

    kmem->cpu_slab->page = NULL;
    kmem->cpu_slab->partial = NULL;
    /*
    * freelist is type::void**
    */
}


static inline void init_pernode_slab(struct kmem_cache *kmem, int flags){
    for(int i=0;i<MAX_NUMNODES;i++){ 
        kmem->node[i] = (struct kmem_cache_node*)kmem_cache_alloc(&kmem_cache_node_cache,flags);
        /*
        * actually init for node slab buffer
        * */
        init_list(&kmem->node[i]->partial);
        kmem->node[i]->nr_partial = 0;
    }
}

/*
* init for these kernel static data 
* todo : move data to a proper place 
*/
static inline void init_kmem_cache(void){


}
/*
* must be called after init_kmem_cache was called 
* i just want slub alloctor 
*
* for example , page align  would be 4096 (not 12)!
*/
struct kmem_cache *kmem_cache_create(const char *name,size_t size, size_t align,unsigned long flags, unsigned int cache_order ,void (*ctor)(void *)){
    if(!name){
        return NULL;
    }

    /*
    * todo :
    * do something with flags 
    * do something with safe check 
    * 
    */
    struct kmem_cache* kmem_cache_desc = (struct kmem_cache*)kmem_cache_alloc(&kmem_cache_cache, flags); 
    kmem_cache_desc->allocflags = flags;
    kmem_cache_desc->ctor = ctor;
    /*
    *
    *   obj size staff: see slab.h please 
    * 
    */ 
    size_t pointer_size = sizeof(void *); /* assert pointer_size must be 1<<n */
    size_t pointer_align_mask = pointer_size - 1;
    size_t tmp_size = (size + pointer_align_mask )&(~pointer_align_mask);

    size_t align_mask = align - 1;
    size_t obj_size = (tmp_size + pointer_size + align_mask) & (~align_mask);

    kmem_cache_desc->size  = size;
    kmem_cache_desc->object_size = obj_size;
    kmem_cache_desc->pointer_size = tmp_size;

    kmem_cache_desc->offset = tmp_size;

    kmem_cache_desc->cache_order = cache_order;

    /*
    *
    *  limit size init 
    * 
     */
    set_cpu_partial(kmem_cache_desc);
    set_min_partial(kmem_cache_desc, ilog2(size)/2 );

    init_cpu_slab(kmem_cache_desc, flags);
    init_pernode_slab(kmem_cache_desc, flags);
    return kmem_cache_desc;
}

void *kmem_cache_alloc(struct kmem_cache *cachep, int flags){
    struct kmem_cache_cpu *c = cachep->cpu_slab;
    /* todo : nuame */
    struct kmem_cache_node *n = &cachep->node[0]; 
    if(c->page){ //
        void * address  = page_address(c->page);
        

    }else{ 
        if(c->partial){

        }else{
            goto seeknode;
        }
    }

seeknode: 
    if(list_emtry(n->partial)){ 
        /* if no any slab in n->partial */
        struct page* pg = new_slab(cachep, flags);
        cachep->cpu_slab->page = pg;
        // cachep->cpu_slab->freelist is already been set;

    }else{
        

    }
 



}

void kmem_cache_destroy(struct kmem_cache *);
void kmem_cache_free(struct kmem_cache *cachep, void *objp);