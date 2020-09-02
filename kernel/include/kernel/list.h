#ifndef __LIST_H__
#define __LIST_H__

struct list_head{
    struct list_head * prev , next; 
};

#define INIT_LIST_HEAD(ptr) do{\
    (ptr)->prev = (ptr);(ptr) -> next = (ptr);
}while(0)

static inline void __list_add(  struct list_head * new, 
                                struct list_head* prev, 
                                struct list_head* next)
{
    prev-> next = new ;
    next->prev = new;
    new->next= next;
    new->prev = prev;                            
}


static inline void list_add(struct list_head *old , struct list_head new){
    __list_add(new,old,old->next);
}

static inline void list_add_tail(struct list_head *old,struct list_head new){
    __list_add(new,old->prev,old);
}

#endif