#ifndef _arp_h
#define _arp_h

#include <cmm/list.h>
#include <cmm/gc.h>

typedef struct ARPool {
	List *pool;	
} ARPool;

#define ARP()				{ ARPool *__ARP__ __attribute__((cleanup(ARPool_release_callback))) = ARPool_create(__GC__);
#define Autorelease(Obj)	ARPool_autorelease(__GC__, Obj)

ARPool *ARPool_create(GC *gc);
void ARPool_release(GC *gc);
void ARPool_autorelease(GC *gc, void *obj);
void ARPool_release_callback(ARPool **arp);

#endif
