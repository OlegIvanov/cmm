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

#define NEW_OBJ(Type) Object_new(sizeof(Type))

#define NEW_FLD(Type, ParentObject, ParentField) Object_new_field(sizeof(Type), ParentObject, &(ParentField))

#define REF(Type, Identifier) Type * Identifier __attribute__((cleanup(Object_release)))

#define ASS(Left, Right) Object_retain(&(Left), &(Right))

#define ONE 1U

#define BIT_SIZE 32

#define ALIGNMENT 8

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

static inline void Object_new_field(size_t object_size, void *parent_object, void **parent_field)
{
	check(parent_object, "Parent object can't be NULL.");

	Header *parent_header = Object_get_header(parent_object);

	uint32_t set_bit = ((void *)parent_field - parent_object) / ALIGNMENT;
	parent_header->descriptor.ref_map |= ONE << set_bit;
	
	*parent_field = Object_new(object_size);

error:
	return;
}

static inline void Object_release(void **object)
{
	if(*object == NULL) return;

	Header *header = Object_get_header(*object);

	int i = 0;
	uint32_t bit_value = 0;
	uint32_t ref_map = header->descriptor.ref_map;

	for(i = 0; i < BIT_SIZE; i++) {
		bit_value = ref_map & ONE;

		if(bit_value) Object_release(*object + i * ALIGNMENT);

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
