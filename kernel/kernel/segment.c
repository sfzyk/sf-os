#include <kernel/segment.h>

void segment_init(){
    asm("lgdt cpu_gdt_descr");
}
