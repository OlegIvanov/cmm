#include <cmm/gc.h>
#include <cmm/dbg.h>

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

static inline uintptr_t GC_get_key(uintptr_t ptr)
{
	return ptr >> (__WORDSIZE - KEY_BIT);
}

static inline uintptr_t GC_get_top(uintptr_t ptr)
{
	return (ptr << KEY_BIT) >> (__WORDSIZE - KEY_BIT);
}

static inline uintptr_t GC_get_bottom(uintptr_t ptr)
{
	return (ptr << (KEY_BIT + LOG_TOP_SZ)) >> (__WORDSIZE - KEY_BIT);
}

GC *GC_create()
{
	GC *gc = calloc(1, sizeof(GC));
	check_mem(gc);

	gc->all_nils = calloc(1, sizeof(BottomIndex));
	check_mem(gc->all_nils);

	GC_init_top_index(gc);
	GC_init_size_map(gc);
	GC_init_freelist(gc);

	return gc;
error:
	free(gc->all_nils);
	free(gc);
	return NULL;
}

int GC_get_size(GC *gc, size_t size)
{
	check(gc, "Argument 'gc' can't be NULL.");
	
	int i = 0;
	for(i = 0; i < SIZE_SZ; i++) {
		if(size <= gc->size_map[i]) {
			return i;
		}
	}

error:
	return -1;
}

void GC_allocate_block(GC *gc, int n, int sz)
{
	check(gc, "Argument 'gc' can't be NULL.");

	void *block = NULL;
	int rc = posix_memalign(&block, BLOCK_SZ, n * BLOCK_SZ);
	check(rc == 0, "Allocating block error occured.");

	GC_subdivide_block(gc, block, sz);

	BottomIndex *bi = NULL;
	uintptr_t top = GC_get_top((uintptr_t)block);

	if(gc->top_index[top] == gc->all_nils) {
		bi = GC_create_bottom_index(gc, block);
		gc->top_index[top] = bi;
	}

	bi = gc->top_index[top];
	uintptr_t key = GC_get_key((uintptr_t)block);
	
	if(bi->key != key) {
		while(bi->hash_link) {
			bi = bi->hash_link;
		}

		bi->hash_link = GC_create_bottom_index(gc, block);
		bi = bi->hash_link;
	}

	BlockHeader *header = GC_create_block_header(sz);

	uintptr_t bottom = GC_get_bottom((uintptr_t)block);
	bi->index[bottom] = header;
	
error:
	return;
}

void GC_subdivide_block(GC *gc, void *block, int sz)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(gc, "Argument 'block' can't be NULL.");

	List *freelist = gc->freelist[sz];
	check(List_count(freelist) == 0, "Free list must be empty before subdividing.");

	unsigned int i = 0;
	unsigned int object_sz = gc->size_map[sz];

	for(i = 0; i < BLOCK_SZ / object_sz; i++) {
		List_push(freelist, block + i * object_sz);
	}

error:
	return;
}

BottomIndex *GC_create_bottom_index(GC *gc, void *block)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(gc, "Argument 'block' can't be NULL.");

	BottomIndex *bi = calloc(1, sizeof(BottomIndex));
	check_mem(bi);
	
	bi->key = GC_get_key((uintptr_t)block);

	return bi;
error:
	return NULL;
}

BlockHeader *GC_create_block_header(GC *gc, int sz)
{
	BlockHeader *header = calloc(1, sizeof(BlockHeader));
	check_mem(header);

	header->size = gc->size_map[sz];

	return header;
error:
	return NULL;
}
