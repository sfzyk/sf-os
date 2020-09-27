#include "kernel/slab.h"
#include "kernel/slub.h"
#include "kernel/log2.h"
#include "kernel/list.h"
#include "i386/page.h"


/*
*
*   init cache cache 
*
*/

struct kmem_cache kmem_cache_cpu_cache = {(0)};
struct kmem_cache kmem_cache_cache ={(0)};
struct kmem_cache kmem_cache_node_cache = {(0)};

static void * kmem_getpages(struct kmem_cache* cachep, int flags){
    /*
    * to do (now)
    */

}

static inline struct page* new_slab(struct kmem_cache *cachep, int flags){  


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
*/

struct kmem_cache *kmem_cache_create(const char *name,size_t size, size_t align,unsigned long flags, void (*ctor)(void *)){
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
    *   obj size staff 
    * 
    */ 
    size_t align_mask = (1<<align) -1 ;
    size_t tmp_size = (size + align_mask )&(~align_mask);
    kmem_cache_desc->size  = size;
    kmem_cache_desc->object_size = tmp_size;
    kmem_cache_desc->offset = 0;
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
    }else{

    }
 



}

void kmem_cache_destroy(struct kmem_cache *);
void kmem_cache_free(struct kmem_cache *cachep, void *objp);