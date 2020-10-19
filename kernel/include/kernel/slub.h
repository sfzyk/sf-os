#ifndef __SLUB_H__
#define __SLUB_H__

void set_min_partial(struct kmem_cache *s, unsigned min_obj);
void set_cpu_partial(struct kmem_cache *s);


#endif 