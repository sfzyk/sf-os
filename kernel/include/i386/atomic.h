#ifndef __ATOMIC_H__
#define __ATOMIC_H__
typedef struct { 
    volatile int counter; 
}atomic_t;

#define ATOMIC_INIT(i) {(i)}

#define atomic_read(v) ((v)->counter)
#define atomic_set(v,i) ((v)->counter = (i))

static inline atomic_add(atomic_t * v,int i){  
    __asm__ __volatile__ (
        "addl %1, %0"
        :"=m"((v)->counter)
        :"ir"(i),"m"((v)->counter)
    );
}

static inline atomic_sub(atomic_t* v,int i){
    __asm__ __volatile__ (
        "subl %1, %0"
        :"=m"((v)->counter)
        :"ir"(i),"m"((v)->counter)
    )
}

/*
true if v->counter not zero 
false if v->counter zero 
*/
static inline atomic_sub_and_test(atomic_t *v,int i){
    unsigned char c;
    __asm__ __volatile__ (
        "subl %2, %0; sete %1"
        :"=m"(v->counter), "=qm" c
        :"qm"(i), "m"(v->counter): "memory"
    )
    return c;
}

/*

more function to do (maybe)

*/

#endif 