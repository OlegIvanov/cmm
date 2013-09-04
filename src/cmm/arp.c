#include <cmm/arp.h>
#include <cmm/gc.h>

ARPool *GC_create_arp(GC *gc)
{
	ARPool *arp = calloc(1, sizeof(ARPool));
	check_mem(arp);

	List_push(gc->arp_stack, arp);

	return arp;
error:
	return NULL;	
}

void GC_release_arp(GC *gc, ARPool *arp)
{	
}
