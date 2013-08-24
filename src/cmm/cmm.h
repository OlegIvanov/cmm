#ifndef _cmm_h
#define _cmm_h

#include <stdint.h>
#include <cmm/dbg.h>

typedef struct Header {
	uint32_t ref_count;	
} Header;

typedef struct ObjectWrapper {
	Header *header;
	void *object;
} ObjectWrapper;

#define NEW(Type) Object_new(sizeof(Type))

static inline void *Object_new(size_t object_size)
{
	ObjectWrapper *wrapper = calloc(1, sizeof(ObjectWrapper));
	check_mem(wrapper);
	
	wrapper->header = calloc(1, sizeof(Header));
	check_mem(wrapper->header);

	wrapper->object = calloc(1, sizeof(object_size));
	check_mem(wrapper->object);

	wrapper->header->ref_count = 1;

	return wrapper->object;
	
error:
	free(wrapper->header);
	free(wrapper->object);
	free(wrapper);
	return NULL;
}

static inline Header *Object_get_header(void *object_ref)
{
	return ((Header *)object_ref) - 1;
}

static inline void Object_release(void *object_ref)
{
	Header *header = Object_get_header(object_ref);

	header->ref_count--;

	if(header->ref_count == 0) {
		free(header);
		free(object_ref);
	}
}

#define OBJECT(Type, Ptr) Type * Ptr __attribute__((cleanup(Object_release)))

#endif
