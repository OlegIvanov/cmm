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
}

void Object_release_childs(GC *gc, void **obj)
{
	void *objptr = *obj;

	BlockHeader *blkhdr = GC_get_block_header(gc, (uintptr_t)objptr);

	uint32_t objsz = blkhdr->size - sizeof(ObjectHeader);
	void *ptr = NULL;

	for(ptr = objptr; ptr < objptr + objsz; ptr++) {
		BlockHeader *blkhdrtemp = GC_get_block_header(gc, (uintptr_t)ptr);

		if(blkhdrtemp != NULL) {
			uintptr_t blkdispl = GC_get_block_displ((uintptr_t)ptr) / WORDSIZEBYTE;
				
			//printf("\n%d\n", blkdispl);

			if(blkhdrtemp->map[blkdispl] == 0) {
				//printf("\n%d\n", blkdispl);

				Object_release(&ptr);
				
				
			}
		}
	}
}
