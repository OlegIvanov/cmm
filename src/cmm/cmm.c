#include <cmm/cmm.h>
#include <cmm/gc.h>

GC *__GC__;

static inline ObjectHeader *Object_get_header(void *obj)
{
	return (ObjectHeader *)obj - 1;
}

void Object_new(GC *gc, size_t size, void **obj)
{
	check(gc, "Argument 'gc' can't be NULL.");

	int sz = GC_get_size(gc, size + sizeof(ObjectHeader));
	List *freelist = gc->freelist[sz];

	if(List_count(freelist) == 0) {
		GC_allocate_block(gc, 1, sz);
	}

	ObjectDescriptor *obj_desc = calloc(1, sizeof(ObjectDescriptor));
	check_mem(obj_desc);

	ObjectHeader *obj_header = List_shift(freelist);
	obj_header->desc = obj_desc;
	
	obj_header->desc->ref_count++;

	*obj = obj_header + 1;

error:
	return;
}

inline void Object_release(void **obj)
{
	void *obj_ptr = *obj;

	if(obj_ptr == NULL) return;

	ObjectHeader *obj_header = Object_get_header(obj_ptr);
	obj_header->desc->ref_count--;

	if(obj_header->desc->ref_count == 0) {
		BlockHeader *block_header = GC_get_block_header(__GC__, (uintptr_t)obj_ptr);
		uint32_t object_size_bytes = block_header->size - sizeof(ObjectHeader);
		void *obj_addr = NULL;

		for(obj_addr = obj_ptr;
			obj_addr < obj_ptr + object_size_bytes;
			obj_addr += WORD_SIZE_BYTES) {

			void *ptr_candidate = *(void **)obj_addr;
			BlockHeader *block_header_candidate = GC_get_block_header(__GC__, (uintptr_t)ptr_candidate);

			if(block_header_candidate) {
				int16_t block_displ_words = GC_get_block_displ((uintptr_t)ptr_candidate) / WORD_SIZE_BYTES;

				if(block_header_candidate->map[block_displ_words - 1] == 0) {
					Object_release(&ptr_candidate);
				}
			}
		}

		int sz = GC_get_size(__GC__, block_header->size);
		List *freelist = __GC__->freelist[sz];
		List_push(freelist, obj_header);
		
		memset(obj_header, 0, block_header->size);
		free(obj_header->desc);
	}
}
