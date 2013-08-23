#ifndef _cmm_h
#define _cmm_h

#include <stdint.h>
#include <cmm/dbg.h>

typedef struct Header {
	uint32_t ref_count;	
} Header;

typedef struct Object {
	Header header;
	void *data;
} Object;

#define NEW(T) Object_new(sizeof(T))

static inline void *Object_new(size_t size)
{
	Object obj = calloc(1, sizeof(Object));
	check_mem(obj);
	
	obj->data = calloc(1, sizeof(size));
	check_mem(obj->data);

	return obj->data;
	
error:
	free(obj);
	return NULL;
}

#endif
