#ifndef _cmm_h
#define _cmm_h

#include <stdint.h>
#include <cmm/dbg.h>

typedef struct Header {
	uint32_t ref_count;	
} Header;

#define NEW(Type) Object_new(sizeof(Type))

static inline void *Object_new(size_t object_size)
{
	Header *header = calloc(1, sizeof(Header) + object_size);
	check_mem(header);

	header->ref_count = 1;

	return ++header;

error:
	free(header);
	return NULL;
}

static inline Header *Object_get_header(void *object_ref)
{
	return ((Header *)object_ref) - 1;
}

static inline void Object_release(void **object_ref)
{
	Header *header = Object_get_header(*object_ref);

	header->ref_count--;

	if(header->ref_count == 0) {
		free(header);
	}
}

#define OBJECT(Type, Ptr) Type * Ptr __attribute__((cleanup(Object_release)))

#endif
