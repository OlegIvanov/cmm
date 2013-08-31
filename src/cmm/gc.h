#ifndef _gc_h
#define _gc_h

#include <cmm/list.h>
#include <stdint.h>

#define TOP_SZ					2048
#define LOG_TOP_SZ				11

#define BOTTOM_SZ				1024
#define LOG_BOTTOM_SZ			10

#define	BLOCK_SZ				8192
#define LOG_BLOCK_SZ			13

#define MIN_ALLOC_UNIT			16
#define LOG_MIN_ALLOC_UNIT		4

#define SIZE_SZ					((LOG_BLOCK_SZ) - (LOG_MIN_ALLOC_UNIT))

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
	uint16_t size_map[SIZE_SZ];
	List *freelist[SIZE_SZ];
} GC;

GC *GC_create();

static int GC_init_top_index(GC *gc);

static int GC_init_size_map(GC *gc);

void *GC_allocate_block(int n);

#endif
