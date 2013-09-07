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
		return hdr(obj)->ref_count > 0;
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

	rtn(hdr);
	*obj = obj(hdr);
error:
	return;
}

void Object_release(GC *gc, void *obj)
{
	if(Object_validate(gc, obj)) {
		rls(hdr(obj));

		if(hdr(obj)->ref_count == 0) {
			BlockHeader *block_header = GC_get_block_header(gc, (uintptr_t)hdr(obj));
			
			void **objend = obj + block_header->size - sizeof(ObjectHeader);
			void **objaddr = NULL;
			
			for(objaddr = obj; objaddr < objend; objaddr++) {
				Object_release(gc, *objaddr);
			}

			List_push(gc->freelist[block_header->size_index], hdr(obj));
		}
	}
}

void Object_copy(GC *gc, void **lobj, void *robj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	if(Object_validate(gc, robj)) {
		rtn(hdr(robj));
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
		return hdr(obj)->ref_count;
	}
error:
	return -1;
}
