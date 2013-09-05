#include <cmm/object.h>
#include <cmm/gc.h>

static inline ObjectHeader *Object_get_header(void *obj)
{
	return (ObjectHeader *)obj - 1;
}

static inline void Object_retain(void *obj)
{
	ObjectHeader *obj_header = Object_get_header(obj);
	obj_header->ref_count++;
}

inline int Object_validate_ptr(GC *gc, void *ptr)
{
	BlockHeader *block_header = GC_get_block_header(gc, (uintptr_t)ptr);

	if(block_header) {
		int16_t block_displ_words = GC_get_block((uintptr_t)ptr) / WORD_SIZE_BYTES;
		
		if(block_header->map[block_displ_words - 1] == 0) {
			return 1;
		}
	}

	return 0;
}

void Object_new(GC *gc, size_t type_size, void **obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	if(Object_validate_ptr(gc, *obj)) {
		Object_release(gc, *obj);
	}

	int size_index = GC_get_size(gc, type_size + sizeof(ObjectHeader));
	List *freelist = gc->freelist[size_index];

	if(List_count(freelist) == 0) {
		GC_allocate_block(gc, 1, size_index);
	}

	ObjectHeader *obj_header = List_shift(freelist);
	memset(obj_header, 0, gc->size_map[size_index]);
	Object_retain((void *)(obj_header + 1));

	*obj = obj_header + 1;
error:
	return;
}

static void Object_release_childs(GC *gc, void *obj, BlockHeader *block_header)
{
	uint32_t object_size_bytes = block_header->size - sizeof(ObjectHeader);
	void **ptr = NULL;

	for(ptr = (void **)obj; 
		ptr < (void **)obj + object_size_bytes; 
		ptr++) {
		
		BlockHeader *blkhdr = GC_get_block_header(gc, (uintptr_t)ptr);
		if(blkhdr) {
			int16_t block_displ_words = GC_get_block((uintptr_t)ptr) / WORD_SIZE_BYTES;
			if(blkhdr->map[block_displ_words - 1] == 0) {
				Object_release(gc, ptr);
			}
		}
	}
}

void Object_release(GC *gc, void *obj)
{
	if(obj == NULL) return;

	ObjectHeader *obj_header = Object_get_header(obj);
	obj_header->ref_count--;

	if(obj_header->ref_count == 0) {
		BlockHeader *block_header = GC_get_block_header(gc, (uintptr_t)obj_header);
		Object_release_childs(gc, obj, block_header);
		List_push(gc->freelist[block_header->size_index], obj_header);	
	}
}

void Object_copy(GC *gc, void **lobj, void *robj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	if(robj) {
		check(Object_validate_ptr(gc, robj), "Invalid 'robj' pointer.");
	}

	if(Object_validate_ptr(gc, *lobj)) {
		Object_release(gc, *lobj);
	}

	if(robj) {
		Object_retain(robj);
	}

	*lobj = robj;
error:
	return;
}
