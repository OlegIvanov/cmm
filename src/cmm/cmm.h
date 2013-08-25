#ifndef _cmm_h
#define _cmm_h

#include <stdint.h>
#include <cmm/dbg.h>

typedef struct Header {
	uint32_t ref_count;	
} Header;

#define NEW(Type) Object_new(sizeof(Type))

#define REF(Type, Identifier) Type * Identifier __attribute__((cleanup(Object_release)))

#define ASS(Left, Right) Object_retain(&(Left), &(Right))

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

static inline Header *Object_get_header(void *object)
{
	return ((Header *)object) - 1;
}

static inline void Object_release(void **object)
{
	if(*object == NULL) return;

	Header *header = Object_get_header(*object);

	header->ref_count--;

	if(header->ref_count == 0) {
		free(header);
	}
}

static inline void Object_retain(void **object_left, void **object_right)
{
	if(*object_right != NULL) {
		Header *header = Object_get_header(*object_right);
		header->ref_count++;
	}

	Object_release(object_left);

	*object_left = *object_right;
}

#endif
