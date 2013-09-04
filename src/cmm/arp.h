#ifndef _arp_h
#define _arp_h

#include <cmm/list.h>
#include <cmm/gc.h>

typedef struct ARPool {
	List *pool;	
} ARPool;

#define ARPC { ARPool *__ARP__ = ARPool_create(__GC__);

#define ARPR ARPool_release(__GC__, __ARP__); }

ARPool *ARPool_create(GC *gc);

void ARPool_release(GC *gc, ARPool *arp);

#endif
