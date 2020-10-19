#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/segment.h>
#include <kernel/multiboot.h>
#include <kernel/printk.h>
#include <kernel/setup.h>
#include <kernel/slab.h>
#include <kernel/test.h>
#include <kernel/list.h>
#include <i386/page.h>

extern struct list_head cache_list_head;

unsigned int jump_num(void **p){
    unsigned int ret = 0;
    while(*p != 0){
        p = *p;
        ret ++;
    }
    ret ++;
    return ret;
}


void dump_cachep_info(struct kmem_cache* kmp){
    printf("cache name is  %s \n",kmp->name);
    printf("        size pointer: %d %d %d. slab-page-num : %d ", kmp->size, kmp->pointer_size, kmp->object_size,1 << kmp->cache_order);
    printf(" inuse:%d \n",kmp->inuse);
    if(kmp->cpu_slab->page){
        printf("current slab : %x %d/ %d \n", page_address(kmp->cpu_slab->page),jump_num(kmp->cpu_slab->freelist),kmp->cpu_slab->page->objects);
    }else{
        printf("no current slab\n");
    }
    struct page * part = kmp->cpu_slab->partial;
    while(part){
        printf("partial; slab : %x %d/ %d \n", page_address(part),jump_num(part->free_list),part->objects);
    }
}

void test_dump_all_cahep(){
    struct list_head *pos;
    list_for_each(pos,&cache_list_head){
        printf("---------\n");
        dump_cachep_info(list_entry(pos,struct kmem_cache,head));
    }
}


/*
*
* test case for slub system (to do more)
*
*/

void test_slub(){
    struct kmem_cache *test1,*test2;
    char * name = "for purpose";
    test1 = kmem_cache_create(name ,54,16,0,2,NULL);
    // test2 = kmem_cache_create("tes2 cache",58,128,0,0,NULL);

    printf("localtion test1 cache : %x , test2 cache: %x\n",test1, test2);

    // test_dump_all_cahep();
    void *t1_o1,*t1_o2,*t1_o3;
    t1_o1 = kmem_cache_alloc(test1, 0);
    t1_o2 = kmem_cache_alloc(test1, 0);
    t1_o3 = kmem_cache_alloc(test1, 0);
    printf("test1 cache alloc result : %x %x %x\n",t1_o1,t1_o2,t1_o3);
    test_dump_all_cahep();

    // terminal_initialize();
    kmem_cache_free(test1, t1_o1);
    kmem_cache_free(test1, t1_o2);
    kmem_cache_free(test1, t1_o3);
    // test_dump_all_cahep();
    /*
    to fix tty code 
    */
}
