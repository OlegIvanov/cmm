#ifndef _gc_h
#define _gc_h

#include <cmm/list.h>
#include <cmm/dbg.h>
#include <stdint.h>
#include <math.h>

#define TOP_SZ					2048
#define LOG_TOP_SZ				11

#define BOTTOM_SZ				1024
#define LOG_BOTTOM_SZ			10

#define	BLOCK_SZ				8192
#define LOG_BLOCK_SZ			13

#define MIN_ALLOC_UNIT			16
#define LOG_MIN_ALLOC_UNIT		4

#define SIZE_SZ					((LOG_BLOCK_SZ) - (LOG_MIN_ALLOC_UNIT))

#define WORD_BIT				sizeof(void *) * 8
#define KEY_BIT					(WORD_BIT - ((LOG_TOP_SZ) + (LOG_BOTTOM_SZ) + (LOG_BLOCK_SZ)))

typedef struct BlockHeader {
	uint32_t size;
} BlockHeader;

typedef struct BottomIndex {
	BlockHeader *index[BOTTOM_SZ];
	uint32_t key;
	List *hash_link;
} BottomIndex;

typedef struct GC {
	BottomIndex *top_index[TOP_SZ];
	BottomIndex *all_nils;
	uint32_t size_map[SIZE_SZ];
	List *freelist[SIZE_SZ];
} GC;

GC *GC_create();

int GC_get_size(GC *gc, size_t size);

void GC_allocate_block(GC *gc, int n, int sz);

void GC_subdivide_block(GC *gc, void *block, int sz);

static int GC_init_top_index(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	int i = 0;
	for(i = 0; i < TOP_SZ; i++) {
		gc->top_index[i] = gc->all_nils;
	}

	return 0;
error:
	return -1;
}

static int GC_init_size_map(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	int i = 0;
	for(i = 0; i < SIZE_SZ; i++) {
		gc->size_map[i] = powl(2, i + LOG_MIN_ALLOC_UNIT);
	}

	return 0;
error:
	return -1;
}

static int GC_init_freelist(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	int i = 0;
	for(i = 0; i < SIZE_SZ; i++) {
		gc->freelist[i] = List_create();
	}

	return 0;
error:
	return -1;
}

#endif
