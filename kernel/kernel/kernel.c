#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/segment.h>

void segment_init();

void kernel_main(void) {
	terminal_initialize();
	segment_init();
	printf("Hello, kernel World!\n");
}
