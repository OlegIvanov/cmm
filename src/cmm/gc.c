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
		gc->freelist[i] = List_create();
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
		object_size_words = gc->size_map[j] / WORD_SIZE_BYTES;

		for(i = 0; i < MAX_BLOCK_OFFSET_WORDS_SZ; i++) {
			*(gc->obj_map + j * MAX_BLOCK_OFFSET_WORDS_SZ + i) = i % object_size_words;
		}
	}

	return 0;
error:
	return -1;
}

#define BUF_SIZE 1000

static int GC_init_heap_range(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	char maps_filename[BUF_SIZE];
	char file_line[BUF_SIZE];
	const char *heap = "[heap]";

	pid_t pid = getpid();

	sprintf(maps_filename, "/proc/%d/maps", pid);
	
	FILE *maps_file = fopen(maps_filename, "r");
	check(maps_file, "maps file has't been opened");

	while(fgets(file_line, BUF_SIZE, maps_file) != NULL) {
		if(strstr(file_line, heap) != NULL) {
			break;
		}
	}

	sscanf(file_line, "%p-%p", &gc->heap_range.low, &gc->heap_range.high);

	fclose(maps_file);

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

	GC_init_top_index(gc);
	GC_init_size_map(gc);
	GC_init_obj_map(gc);
	GC_init_heap_range(gc);
	GC_init_freelist(gc);

	return gc;
error:
	if(gc) {
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

void GC_allocate_block(GC *gc, int n, uint16_t size_index)
{
	check(gc, "Argument 'gc' can't be NULL.");

	void *block = NULL;

	int rc = posix_memalign(&block, BLOCK_SZ, n * BLOCK_SZ);
	check(rc == 0, "Allocating block error occured.");

	GC_subdivide_block(gc, block, size_index);

	BottomIndex *bi = NULL;
	uintptr_t top = TOP((uintptr_t)block);

	if(gc->top_index[top] == gc->all_nils) {
		bi = GC_create_bottom_index(block);
		gc->top_index[top] = bi;
	}

	bi = gc->top_index[top];

	while(bi->key != KEY((uintptr_t)block)) {
		bi = bi->hash_link;

		if(bi == NULL) {
			bi = GC_create_bottom_index(block);
			break;
		}
	}

	BlockHeader *header = GC_create_block_header(gc, size_index);
	bi->index[BOTTOM((uintptr_t)block)] = header;
error:
	return;
}

void GC_subdivide_block(GC *gc, void *block, uint16_t size_index)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(block, "Argument 'block' can't be NULL.");

	List *freelist = gc->freelist[size_index];
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
	
	bi->key = KEY((uintptr_t)block);

	return bi;
error:
	if(bi) {
		free(bi->index);
	}	
	free(bi);
	return NULL;
}

BlockHeader *GC_create_block_header(GC *gc, uint16_t size_index)
{
	check(gc, "Argument 'gc' can't be NULL.");

	BlockHeader *header = calloc(1, sizeof(BlockHeader));
	check_mem(header);

	header->size = gc->size_map[size_index];
	header->size_index = size_index;

	header->map = gc->obj_map + size_index * MAX_BLOCK_OFFSET_WORDS_SZ;

	return header;
error:
	return NULL;
}

inline BlockHeader *GC_get_block_header(GC *gc, uintptr_t ptr)
{
	if(ptr < (uintptr_t)gc->heap_range.low
	|| ptr > (uintptr_t)gc->heap_range.high) return NULL;

	BottomIndex *bi = gc->top_index[TOP(ptr)];

	while(bi->key != KEY(ptr)) {
		bi = bi->hash_link;
		if(bi == NULL) return NULL;
	}

	return bi->index[BOTTOM(ptr)];
}
