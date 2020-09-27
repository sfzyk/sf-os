#ifndef __SLAB_H__ 
#define __SLAB_H__
#include <kernel/list.h>
#include <i386/mm.h>
#include <cdefs.h>

/*
    actually i just want slub
*/
struct kmem_cache {
    struct kmem_cache_cpu *cpu_slab;
    unsigned long flags ;
    unsigned long min_partial;

    int size;
    int object_size;

    int offset;
    int cpu_partial;
    unsigned long  oo;
    unsigned long  max;
    unsigned long  min;

    unsigned long  allocflags;

    int refcount;
    void (*ctor)(void *); 
    int inuse ;
    int  align;
    int reserved;
    const char * name;
    struct list_head head;
    /*
    todo : sysfs and cgroup 
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
struct kmem_cache *kmem_cache_create(const char *name,
        size_t size,
        size_t align,
        unsigned long flags,
        void (*ctor)(void *));
void kmem_cache_destroy(struct kmem_cache *);
void *kmem_cache_alloc(struct kmem_cache *cachep, int flags);
void kmem_cache_free(struct kmem_cache *cachep, void *objp);

 

#endif 