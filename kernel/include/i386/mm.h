#ifndef __MM_H__ 
#define __MM_H__
#include <i386/atomic.h>
#include <kernel/list.h>



#define PG_locked	 	 0	/* Page is locked. Don't touch. */
#define PG_error		 1
#define PG_referenced		 2
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


struct zone{
    unsigned long free_pages;
    unsigned long pages_min, pages_low, pages_high;
    unsigned long lowmem_reserve[MAX_NR_ZONES];

    struct list_head	active_list;
	struct list_head	inactive_list;
	unsigned long		nr_scan_active;
	unsigned long		nr_scan_inactive;
	unsigned long		nr_active;
	unsigned long		nr_inactive;
	unsigned long		pages_scanned;	   /* since last reclaim */
	int			all_unreclaimable; /* All pages pinned */

    struct pglist_data	*zone_pgdat;
	struct page		*zone_mem_map;
	/* zone_start_pfn == zone_start_paddr >> PAGE_SHIFT */
	unsigned long		zone_start_pfn;

	unsigned long		spanned_pages;	/* total size, including holes */
	unsigned long		present_pages;	/* amount of memory (excluding holes) */

}

struct zonelist {
	struct zone *zones[MAX_NUMNODES * MAX_NR_ZONES + 1]; // NULL delimited
};

struct page;
typedef struct pglist_data{
	struct zone node_zones[MAX_NR_ZONES];
	struct zonelist node_zonelists[GFP_ZONETYPES];
	int nr_zones;
	struct page* node_mem_map;
	unsigned long node_start_pfn;
	unsigned long node_present_pages; /* total number of physical pages */
	unsigned long node_spanned_pages; /* total size of physical page
					     range, including holes */
	int node_id;
}



struct page{
    unsigned long flags; //page status flags

    atomic_t _count; // page refence count

    atomic_t _mapcount; // page entry in page ? 

    unsigned long private;// 

    struct address_space * mmapping ; 

    unsigned long index;

    struct list_head lru;
}





#endif 

