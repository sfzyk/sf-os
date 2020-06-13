#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/segment.h>

void segment_init();

void kernel_main(void) {
	segment_init();
	terminal_initialize();
	printf("Hello, kernel World!\n");
}
