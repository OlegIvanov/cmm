#include <cmm/object.h>
#include <cmm/gc.h>

static inline int Object_validate_ptr(GC *gc, void *ptr)
{
	BlockHeader *block_header = GC_get_block_header(gc, (uintptr_t)ptr);

	if(block_header) {
		if(block_header->map[BLOCK((uintptr_t)ptr) / WORD_SIZE_BYTES - 1] == 0) {
			return 1;
		}
	}

	return 0;
}

static inline int Object_validate(GC *gc, void *obj)
{
	if(Object_validate_ptr(gc, obj)) {
		return header(obj)->ref_count > 0;
	}

	return 0;
}

void Object_new(GC *gc, size_t type_size, void **obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	Object_release(gc, *obj);

	int size_index = GC_get_size(gc, type_size + sizeof(ObjectHeader));
	List *freelist = gc->freelist[size_index];

	if(List_count(freelist) == 0) {
		GC_allocate_block(gc, 1, size_index);
	}

	ObjectHeader *hdr = List_shift(freelist);
	memset(hdr, 0, gc->size_map[size_index]);

	retain(hdr);
	*obj = object(hdr);
error:
	return;
}

void Object_release(GC *gc, void *obj)
{
	if(Object_validate(gc, obj)) {
		release(header(obj));

		if(header(obj)->ref_count == 0) {
			BlockHeader *block_header = GC_get_block_header(gc, (uintptr_t)header(obj));
			
			void **interior = obj;
			void **objend = obj + block_header->size - sizeof(ObjectHeader);
			while(interior < objend) {
				Object_release(gc, *interior);
				interior++;
			}

			List_push(gc->freelist[block_header->size_index], header(obj));
		}
	}
}

void Object_copy(GC *gc, void **lobj, void *robj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	if(Object_validate(gc, robj)) {
		retain(header(robj));
	}
	
	Object_release(gc, *lobj);

	*lobj = robj;
error:
	return;
}

int Object_retain_count(GC *gc, void *obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	if(Object_validate(gc, obj)) {
		return header(obj)->ref_count;
	}
error:
	return -1;
}
