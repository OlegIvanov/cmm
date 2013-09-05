#ifndef _gc_h
#define _gc_h

#include <cmm/list.h>
#include <cmm/dbg.h>
#include <stdint.h>

#define LOG_TOP_SZ					11
#define LOG_BOTTOM_SZ				10
#define LOG_BLOCK_SZ				13

#define BLOCK_DISPL_MASK			0x1FFFUL

#if __WORDSIZE == 64
	#define LOG_MIN_OBJECT_SZ		4
#else
	#define LOG_MIN_OBJECT_SZ		3
#endif

#define SIZE_SZ						((LOG_BLOCK_SZ) - (LOG_MIN_OBJECT_SZ))

#define KEY_BIT						(__WORDSIZE - ((LOG_TOP_SZ) + (LOG_BOTTOM_SZ) + (LOG_BLOCK_SZ)))

#define WORD_SIZE_BYTES				(__WORDSIZE / 8)

#define MAX_BLOCK_OFFSET_WORDS_SZ	((1UL << LOG_BLOCK_SZ) / WORD_SIZE_BYTES)

typedef struct BlockHeader {
	uint32_t size;
	uint16_t size_index;
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

extern GC *__GC__;

inline uintptr_t GC_get_block(uintptr_t ptr);
GC *GC_create();
int GC_get_size(GC *gc, size_t size);
void GC_allocate_block(GC *gc, int n, uint16_t size_index);
void GC_subdivide_block(GC *gc, void *block, uint16_t size_index);
BottomIndex *GC_create_bottom_index(GC *gc, void *block);
BlockHeader *GC_create_block_header(GC *gc, uint16_t size_index);
inline BlockHeader *GC_get_block_header(GC *gc, uintptr_t ptr);

#endif
