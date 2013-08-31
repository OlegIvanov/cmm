#include <cmm/gc.h>
#include <cmm/dbg.h>

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

	if(gc->top_index[sz] == gc->all_nils) {
				
	}

error:
	return;
}

void GC_subdivide_block(GC *gc, void *block, int sz)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(gc, "Argument 'block' can't be NULL.");

	List *freelist = gc->freelist[sz];
	check(List_count(freelist) == 0, "Free list must be empty before subdividing.");

	int i = 0;
	uint16_t object_sz = gc->size_map[sz];

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
	
	bi->key = block >> LOG_TOP_SZ + LOG_BOTTOM_SZ + LOG_BLOCK_SZ;
	bi->hash_link = List_create();

	return bi;
error:
	return NULL;
}
