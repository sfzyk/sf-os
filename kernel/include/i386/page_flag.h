#ifndef __PAGE_FLAG_H__
#define __PAGE_FLAG_H__

#include <i386/bitops.h>

// zone flags 
/* Zone modifiers in GFP_ZONEMASK (see linux/mmzone.h - low two bits) */
#define __GFP_DMA	0x01
#define __GFP_HIGHMEM	0x02

/*
 * Action modifiers - doesn't change the zoning
 *
 * __GFP_REPEAT: Try hard to allocate the memory, but the allocation attempt
 * _might_ fail.  This depends upon the particular VM implementation.
 *
 * __GFP_NOFAIL: The VM implementation _must_ retry infinitely: the caller
 * cannot handle allocation failures.
 *
 * __GFP_NORETRY: The VM implementation must not retry indefinitely.
 */
#define __GFP_WAIT	0x10	/* Can wait and reschedule? */
#define __GFP_HIGH	0x20	/* Should access emergency pools? */
#define __GFP_IO	0x40	/* Can start physical IO? */
#define __GFP_FS	0x80	/* Can call down to low-level FS? */
#define __GFP_COLD	0x100	/* Cache-cold page required */
#define __GFP_NOWARN	0x200	/* Suppress page allocation failure warning */
#define __GFP_REPEAT	0x400	/* Retry the allocation.  Might fail */
#define __GFP_NOFAIL	0x800	/* Retry for ever.  Cannot fail */
#define __GFP_NORETRY	0x1000	/* Do not retry.  Might fail */
#define __GFP_NO_GROW	0x2000	/* Slab internal usage */
#define __GFP_COMP	0x4000	/* Add compound page metadata */
#define __GFP_ZERO	0x8000	/* Return zeroed page on success */

#define __GFP_BITS_SHIFT 16	/* Room for 16 __GFP_FOO bits */
#define __GFP_BITS_MASK ((1 << __GFP_BITS_SHIFT) - 1)

/* if you forget to add the bitmask here kernel will crash, period */
#define GFP_LEVEL_MASK (__GFP_WAIT|__GFP_HIGH|__GFP_IO|__GFP_FS| \
			__GFP_COLD|__GFP_NOWARN|__GFP_REPEAT| \
			__GFP_NOFAIL|__GFP_NORETRY|__GFP_NO_GROW|__GFP_COMP)

#define GFP_ATOMIC	(__GFP_HIGH)
#define GFP_NOIO	(__GFP_WAIT)
#define GFP_NOFS	(__GFP_WAIT | __GFP_IO)
#define GFP_KERNEL	(__GFP_WAIT | __GFP_IO | __GFP_FS)
#define GFP_USER	(__GFP_WAIT | __GFP_IO | __GFP_FS)
#define GFP_HIGHUSER	(__GFP_WAIT | __GFP_IO | __GFP_FS | __GFP_HIGHMEM)


// page status 
#define PG_locked	 	 0	/* Page is locked. Don't touch. */
#define PG_error		 1
#define PG_referenced	 2
#define PG_uptodate		 3

#define PG_dirty	 	 4
#define PG_lru			 5
#define PG_active		 6
#define PG_slab			 7	/* slab debug (Suparna wants this) */

#define PG_highmem		 8
#define PG_checked		 9	/* kill me in 2.5.<early>. */
#define PG_arch_1		10
#define PG_reserved		11

#define PG_private		12	/* Has something at ->private */
#define PG_writeback		13	/* Page is under writeback */
#define PG_nosave		14	/* Used for system suspend/resume */
#define PG_compound		15	/* Part of a compound page */

#define PG_swapcache		16	/* Swap page: swp_entry_t in private */
#define PG_mappedtodisk		17	/* Has blocks allocated on-disk */
#define PG_reclaim		18	/* To be reclaimed asap */
#define PG_nosave_free		19	/* Free, should not be written */


struct page_state {
	unsigned long nr_dirty;		/* Dirty writeable pages */
	unsigned long nr_writeback;	/* Pages under writeback */
	unsigned long nr_unstable;	/* NFS unstable pages */
	unsigned long nr_page_table_pages;/* Pages used for pagetables */
	unsigned long nr_mapped;	/* mapped into pagetables */
	unsigned long nr_slab;		/* In slab */

	/*
	 * The below are zeroed by get_page_state().  Use get_full_page_state()
	 * to add up all these.
	 */
	unsigned long pgpgin;		/* Disk reads */
	unsigned long pgpgout;		/* Disk writes */
	unsigned long pswpin;		/* swap reads */
	unsigned long pswpout;		/* swap writes */
	unsigned long pgalloc_high;	/* page allocations */

	unsigned long pgalloc_normal;
	unsigned long pgalloc_dma;
	unsigned long pgfree;		/* page freeings */
	unsigned long pgactivate;	/* pages moved inactive->active */
	unsigned long pgdeactivate;	/* pages moved active->inactive */

	unsigned long pgfault;		/* faults (major+minor) */
	unsigned long pgmajfault;	/* faults (major only) */
	unsigned long pgrefill_high;	/* inspected in refill_inactive_zone */
	unsigned long pgrefill_normal;
	unsigned long pgrefill_dma;

	unsigned long pgsteal_high;	/* total highmem pages reclaimed */
	unsigned long pgsteal_normal;
	unsigned long pgsteal_dma;
	unsigned long pgscan_kswapd_high;/* total highmem pages scanned */
	unsigned long pgscan_kswapd_normal;

	unsigned long pgscan_kswapd_dma;
	unsigned long pgscan_direct_high;/* total highmem pages scanned */
	unsigned long pgscan_direct_normal;
	unsigned long pgscan_direct_dma;
	unsigned long pginodesteal;	/* pages reclaimed via inode freeing */

	unsigned long slabs_scanned;	/* slab objects scanned */
	unsigned long kswapd_steal;	/* pages reclaimed by kswapd */
	unsigned long kswapd_inodesteal;/* reclaimed via kswapd inode freeing */
	unsigned long pageoutrun;	/* kswapd's calls to page reclaim */
	unsigned long allocstall;	/* direct reclaim calls */

	unsigned long pgrotated;	/* pages rotated to tail of the LRU */
};

#define PageChecked(page)	test_bit(PG_checked, &(page)->flags)
#define SetPageChecked(page)	set_bit(PG_checked, &(page)->flags)
#define ClearPageChecked(page)	clear_bit(PG_checked, &(page)->flags)

#define PageReserved(page)	test_bit(PG_reserved, &(page)->flags)
#define SetPageReserved(page)	set_bit(PG_reserved, &(page)->flags)
#define ClearPageReserved(page)	clear_bit(PG_reserved, &(page)->flags)
#define __ClearPageReserved(page)	__clear_bit(PG_reserved, &(page)->flags)

#define SetPagePrivate(page)	set_bit(PG_private, &(page)->flags)
#define ClearPagePrivate(page)	clear_bit(PG_private, &(page)->flags)
#define PagePrivate(page)	test_bit(PG_private, &(page)->flags)
#define __SetPagePrivate(page)  __set_bit(PG_private, &(page)->flags)
#define __ClearPagePrivate(page) __clear_bit(PG_private, &(page)->flags)


#endif 