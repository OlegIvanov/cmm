#ifndef _cmm_h
#define _cmm_h

#include <stdint.h>
#include <cmm/dbg.h>

typedef struct Descriptor {
	uint32_t ref_map;
} Descriptor;

typedef struct Header {
	Descriptor descriptor;
	uint32_t ref_count;	
} Header;

#define OBJECT(Type, Ref) Object_new(sizeof(Type), &(Ref))

#define FIELD(Type, Object, Field) Object_new_field(sizeof(Type), Object, &(Object->Field))

#define REF(Type, Identifier) Type * Identifier __attribute__((cleanup(Object_release))) = NULL

#define ASS(Left, Right) Object_retain(&(Left), &(Right))

#define ONE 1U

#define BIT_SIZE 32

#define ALIGNMENT 8

static inline void Object_new(size_t object_size, void **object_ref)
{
	Header *header = calloc(1, sizeof(Header) + object_size);

	header->ref_count = 1;

	*object_ref = ++header;
}

static inline Header *Object_get_header(void *object)
{
	return ((Header *)object) - 1;
}

static inline void Object_new_field(size_t field_size, void *object, void **field)
{
	Header *header = Object_get_header(object);

	uint32_t bit = ((void *)field - object) / ALIGNMENT;
	header->descriptor.ref_map |= ONE << bit;
	
	Object_new(field_size, field);
}

static inline void Object_release(void **object)
{
	if(*object == NULL) return;

	Header *header = Object_get_header(*object);

	int i = 0;
	uint32_t bit = 0;
	uint32_t ref_map = header->descriptor.ref_map;

	for(i = 0; i < BIT_SIZE; i++) {
		bit = ref_map & ONE;

		if(bit) Object_release(*object + i * ALIGNMENT);

		ref_map = ref_map >> ONE;
	}

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
