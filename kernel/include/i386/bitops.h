#ifndef __BITOPS_H__
#define __BITOPS_H__


static inline void  set_bit(int nr,volatile unsigned long * addr){
    __asm__ volatile (
        "lock;btsl %1,%0"
        :"=m"(addr)
        :"Ir"(nr)
    );
}


static inline void  non_atomic_set_bit(int nr,volatile unsigned long * addr){
    __asm__ volatile (
        "btsl %1,%0"
        :"=m"(addr)
        :"Ir"(nr)
    );
}


static inline void clear_bit(int nr , volatile unsigned long *addr){
    __asm__ volatile(
        "lock;btrl %1,%0"
        :"=m"(addr)
        :"Ir"(nr)
    );
}


static inline void non_atomic_clear_bit(int nr , volatile unsigned long *addr){
    __asm__ volatile(
        "btrl %1,%0"
        :"=m"(addr)
        :"Ir"(nr)
    );
}

#ifdef __compiler_offsetof
#define offsetof(TYPE, member) __compiler_offsetof(TYPE,member) 
#else 
#define offsetof(TYPE, member) &((TYPE*)0)->member 
#endif 

#endif 