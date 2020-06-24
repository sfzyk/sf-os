#ifndef __I386_PROCESS_H__
#define __I386_PROCESS_H__
// for dsec struct 
struct desc_struct  {
    unsigned long a,b;
};

#define desc_empty(x) \
        (!(x->a+x->b))
#define desc_equal(x,y) \
        ((x->a==y->a)&(x->b==y->b))

        
#endif 