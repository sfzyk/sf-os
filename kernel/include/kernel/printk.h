#ifndef __PRINTK_H__
#define __PRINTK_H__
#include <i386/linkage.h>
int asmlinkage printk(const char* restrict fmt, ...);
#endif 