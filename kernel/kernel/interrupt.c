#include <i386/process.h>


struct desc_struct idt_table[256] __attribute__((__section__(".data"))) =  {{0,0}, };
