#ifndef _arp_h
#define _arp_h

typedef struct ARPool {
	List *pool;	
} ARPool;

#define ARPC { ARPool *__ARP__ = GC_create_arp(__GC__);

#define ARPR GC_release_arp(__GC__, __ARP__); }

ARPool *GC_create_arp(GC *gc);

void GC_release_arp(GC *gc, ARPool *arp);

#endif
