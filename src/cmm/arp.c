#include <cmm/arp.h>
#include <cmm/gc.h>
#include <cmm/object.h>

ARPool *ARPool_create(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	ARPool *arp = NULL;
	New(ARPool, arp);

	arp->pool = List_create();
	List_push(gc->arp_stack, arp);

	return arp;
error:
	return NULL;
}

void ARPool_release(GC *gc)
{
	check(gc, "Argument 'gc' can't be NULL.");

	ARPool *arp = List_pop(gc->arp_stack);
	check(arp, "There is no pool.");

	LIST_FOREACH(arp->pool, first, next, cur) {
		Release(cur->value);
	}

	GC_sweep(gc);

	List_destroy(arp->pool);
	Release(arp);
error:
	return;
}

void ARPool_autorelease(GC *gc, void *obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	ARPool *arp = List_last(gc->arp_stack);
	check(arp, "There is no pool.");

	List_push(arp->pool, obj);
error:
	return;
}

void ARPool_release_callback(ARPool **arp)
{
	*arp = NULL;
	ARPool_release(__GC__);
}
