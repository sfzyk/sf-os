#include <kernel/i386/page.h>
#include <kernel/i386/page_table.h>

pgd_t global_page_dir[1024] __attribute__((__section__(".bss.pagedir")))= { {0}, }; 

