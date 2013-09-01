#include <cmm/cmm.h>
#include <cmm/gc.h>

GC *__GC__;

static inline ObjectHeader *Object_get_header(void *obj)
{
	return (ObjectHeader *)obj - 1;
}

void Object_new(GC *gc, size_t size, void **obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	int sz = GC_get_size(gc, size + sizeof(ObjectHeader));
	List *freelist = gc->freelist[sz];

	if(List_count(freelist) == 0) {
		GC_allocate_block(gc, 1, sz);
	}

	ObjectDescriptor *desc = calloc(1, sizeof(ObjectDescriptor));
	check_mem(desc);

	ObjectHeader *hdr = List_shift(freelist);
	hdr->desc = desc;
	
	hdr->desc->ref_count++;

	*obj = hdr + 1;

	/*
	BlockHeader *blkhdr = GC_get_block_header(gc, objhdr);
	blkhdr->ref_count++;
	*/

error:
	return;
}

void Object_release(void **obj)
{
	if(*obj == NULL) return;
	
	ObjectHeader *hdr = Object_get_header(*obj);
	
	hdr->desc->ref_count--;

	if(hdr->desc->ref_count == 0) {
		Object_release_childs(__GC__, obj);
	}

	/*
	BlockHeader *blkhdr = GC_get_block_header(gc, *obj);
	blkhdr->ref_count--;

	if(blkhdr->ref_count == 0) {
		
	}
	*/

	/*
	ObjectHeader *header = Object_get_header(*obj);
	
	header->desc->ref_count--;

	if(header->desc->ref_count == 0) {
		Object_release_childs(obj);
	}
	*/

	//printf("\n%d\n", desc->ref_count);

	/*
	if(*object == NULL) return;

	Header *header = Object_get_header(*object);
	header->ref_count--;

	if(header->ref_count == 0) {
		Object_release_childs(object);
		free(header);
	}
	*/
}

void Object_release_childs(GC *gc, void **obj)
{
	BlockHeader *blkhdr = GC_get_block_header(gc, (uintptr_t)*obj);

	uint32_t objsz = blkhdr->size - sizeof(ObjectHeader);
	void *ptrcnd = NULL;

	for(ptrcnd = *obj; ptrcnd < *obj + objsz; ptrcnd++) {
		GC_test_ptr(gc, (uintptr_t)ptrcnd);
	}
	
	/*	
	uintptr_t top = GC_get_top((uintptr_t)ptr);
	BottomIndex *bi = gc->top_index[top];

	uintptr_t key = GC_get_key((uintptr_t)ptr);
	while(bi->key != key) bi = bi->hash_link;

	uintptr_t bottom = GC_get_bottom((uintptr_t)ptr);
	return bi->index[bottom];
	*/

	/*
	for(i = 0; i < objsz / WORDSIZEBYTE; i++) {
	}
	*/

	/*
	Header *hdr = Object_get_header(*obj);

	int i = 0;
	uint32_t bit = 0;
	uint32_t ref_map = hdr->desc->ref_map;

	for(i = 0; i < BIT_SIZE; i++) {
		bit = ref_map & ONE;

		if(bit) Object_release(*object + i * ALIGNMENT);

		ref_map = ref_map >> ONE;
	}
	*/
	/*
	Header *header = Object_get_header(*object);

	int i = 0;
	uint32_t bit = 0;
	uint32_t ref_map = header->descriptor.ref_map;

	for(i = 0; i < BIT_SIZE; i++) {
		bit = ref_map & ONE;

		if(bit) Object_release(*object + i * ALIGNMENT);

		ref_map = ref_map >> ONE;
	}
	*/
}
