#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__
#include "i386/page.h"
#include "i386/page_flag.h"
#include "i386/mm.h"

struct page * __alloc_pages(unsigned int gfp_mask, unsigned int order, struct zonelist *zonelist);
void __free_pages(struct page* pg, unsigned int order);

#endif