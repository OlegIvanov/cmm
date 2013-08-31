#include <cmm/gc.h>
#include <cmm/dbg.h>
#include <math.h>

GC *GC_create()
{
	GC *gc = calloc(1, sizeof(GC));
	check_mem(gc);

	gc->all_nils = calloc(1, sizeof(BottomIndex));
	check_mem(gc->all_nils);

	GC_init_top_index(gc);
	GC_init_size_map(gc);

	return gc;
error:
	free(gc->all_nils);
	free(gc);
	return NULL;
}

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

void *GC_allocate_block(int n)
{
	void *block = NULL;

	int rc = posix_memalign(&block, BLOCK_SZ, n * BLOCK_SZ);
	check(rc == 0, "Allocating block error occured.");

	return block;
error:
	return NULL;
}
