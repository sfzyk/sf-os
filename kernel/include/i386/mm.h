#ifndef __MM_H__ 
#define __MM_H__
#include <i386/atomic.h>
#include <kernel/list.h>
#include <i386/config.h>
#include <kernel/multiboot.h>

#define DMA_ZONE 0
#define NORMAL_ZONE 1
#define HIGH_ZONE 2

#define MAX_ORDER 11 

#define MAX_NR_ZONES 3
#define MAX_NUMNODES 1 

/*
dont support numa for now 
*/

#define TYPE_ZONES_SHIFT 2 
#define NUMNODES_SHIFT 0 

#define GFP_ZONETYPES 4



struct page;


struct free_area{
	struct list_head head;
	unsigned long nr_free;
};

struct per_cpu_pages{
	int count;
	int low;
	int high;
	int batch;    // num to add or remove 
	struct list_head list;
};

struct per_cpu_pageset{
	struct per_cpu_pages pcp[2];
};

 

struct zone {
    unsigned long free_pages;
    unsigned long pages_min, pages_low, pages_high;

	struct free_area free_area[MAX_ORDER];// use by buddy system 
    unsigned long lowmem_reserve[MAX_NR_ZONES]; // ? a zone contains all

	struct per_cpu_pageset pageset[NR_CPU];

    struct list_head	active_list;
	struct list_head	inactive_list;
	unsigned long		nr_scan_active;
	unsigned long		nr_scan_inactive;
	unsigned long		nr_active;
	unsigned long		nr_inactive;
	unsigned long		pages_scanned;	   /* since last reclaim */
	int			all_unreclaimable; /* All pages pinned */

    struct pglist_data	*pglist;


	struct page		*zone_mem_map;
	/* zone_start_pfn == zone_start_paddr >> PAGE_SHIFT */
	unsigned long		zone_start_pfn;

	unsigned long		spanned_pages;	/* total size, including holes */
	unsigned long		present_pages;	/* amount of memory (excluding holes) */

};

struct zonelist {
	struct zone *zones[MAX_NUMNODES * MAX_NR_ZONES + 1]; // NULL delimited
};


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
}pg_data_t;

typedef unsigned long pg_flags_t;

#define NODEZONE_SHIFT (sizeof(pg_flags_t)*8 - MAX_NR_ZONES - MAX_NUMNODES)

struct page{
    pg_flags_t flags; //page status flags

    atomic_t _count; // page refence count

    atomic_t _mapcount; // page entry in page ? 

    unsigned long private;// page order in 

    struct address_space * mmapping ; 

    unsigned long index;

	struct {			/* SLUB */
		unsigned inuse:15;
		unsigned objects:15;
		unsigned buddy:1;
		unsigned frozen:1;  /*frozen means it currently been used */
	};
	void ** free_list; /* SLUB */
	union{
    	struct list_head lru;
		struct page* next; 
		/* SLUB  use in cpu slab , or in buddy slub */
	};

	unsigned int pobjects;/* all slab in a paritial list */

	/*
		page　free : struct list_head when page is free 
		page in slub : the next pointer

	*/
};
void mm_page_init(multiboot_info_t* );
void node_alloc_mem_map(struct pglist_data*,unsigned int *);
struct mm_struct;

//内存的线性区
struct vm_area_struct{
	struct mm_struct * mm;// 这个线性区属于的内存描述符。
	unsigned long vm_start;// 线性区的第一个地址。
	unsigned long vm_end;//线性区之后的第一个地址。
};
//红黑树的根节点
struct rb_root{
		

};

//内存描述符
struct mm_struct{
	struct vm_area_struct * mmap; //线性区的链表头
	struct rb_root mm_rb;// 根节点
	struct vm_area_struct * mmap_cache;//最近访问的线性区结构

};



#endif 

