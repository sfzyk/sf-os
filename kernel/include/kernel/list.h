#ifndef __LIST_H__
#define __LIST_H__

#include <i386/bitops.h> 

struct list_head{
    struct list_head *prev , *next; 
};


#define list_entry(list_head ,type ,member)  (type *)((char *)list_head - offsetof(type,member))


#define init_list(ptr) do{\
    (ptr)->prev = (ptr);(ptr) -> next = (ptr);\
} while(0)


static inline void __list_add(  struct list_head * new, 
                                struct list_head* prev, 
                                struct list_head* next)
{
    prev-> next = new ;
    next->prev = new;
    new->next= next;
    new->prev = prev;                            
}

#define list_empty(ptr) ((ptr)->prev == (ptr) &&(ptr)->next == (ptr))

static inline void list_add(struct list_head *old , struct list_head* new){
    __list_add(new,old,old->next);
}

static inline void list_add_tail(struct list_head *old,struct list_head* new){
    __list_add(new,old->prev,old);
}

static inline void list_del(struct list_head *it){
    it->next->prev = it->prev;
    it->prev->next = it->next;
    /*
    list posion staff ;
    */
}
#endif