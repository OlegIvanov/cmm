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

void *GC_allocate_block(int n)
{
	void *block = NULL;

	int rc = posix_memalign(&block, BLOCK_SZ, n * BLOCK_SZ);
	check(rc == 0, "Allocating block error occured.");

	return block;
error:
	return NULL;
}
