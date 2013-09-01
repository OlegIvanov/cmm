#include <cmm/cmm.h>
#include <cmm/gc.h>

GC *__GC__;

void Object_new(GC *gc, size_t size, void **obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	int sz = GC_get_size(gc, size + sizeof(ObjectHeader));
	List *freelist = gc->freelist[sz];

	if(List_count(freelist) == 0) {
		GC_allocate_block(gc, 1, sz);
	}

	ObjectDescriptor *obj_desc = calloc(1, sizeof(ObjectDescriptor));
	check_mem(obj_desc);

	ObjectHeader *obj_header = List_shift(freelist);
	obj_header->obj_desc = obj_desc;

	*obj = obj_header + 1;

error:
	return;	
}

void Object_release(void **obj)
{
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
