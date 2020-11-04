#ifndef __ASMLINKAGE_H__
#define __ASMLINKAGE_H__
// for asm use 

#define asmlinkage __attribute__((regparm(0)))
//  #define __init__ __atrribute__((_sections("in")))
#define fastcall	__attribute__((regparm(3)))

#ifndef __ALIGN
#define __ALIGN		.align 4,0x90
#define __ALIGN_STR	".align 4,0x90"
#endif

#define ALIGN __ALIGN
#define ALIGN_STR __ALIGN_STR

#define ENTRY(name) \
  .globl name; \
  ALIGN; \
  name:

#endif 