#ifndef __SLAB_H__ 
#define __SLAB_H__
/*
    actually i just want slub
*/
struct kmem_cache_cpu{
    void **freelist;
    unsigned long tid; //unused for now
    struct page *page;//
    struct page *page;

};
 

#endif 