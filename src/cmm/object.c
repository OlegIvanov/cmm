#include <cmm/object.h>
#include <cmm/gc.h>

static inline int Object_validate(GC *gc, void *obj)
{
	BlockHeader *blkhdr = GC_get_block_header(gc, obj);
	ObjectHeader *objhdr = header(obj);

	if(blkhdr) {
		if(blkhdr->map[BLOCK(objhdr) >> LOG_WORD_BYTES] == 0) {
			return objhdr->ref_count > 0;
		}
	}

	return 0;
}

void Object_new(GC *gc, size_t type_size, void **obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	Object_release(gc, *obj);

	int size_index = GC_get_size(gc, type_size + sizeof(ObjectHeader));
	List *freelist = gc->freelists[size_index];

	if(List_count(freelist) == 0) {
		if(List_last(gc->blkfreelist) == NULL) {
			GC_sweep(gc);
			if(List_last(gc->blkfreelist) == NULL) {
				GC_allocate_block(gc, 1, size_index);
			} else {
				GC_recycle_block(gc, List_pop(gc->blkfreelist), size_index);
			}
		} else {
			GC_recycle_block(gc, List_pop(gc->blkfreelist), size_index);
		}
	}

	ObjectHeader *objhdr = List_shift(freelist);

	retain(objhdr);

	*obj = object(objhdr);
error:
	return;
}

void Object_release(GC *gc, void *obj)
{
	if(Object_validate(gc, obj)) {
		ObjectHeader *objhdr = header(obj);

		release(objhdr);

		if(objhdr->ref_count == 0) {
			BlockHeader *blkhdr = GC_get_block_header(gc, objhdr);
			GC_unset_mark(blkhdr, objhdr);
			
			void **interior = obj;
			void **objend = obj + blkhdr->size - sizeof(ObjectHeader);

			while(interior < objend) {
				Object_release(gc, *interior);
				interior++;
			}
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
