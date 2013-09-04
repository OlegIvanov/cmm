#include <cmm/arp.h>
#include <cmm/gc.h>
#include <cmm/object.h>

ARPool *ARPool_create(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	ARPool *arp = calloc(1, sizeof(ARPool));
	check_mem(arp);

	arp->pool = List_create();

	List_push(gc->arp_stack, arp);

	return arp;
error:
	free(arp);
	return NULL;
}

void ARPool_destroy(ARPool *arp)
{
}

void ARPool_release(GC *gc/*, ARPool *arp*/)
{
	check(gc, "Argument 'gc' can't be NULL.");

	ARPool *arp = List_pop(gc->arp_stack);
	check(arp, "There is no pool.");

	LIST_FOREACH(arp->pool, first, next, cur) {
		Object_release((void **)&cur->value);
	}

error:
	return;
}

void ARPool_autorelease(GC *gc, void *obj)
{
	check(gc, "Argument 'gc' can't be NULL.");
	check(obj, "Can't add to pool NULL object.");

	ARPool *arp = List_last(gc->arp_stack);
	check(arp, "There is no pool.");

	List_push(arp->pool, obj);

error:
	return;
}
