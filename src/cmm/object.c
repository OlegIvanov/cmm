/*
#include <cmm/object.h>
#include <cmm/gc.h>

void Object_new(GC *gc, size_t size, void **obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	int sz = GC_get_size(gc, size);
	List *freelist = gc->freelist[sz];

	if(List_count(freelist) == 0) {
		GC_allocate_block(gc, 1, sz);
	}

	*obj = List_shift(freelist);
error:
	return;	
}
*/
