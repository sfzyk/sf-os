#ifndef __SLAB_H__ 
#define __SLAB_H__
#include <kernel/list.h>
#include <i386/mm.h>
#include <stddef.h>


/*
    actually i just want slub
*/
struct kmem_cache {
    struct kmem_cache_cpu *cpu_slab;
    unsigned long flags ;
    unsigned long min_partial;
    /*
    * |--------------------++|---------------|++++++++++++++++++++++++|
    * ----------obj---------size--pointer---(ps)-------------------objsize
    * + : unused 
    * - : used
    * Note : obj size >= pointer size >= actully_size 
    */
    int pointer_size;   // pointer size
    int size;           // actully size
    int object_size;    // obj size

    int offset;
    int cpu_partial;
    unsigned long  oo;
    unsigned long  max;
    unsigned long  min;

    unsigned long  allocflags;

    int cache_order;
    int refcount;
    void (*ctor)(void *); 
    int inuse ;
    int  align;
    int reserved;
    const char * name;
    struct list_head head;
    /*
    * todo : sysfs and cgroup 
    */
   struct kmem_cache_node *node[MAX_NUMNODES];
};

struct kmem_cache_cpu{
    void **freelist;
    //unsigned long tid;  //  i just cant make thing more comlex 
    struct page *page;   //
    struct page *partial;
};


struct kmem_cache_node{
    unsigned long nr_partial;
    struct list_head partial;
};
/*
*
* all function provide for other
* 
*/
void init_kmem_cache(void);
struct kmem_cache *kmem_cache_create(const char *name,
        size_t size,
        size_t align,
        unsigned long flags,
        unsigned int cache_order,
        void (*ctor)(void *));
void kmem_cache_destroy(struct kmem_cache *);
void *kmem_cache_alloc(struct kmem_cache *cachep, int flags);
void kmem_cache_free(struct kmem_cache *cachep, void *objp);

void kmalloc_init();
void*kmalloc(int size);
void kfree(void *obj);
#endif 