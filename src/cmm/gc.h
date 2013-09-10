#ifndef _gc_h
#define _gc_h

#include <cmm/list.h>
#include <cmm/dbg.h>
#include <stdint.h>

#define LOG_TOP_SZ					11
#define TOP_SZ						(1UL << LOG_TOP_SZ)

#define LOG_BOTTOM_SZ				10
#define BOTTOM_SZ					(1UL << LOG_BOTTOM_SZ)

#define LOG_BLOCK_SZ				13
#define BLOCK_SZ					(1UL << LOG_BLOCK_SZ)

#define LOG_KEY_SZ					(__WORDSIZE - (LOG_TOP_SZ + LOG_BOTTOM_SZ + LOG_BLOCK_SZ))

#if __WORDSIZE == 64
	#define LOG_MIN_OBJECT_SZ		4
	#define LOG_WORD_BYTES			3
#else
	#define LOG_MIN_OBJECT_SZ		3
	#define LOG_WORD_BYTES			2
#endif

#define SIZE_SZ						(LOG_BLOCK_SZ - LOG_MIN_OBJECT_SZ)

#define MAX_BLOCK_OFFSET_WORDS_SZ	(BLOCK_SZ >> LOG_WORD_BYTES)

#define intcast(ptr)				((uintptr_t)(ptr))

#define	KEY(Ptr)					(intcast(Ptr) >> (__WORDSIZE - LOG_KEY_SZ))
#define	TOP(Ptr)					((intcast(Ptr) << LOG_KEY_SZ) >> (__WORDSIZE - LOG_TOP_SZ))
#define	BOTTOM(Ptr)					((intcast(Ptr) << (LOG_KEY_SZ + LOG_TOP_SZ)) >> (__WORDSIZE - LOG_BOTTOM_SZ))
#define BLOCK(Ptr)					(intcast(Ptr) & (UINTPTR_MAX >> (__WORDSIZE - LOG_BLOCK_SZ)))

// This only works if "b" is a power of two.
#define remainder(a, b)				((a) & (b - 1))

typedef struct BlockHeader {
	uint32_t size;
	int16_t *map;
	void *block;
	uint8_t *marks;
} BlockHeader;

struct BottomIndex;

typedef struct BottomIndex {
	BlockHeader **index;
	uintptr_t key;
	struct BottomIndex *hash_link;
} BottomIndex;

typedef struct Heap {
	void *low;
	void *high;
} Heap;

typedef struct GC {
	BottomIndex **top_index;
	BottomIndex *all_nils;
	uint32_t size_map[SIZE_SZ];
	int16_t *obj_map;
	Heap heap;
	List *block_list;
	List *freelists[SIZE_SZ];
	List *block_freelist;
	List *arp_stack;
} GC;

extern GC *__GC__;

GC *GC_create();
int GC_get_size(GC *gc, size_t size);
int GC_allocate_block(GC *gc, int blocks_number, uint16_t size_index);
inline void GC_unset_mark(BlockHeader *blkhdr, void *objhdr);
inline BlockHeader *GC_get_block_header(GC *gc, void *ptr);
int GC_sweep(GC *gc);
int GC_recycle_block(GC *gc, BlockHeader *blkhdr, uint16_t size_index);

#endif
