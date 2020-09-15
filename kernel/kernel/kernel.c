#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/segment.h>
#include <kernel/multiboot.h>
#include <kernel/printk.h>
#include <kernel/setup.h>

extern unsigned int *page_start;
void segment_init();

void kernel_main(multiboot_info_t* mbd, unsigned int magic) {
	//segment_init();
	terminal_initialize();
	setup_arch(mbd);

	printk("LINUX GREAT START SF!!!\n\n");
	printf("Hello, kernel World!\n");

}
