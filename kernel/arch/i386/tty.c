#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"
volatile int terminal_offset; // always >= 0 


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000; 
//80*25*2（一个显示需要两个byte ） == 4000 1M之下的映射地址 B8000 -- BC000的地址为VGA内存映射区

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

#define RING_SIZE (32000)
static uint16_t terminal_ringbuffer[RING_SIZE];
static int terminal_pos_now;
static int terminal_pos_base;

void flush_ringbuffer_to_buffer(){
	int i=terminal_pos_base,j=terminal_pos_now;
	
	terminal_buffer = VGA_MEMORY;
	while(i != j){
		*terminal_buffer = terminal_buffer[i];
		i = (i+1) % RING_SIZE;
	}
}


void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_offset = 0;
	terminal_pos_now = 0;
	terminal_pos_base = 0;

	terminal_color =  vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	memset(terminal_ringbuffer,0,sizeof(terminal_ringbuffer));

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c){
	unsigned char uc = c;
	/*
	if(uc == '\n'){
		terminal_row ++ ;
		terminal_column = 0;
		return ;
	}

	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (++terminal_column >= VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row >= VGA_HEIGHT)
			terminal_row = 0;
	}
	*/
	if(uc == '\n'){
		int t;
		t = ((terminal_pos_now + VGA_WIDTH - 1)/VGA_WIDTH) * VGA_WIDTH;
		for(int i=terminal_pos_now;i<t;i++){
			terminal_ringbuffer[i] = vga_entry(' ',terminal_color);
		}
		terminal_pos_now = t;
	}
	else{
		terminal_ringbuffer[terminal_pos_now] = vga_entry(c,terminal_color);
		terminal_pos_now = (terminal_pos_now + 1) % RING_SIZE;
	}

	if(terminal_pos_base < terminal_pos_now - VGA_WIDTH*VGA_HEIGHT){
		terminal_pos_base = terminal_pos_now - VGA_WIDTH*VGA_HEIGHT;
	}
	flush_ringbuffer_to_buffer();
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
