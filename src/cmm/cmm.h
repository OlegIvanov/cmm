#ifndef _cmm_h
#define _cmm_h

#include <cmm/object.h>
#include <cmm/arp.h>

#ifdef CMM
__attribute__((__constructor__)) void cmm_init()
{ 
	__GC__ = GC_create();
}
#endif

#endif
