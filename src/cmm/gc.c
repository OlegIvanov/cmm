#include <cmm/gc.h>
#include <cmm/dbg.h>
#include <unistd.h>

GC *__GC__;

static int GC_init_top_index(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	uintptr_t i = 0;
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
		gc->size_map[i] = 1U << (LOG_MIN_OBJECT_SZ + i);
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
		gc->freelists[i] = List_create();
	}

	return 0;
error:
	return -1;
}

static int GC_init_obj_map(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	uint32_t j = 0;
	uint32_t i = 0;
	uint32_t object_size_words = 0;

	for(j = 0; j < SIZE_SZ; j++) {
		object_size_words = gc->size_map[j] >> LOG_WORD_BYTES;

		for(i = 0; i < MAX_BLOCK_OFFSET_WORDS_SZ; i++) {
			*(gc->obj_map + j * MAX_BLOCK_OFFSET_WORDS_SZ + i) = remainder(i, object_size_words);
		}
	}

	return 0;
error:
	return -1;
}

static int GC_init_heap(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	gc->heap.low = (void *)UINTPTR_MAX;
	gc->heap.high = NULL;

	return 0;
error:
	return -1;
}

__attribute__((constructor)) void GC_init()
{
	__GC__ = GC_create();
}

GC *GC_create()
{
	GC *gc = calloc(1, sizeof(GC));
	check_mem(gc);

	gc->top_index = calloc(TOP_SZ, sizeof(BottomIndex *));
	check_mem(gc->top_index);

	gc->all_nils = GC_create_bottom_index(NULL);
	check(gc->all_nils, "Error creating all_nils table.");

	gc->obj_map = calloc(1, SIZE_SZ * MAX_BLOCK_OFFSET_WORDS_SZ * sizeof(int16_t));
	check_mem(gc->obj_map);

	gc->arp_stack = List_create();
	gc->block_list = List_create();
	gc->block_freelist = List_create();

	GC_init_top_index(gc);
	GC_init_size_map(gc);
	GC_init_obj_map(gc);
	GC_init_freelist(gc);
	GC_init_heap(gc);

	return gc;
error:
	if(gc) {
		List_destroy(gc->block_freelist);
		List_destroy(gc->block_list);
		List_destroy(gc->arp_stack);
		free(gc->obj_map);
		if(gc->all_nils) {
			free(gc->all_nils->index);
		}
		free(gc->all_nils);
		free(gc->top_index);
	}
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

static int GC_update_heap(GC *gc, void *block, size_t size)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(block, "Argument 'block' can't be NULL.");

	if(block < gc->heap.low) {
		gc->heap.low = block;
	}
	if(block + size > gc->heap.high) {
		gc->heap.high = block + size;
	}

	return 0;
error:
	return -1;
}

void GC_allocate_block(GC *gc, int blocks_number, uint16_t size_index)
{
	check(gc, "Argument 'gc' can't be NULL.");

	void *block = NULL;
	int rc = posix_memalign(&block, BLOCK_SZ, blocks_number * BLOCK_SZ);
	check(rc == 0, "Allocating block error occured.");

	memset(block, 0, blocks_number * BLOCK_SZ);

	GC_subdivide_block(gc, block, size_index);
	
	BottomIndex *bi = gc->top_index[TOP(block)];

	if(bi == gc->all_nils) {
		bi = GC_create_bottom_index(block);
		gc->top_index[TOP(block)] = bi;
	} else {
		while(bi->key != KEY(block)) {
			if(bi->hash_link == NULL) {
				bi->hash_link = GC_create_bottom_index(block);
			}
			bi = bi->hash_link;
		}
	}

	BlockHeader *header = GC_create_block_header(gc, size_index);
	bi->index[BOTTOM(block)] = header;

	GC_update_heap(gc, block, blocks_number * BLOCK_SZ);
	List_push(gc->block_list, header);
error:
	return;
}

void GC_subdivide_block(GC *gc, void *block, uint16_t size_index)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(block, "Argument 'block' can't be NULL.");

	List *freelist = gc->freelists[size_index];
	check(List_count(freelist) == 0, "Free list must be empty before subdividing.");

	uint32_t i = 0;
	uint32_t object_size_bytes = gc->size_map[size_index];

	for(i = 0; i < BLOCK_SZ / object_size_bytes; i++) {
		List_push(freelist, block + i * object_size_bytes);
	}
error:
	return;
}

BottomIndex *GC_create_bottom_index(void *block)
{
	BottomIndex *bi = calloc(1, sizeof(BottomIndex));
	check_mem(bi);

	bi->index = calloc(BOTTOM_SZ, sizeof(BlockHeader *));
	check_mem(bi->index);
	
	bi->key = KEY(block);

	return bi;
error:
	if(bi) {
		free(bi->index);
	}	
	free(bi);
	return NULL;
}

static void GC_set_marks(GC *gc, BlockHeader *header, int marks_size_bits)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(header, "Argument 'header' can't be NULL.");

	int i = 0;
	int marks_size_bytes = marks_size_bits / 8;
	int remain_bits = remainder(marks_size_bits, 8);

	for(i = 0; i < marks_size_bytes; i++) {
		header->marks[i] = UINT8_MAX;
	}
	if(remain_bits > 0) {
		header->marks[i] = UINT8_MAX >> (8 - remain_bits);
	}
error:
	return;
}

inline void GC_unset_mark(BlockHeader *block_header, void *object_header)
{
	int unset_bit = block_header->map[BLOCK(object_header) >> LOG_WORD_BYTES];
	block_header->marks[unset_bit / 8] &= ~(1U << remainder(unset_bit, 8));
}

static int GC_get_marks_size_bytes(int marks_size_bits)
{
	return marks_size_bits / 8 + (remainder(marks_size_bits, 8) > 0);
}

BlockHeader *GC_create_block_header(GC *gc, uint16_t size_index)
{
	BlockHeader *header = calloc(1, sizeof(BlockHeader));
	check_mem(header);

	check(gc, "Argument 'gc' can't be NULL.");

	header->size = gc->size_map[size_index];
	header->map = gc->obj_map + size_index * MAX_BLOCK_OFFSET_WORDS_SZ;

	int marks_size_bits = BLOCK_SZ / header->size;
	
	header->marks = calloc(1, GC_get_marks_size_bytes(marks_size_bits));
	check_mem(header->marks);

	GC_set_marks(gc, header, marks_size_bits);

	return header;
error:
	if(header) {
		free(header->marks);
	}
	free(header);
	return NULL;
}

inline BlockHeader *GC_get_block_header(GC *gc, void *ptr)
{
	if(ptr < gc->heap.low || ptr > gc->heap.high) return NULL;

	BottomIndex *bi = gc->top_index[TOP(ptr)];

	while(bi->key != KEY(ptr)) {
		bi = bi->hash_link;
		if(bi == NULL) return NULL;
	}

	return bi->index[BOTTOM(ptr)];
}

static inline int GC_check_marks_if_zero(uint8_t *marks, int marks_length)
{
	int i = 0;
	for(i = 0; i < marks_length; i++) {
		if(marks[i] != 0) return 0;
	}

	return 1;
}

static int GC_remove_unused_blocks(GC *gc, List *unused_blocks)
{
	check(gc, "Argument 'gc' can't be NULL.");

	LIST_FOREACH(unused_blocks, first, next, cur) {
		List_remove(gc->block_list, cur->value);
	}

	return 0;
error:
	return -1;
}

int GC_sweep(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	List *unused_blocks = List_create();

	LIST_FOREACH(gc->block_list, first, next, cur) {
		BlockHeader *header = cur->value;
		int marks_length = GC_get_marks_size_bytes(BLOCK_SZ / header->size);

		if(GC_check_marks_if_zero(header->marks, marks_length)) {
			List_push(gc->block_freelist, header);
			List_push(unused_blocks, cur);
		}
	}

	GC_remove_unused_blocks(gc, unused_blocks);
	List_destroy(unused_blocks);

	return 0;
error:
	return -1;
}
