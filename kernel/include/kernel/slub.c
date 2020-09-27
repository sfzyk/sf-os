#include <i386/page.h>
#include <i386/page_table.h>
#include "kernel/slab.h"

#define MAX_PARTIAL 10
#define MIN_PARTIAL 5

static void set_min_partial(struct kmem_cache *s, unsigned min_obj){    
	if (min_obj < MIN_PARTIAL)
		min_obj = MIN_PARTIAL;
	else if (min_obj > MAX_PARTIAL)
		min_obj = MAX_PARTIAL;
	s->min_partial = min_obj;
}

static void set_cpu_partial(struct kmem_cache *s)
{
 
	/*
	 * cpu_partial determined the maximum number of objects kept in the
	 * per cpu partial lists of a processor.
	 *
	 * Per cpu partial lists mainly contain slabs that just have one
	 * object freed. If they are used for allocation then they can be
	 * filled up again with minimal effort. The slab will never hit the
	 * per node partial lists and therefore no locking will be required.
	 *
	 * This setting also determines
	 *
	 * A) The number of objects from per cpu partial slabs dumped to the
	 *    per node list when we reach the limit.
	 * B) The number of objects in cpu partial slabs to extract from the
	 *    per node list when we run out of per cpu objects. We only fetch
	 *    50% to keep some capacity around for frees.
	 */
	if (s->size >= PAGE_SIZE)
		s->cpu_partial = 2;
	else if (s->size >= 1024)
		s->cpu_partial = 6;
	else if (s->size >= 256)
		s->cpu_partial = 13;
	else
		s->cpu_partial = 30;
}