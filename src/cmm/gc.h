#ifndef _gc_h
#define _gc_h

#include <cmm/list.h>
#include <cmm/dbg.h>
#include <stdint.h>

#define LOG_TOP_SZ					11
#define LOG_BOTTOM_SZ				10
#define LOG_BLOCK_SZ				13

#define BLOCK_DISPL_MASK			0x1FFFUL

#define MIN_ALLOC_UNIT				16
#define LOG_MIN_ALLOC_UNIT			4

#define SIZE_SZ						((LOG_BLOCK_SZ) - (LOG_MIN_ALLOC_UNIT))

#define KEY_BIT						(__WORDSIZE - ((LOG_TOP_SZ) + (LOG_BOTTOM_SZ) + (LOG_BLOCK_SZ)))

#define WORD_SIZE_BYTES				(__WORDSIZE / 8)

#define MAX_BLOCK_OFFSET_WORDS_SZ	((GC_get_block(UINTPTR_MAX) + 1) / WORD_SIZE_BYTES)

typedef struct BlockHeader {
	uint32_t size;
	int16_t *map;
} BlockHeader;

struct BottomIndex;

typedef struct BottomIndex {
	BlockHeader **index;
	uintptr_t key;
	struct BottomIndex *hash_link;
} BottomIndex;

typedef struct HeapRange {
	void *low;
	void *high;
} HeapRange;

typedef struct GC {
	BottomIndex **top_index;
	BottomIndex *all_nils;
	uint32_t size_map[SIZE_SZ];
	int16_t *obj_map;
	List *freelist[SIZE_SZ];
	HeapRange heap_range;
	List *arp_stack;
} GC;

typedef struct ObjectDescriptor {
	uintptr_t ref_count;
	uintptr_t ref_map;
} ObjectDescriptor;

typedef struct ObjectHeader {
	ObjectDescriptor *desc;
} ObjectHeader;

inline uintptr_t GC_get_block(uintptr_t ptr);

GC *GC_create();

int GC_get_size(GC *gc, size_t size);

void GC_allocate_block(GC *gc, int n, int sz);

void GC_subdivide_block(GC *gc, void *block, int sz);

BottomIndex *GC_create_bottom_index(GC *gc, void *block);

BlockHeader *GC_create_block_header(GC *gc, int sz);

inline BlockHeader *GC_get_block_header(GC *gc, uintptr_t ptr);

#endif
