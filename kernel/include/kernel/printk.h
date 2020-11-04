#ifndef __PRINTK_H__
#define __PRINTK_H__
#include <kernel/linkage.h>
int asmlinkage printk(const char* restrict fmt, ...);
#endif 