#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/segment.h>
#include <kernel/multiboot.h>
#include <kernel/printk.h>
#include <kernel/setup.h>
#include <kernel/slab.h>

#include <kernel/test.h>

extern unsigned int *page_start;
void segment_init();



void kernel_main(multiboot_info_t* mbd, unsigned int magic) {
	//segment_init();
	terminal_initialize();
	
	setup_arch(mbd);

	init_kmem_cache();

	terminal_initialize();
	printf("test-begin-all-parts\n\n");
	test_slub();
	printf("slub has been test well\n\n");

}
