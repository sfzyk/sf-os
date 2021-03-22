#include "kernel/slab.h"
#include "kernel/slub.h"
#include "kernel/log2.h"
#include "kernel/list.h"
#include "kernel/printk.h"
#include "i386/mm.h"
#include "i386/page.h"
#include "i386/page_alloc.h"
#include "i386/page_flag.h"

/*
*
*   init cache cache 
*
*/
struct list_head cache_list_head;

extern struct pglist_data global_mem_node;

struct kmem_cache kmem_cache_cpu_cache = {(0)};
struct kmem_cache_cpu t1 ={(0)};
struct kmem_cache_node n1 = {(0)};

struct kmem_cache kmem_cache_cache ={(0)};
struct kmem_cache_cpu t2 ={(0)};
struct kmem_cache_node n2={(0)};

struct kmem_cache kmem_cache_node_cache = {(0)};
struct kmem_cache_cpu t3={(0)};
struct kmem_cache_node n3={(0)};


/*
* init for these kernel static data 
* stupid code style , fix me. 
*/
void init_kmem_cache(void){
    init_list(&cache_list_head);
    list_add(&cache_list_head, &kmem_cache_cache.head);
    list_add(&cache_list_head, &kmem_cache_node_cache.head);
    list_add(&cache_list_head, &kmem_cache_cpu_cache.head);
    {
        kmem_cache_cache.cache_order = 1;
        kmem_cache_cache.inuse = 0;
        sprintf(kmem_cache_cache.name,sizeof(kmem_cache_cache.name), "kernel used kmem cache(original cache)");
        size_t size = sizeof(struct kmem_cache);
        size_t align = 128; /* nonsense */
        size_t pointer_size = sizeof(void *); /* assert pointer_size must be 1<<n */
        size_t pointer_align_mask = pointer_size - 1;
        size_t tmp_size = (size + pointer_align_mask )&(~pointer_align_mask);
        size_t align_mask = align - 1;
        size_t obj_size = (tmp_size + pointer_size + align_mask) & (~align_mask);
        kmem_cache_cache.size  = size;
        kmem_cache_cache.object_size = obj_size;
        kmem_cache_cache.pointer_size = tmp_size;
        kmem_cache_cache.offset = tmp_size;
        set_cpu_partial(&kmem_cache_cache);
        set_min_partial(&kmem_cache_cache, ilog2(size)/2 );
        kmem_cache_cache.cpu_slab = &t1;
        kmem_cache_cache.node[0]=&n1;
        init_list(&n1.partial);
    }

    {
        kmem_cache_cpu_cache.cache_order = 1;
        kmem_cache_cpu_cache.inuse = 0;
        sprintf(kmem_cache_cpu_cache.name,sizeof(kmem_cache_cpu_cache.name), "kernel used kmem cpu cache(original cache)");
        //kmem_cache_cpu_cache.name = "kernel used kmem cpu cache(original cache)";
        size_t size = sizeof(struct kmem_cache_cpu);
        size_t align = 128; /* nonsense */
        size_t pointer_size = sizeof(void *); /* assert pointer_size must be 1<<n */
        size_t pointer_align_mask = pointer_size - 1;
        size_t tmp_size = (size + pointer_align_mask )&(~pointer_align_mask);
        size_t align_mask = align - 1;
        size_t obj_size = (tmp_size + pointer_size + align_mask) & (~align_mask);
        kmem_cache_cpu_cache.size  = size;
        kmem_cache_cpu_cache.object_size = obj_size;
        kmem_cache_cpu_cache.pointer_size = tmp_size;
        kmem_cache_cpu_cache.offset = tmp_size;
        set_cpu_partial(&kmem_cache_cpu_cache);
        set_min_partial(&kmem_cache_cpu_cache, ilog2(size)/2 );
        kmem_cache_cpu_cache.cpu_slab = &t2;
        kmem_cache_cpu_cache.node[0]=&n2;
        init_list(&n2.partial);
    }

    {
        kmem_cache_node_cache.cache_order = 1;
        kmem_cache_node_cache.inuse = 0;
        sprintf(kmem_cache_node_cache.name,sizeof(kmem_cache_node_cache.name), "kernel used kmem node cache(original cache)");
        // kmem_cache_node_cache.name = "kernel used kmem node cache(original cache)";
        size_t size = sizeof(struct kmem_cache_node);
        size_t align = 128; /* nonsense */
        size_t pointer_size = sizeof(void *); /* assert pointer_size must be 1<<n */
        size_t pointer_align_mask = pointer_size - 1;
        size_t tmp_size = (size + pointer_align_mask )&(~pointer_align_mask);
        size_t align_mask = align - 1;
        size_t obj_size = (tmp_size + pointer_size + align_mask) & (~align_mask);
        kmem_cache_node_cache.size  = size;
        kmem_cache_node_cache.object_size = obj_size;
        kmem_cache_node_cache.pointer_size = tmp_size;
        kmem_cache_node_cache.offset = tmp_size;
        set_cpu_partial(&kmem_cache_node_cache);
        set_min_partial(&kmem_cache_node_cache, ilog2(size)/2 );
        kmem_cache_node_cache.cpu_slab = &t3;
        kmem_cache_node_cache.node[0]=&n3;
        init_list(&n3.partial);
    }
}


static void * kmem_getpages(struct kmem_cache* cachep, int flags){
    /*
    *
    * to do with staff flags
    *
    */
    int cache_o = cachep->cache_order;
    int slab_size = 1 << cache_o ;
    struct page * pg = __alloc_pages(0, cache_o, &global_mem_node.node_zonelists[0]);

    if(pg)
        for(int i =0 ;i<slab_size;i++){
            SetPageSlab((pg+i));
            if(i!=0){
                (pg+i)->next = pg; /* for convience */
                (pg+i)->buddy = 1;
            }
            pg->cache_p = cachep;
        } 

    pg->buddy = 0;
    return (void *) pg;
}

static void * kmem_freepages(struct kmem_cache* cachep, struct page * pg ,int flags){
    /*
    *
    * to do with staff flags
    *
    */
    int cache_o = cachep->cache_order;
    int slab_size = 1 << cache_o ;

    for(int i =0 ;i<slab_size;i++){
        ClearPageSlab((pg+i));
    } 
    __free_pages(pg, cachep->cache_order); 
    return ;
}

/*
* get a new slab for cahep :
* int: cache_order ,int flags ? 
*/
static inline struct page* new_slab(struct kmem_cache *cachep, int flags){  
    struct page  * page =  (struct page *) kmem_getpages(cachep,cachep->cache_order);

    unsigned int *pg = page_address(page);

    unsigned int page_nums = 1 << cachep->cache_order;
    unsigned int pointer_size = sizeof(void *);
    unsigned int slab_byte_size = page_nums << PAGE_SHIFT;
    unsigned int slab_limit = (unsigned char*)pg + slab_byte_size;

    unsigned char *p = (unsigned char *)pg + cachep->offset;
    /*
    *
    *  init free list
    * 
    */

    unsigned int obj_num = 0 ;
    /* could be calculated , but i just dont do that */

    while(p + cachep->object_size < slab_limit){
        *(unsigned int *)p = p + cachep->object_size;
        p = *(unsigned int *)p;
        obj_num ++ ;
    }

    *(unsigned int *)p = 0; /* the end of list */
    obj_num ++ ;
    page->inuse = page->objects = obj_num;
    page->frozen = 1; /* when i get a slab it must be currently used */
    cachep->cpu_slab->freelist = (unsigned char*)pg + cachep->offset; /* understand void ** */

    return page;
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
    sprintf(kmem_cache_desc->name,sizeof(kmem_cache_desc->name),name);
    /*
    *
    *   obj size staff: see slab.h for more information 
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
 
    init_cpu_slub(kmem_cache_desc, flags);
    init_pernode_slab(kmem_cache_desc, flags);

    list_add(&cache_list_head, &kmem_cache_desc->head);
    return kmem_cache_desc;
}
/*
using lru  double link in  node slub 
using next single link in  cpu  slub 
*/
void *kmem_cache_alloc(struct kmem_cache *cachep, int flags){
    /* todo : nume */
    void * return_obj_p = NULL;

    struct kmem_cache_node *n = cachep->node[0]; 
    struct kmem_cache_cpu *c = cachep->cpu_slab;

restart : 
    if(c->page){ /* now we get it from the very first level page pool for slab */
        void * address  = page_address(c->page);
        void ** free_list_head = cachep->cpu_slab->freelist;
        return_obj_p = (unsigned char *)free_list_head - cachep->offset;
        cachep->cpu_slab->freelist = *free_list_head;
        if(*cachep->cpu_slab->freelist == NULL){ /*a slab ends its life, then become ghost :) */
            cachep->cpu_slab->page = NULL;
            cachep->cpu_slab->freelist = NULL;
            c->page->pobjects = 0; /* all objects is in use */
            c->page->frozen = 0; /* died */
        }
        /* c->page->inuse wont update */

    }else{ 
        if(c->partial){
        /* now we get to cpu slab move a slab to c->page */
            c->page = c->partial;
            c->freelist = c->page->free_list; /* do not update page freelist since it's c->page */
            c->partial = c->partial->next; 
            goto restart;

        }else{
            goto seeknode;
        }
    }

    /* every thing would be ok */
    return return_obj_p;
seeknode: 
    if(list_empty(&n->partial)){ 
        /* if no any slab in n->partial */
        struct page* pg = new_slab(cachep, flags);
        cachep->cpu_slab->page = pg;
        // cachep->cpu_slab->freelist is already been set;
        goto restart;

    }else{ /* get slab from node */
        struct list_head *p = n->partial.next;
        list_del(p);
        struct page *pg = (struct page*)list_entry(p,struct page,free_list);
        cachep->cpu_slab->page = pg;
        cachep->cpu_slab->freelist = pg->free_list;
        cachep->cpu_slab->page->inuse = pg->objects;
        goto restart;
    }
}

void kmem_cache_free(struct kmem_cache *cachep, void *objp){
    struct page* pg = kaddr_to_page(objp);
    if(pg->buddy==1){
        pg = pg->next; /* we go to the head slab page */
    }

    if(cachep->cpu_slab->page == pg){
        /* just free it ,i hope it work, pointer sucks*/
        void * head = *cachep->cpu_slab->freelist;
        if(head == NULL){ 
            /* already full */
            printk("full page still in current front???\n");
            return;
        }
    
        while(*(unsigned int *)head){

            if(*(unsigned int *)head > objp) /*find it*/
                break;

            head = (void *)*(unsigned int *)head; 
            /* a little complex 
            * free pointer is *****void so be *void 
            */
        }

        void * next = (void *)*(unsigned int *)head;
        *(unsigned int *)head = objp + cachep->offset;
        head = (void *)*(unsigned int *)head;
        if(head) 
            *(unsigned int *)head = next;
        return;
    }

    /* free it, if it alive again ,a lots to do 
    *
    *  free_list in pg struct 
    * */
    void * head = pg->free_list;

    if(head == NULL){ 
        /* already full , it alive again, we put it in cpu slab */
        pg->free_list = objp + cachep->offset;
        *(unsigned int *)(objp+ cachep->offset) = NULL; /* init for this slab free_list */
        pg->inuse -= 1; /* inuse == objects before */

        unsigned total_obj_nums  = 0;
        void * tmp_p;
        if(cachep->cpu_slab->partial){  /* already a pobjects avaliable */
            tmp_p  = cachep->cpu_slab->partial;
            total_obj_nums = cachep->cpu_slab->partial->pobjects;
            total_obj_nums = pg->pobjects = pg->pobjects + total_obj_nums + 1;  

            if(total_obj_nums < cachep->cpu_partial){ /* still cpu slab have enough space for that */
                cachep->cpu_slab->partial = pg;
                pg->next = tmp_p;
            }else{ /* must move some slab to node slab */
                list_add(&cachep->node[0]->partial, &pg->lru);
                cachep->node[0]->nr_partial ++ ;
                /* other staff done at free empty time  */
            }

        }else{
            cachep->cpu_slab->partial = pg;
            pg->pobjects += 1;
            pg->next = NULL;
        }
        return;

    }else{
        /* slab already in cpu slab or node slab  */
        while(*(unsigned int *)head){
            if(*(unsigned int *)head > objp) /*find it*/
                break;
            head = (void *)*(unsigned int *)head; 
            /* a little complex 
            * free pointer is *****void so be *void 
            */
        }
        void * next = (void *)*(unsigned int *)head;
        *(unsigned int *)head = objp + cachep->offset;
        head = (void *)*(unsigned int *)head;
        if(head) 
            *(unsigned int *)head = next;

        pg->inuse -=1 ;

        if(pg->inuse == 0){ /* already empty slab , if there are too many slab we free it */
            if(cachep->min_partial < cachep->node[0]->nr_partial){

                /* free whole slab , when there are many in node slab */
                if(pg->frozen == 1){ /* remove cpu slab */
                    unsigned int all_nums_slab = cachep->cpu_slab->page->pobjects + 1 - cachep->cpu_slab->page->objects;
                    struct page** tmp_page = &cachep->cpu_slab->partial;

                    while( (*tmp_page)!= pg){
                        tmp_page = &(*tmp_page)->next;
                    }
                    *tmp_page = pg->next ;
                    cachep->cpu_slab->page->pobjects = all_nums_slab;

                }else{
                    list_del(&pg->lru);
                    cachep->node[0]->nr_partial--;
                }
                kmem_freepages(cachep,pg,cachep->flags); 
                return;
            }
        }
        /* not full or not empty , dont do anything */
        if(pg->frozen == 1){/* in cpu slab */
            cachep->cpu_slab->partial->pobjects ++ ;
        }else{/*in node slab */
            /* wont do anything ?*/
        }
        return;
    }
}

void kmem_cache_destroy(struct kmem_cache *cachep){
    /*to do */
}

/*
*   这里认为如果分配的内存小于一页，那就用高速缓存，大于一页就应该给一页，但是不应该有一次要太多页的情况
*/
struct kmem_cache* kmalloc_small[PAGE_SHIFT];
void kmalloc_init(){
    char name[300];
    for(int i=0;i<PAGE_SHIFT;i++){
        sprintf(name,sizeof(name),"kmalloc %dth",i);
        kmalloc_small[i] = kmem_cache_create(name,1<<i,1<<i,0,0,NULL); 
    }
}

int get_order(int size){
    if(size==1)return 0;

    int res = 0;
    while(size){
        res++;
        size/=2;
    }
    return res;
}

void* kmalloc(int size){
    if(size < PAGE_SIZE){
        int order = get_order(size);
        return kmem_cache_alloc(kmalloc_small[order],NULL);
    }
    else{
        if(size > 4*PAGE_SIZE){
            return NULL; 
        }
        int pages_nums = (size+PAGE_SIZE - 1) / PAGE_SIZE ;
        int cache_o = get_order(pages_nums);
        struct page* pg = __alloc_pages(0, cache_o, &global_mem_node.node_zonelists[0]);
        for(int i=0;i<pages_nums;i++){
            SetPageKmem((pg+i));
            if(i==0)
                (pg+i)->kmem_size = pages_nums;
            else
                (pg+i)->kmem_size = -1;
        }
        /*
        set page descriptor proper;
        */
        return page_address(pg);
    }
}

void kfree(void *obj){
    
    struct page* pg =  kaddr_to_page(obj);
    if(PageKmem(pg)){
        int kmem_size = pg->kmem_size;
        if(kmem_size==-1)return;
        for(int j=0;j<kmem_size;j++){
            free_page(pg+j);
        }
    }else if PageSlab(pg){
        struct kmem_cache* kc = pg->cache_p;
        kmem_cache_free(kc,obj);
        return ;
    }
}
