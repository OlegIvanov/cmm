#include <cmm/arp.h>
#include <cmm/gc.h>

ARPool *ARPool_create(GC *gc)
{
	ARPool *arp = calloc(1, sizeof(ARPool));
	check_mem(arp);

	List_push(gc->arp_stack, arp);

	return arp;
error:
	return NULL;	
}

void ARPool_release(GC *gc, ARPool *arp)
{	
}
