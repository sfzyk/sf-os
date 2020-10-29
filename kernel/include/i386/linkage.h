#ifndef __ASMLINKAGE_H__
#define __ASMLINKAGE_H__
// for asm use 

#define asmlinkage __attribute__((regparm(0)))
//  #define __init__ __atrribute__((_sections("in")))
#define fastcall	__attribute__((regparm(3)))

#endif 