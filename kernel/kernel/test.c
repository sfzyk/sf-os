#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/segment.h>
#include <kernel/multiboot.h>
#include <kernel/printk.h>
#include <kernel/setup.h>
#include <kernel/slab.h>
#include <kernel/test.h>
 
void test_slub(){
    struct kmem_cache *test1;
    test1 = kmem_cache_create("test1 cache",54,16,0,1,NULL);
    printf("localtion test1 cache : %x \n",test1);
}