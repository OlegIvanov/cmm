#include <cmm/allocator.h>
#include <cmm/dbg.h>
#include <stdlib.h>

Allocator *Allocator_create()
{
	Allocator *allocator = calloc(1, sizeof(Allocator));
	check_mem(allocator);
	
	/*
	allocator->block_list = List_create();
	*/

	return allocator;
error:
	return NULL;
}

void *Allocator_allocate_block(Allocator *allocator, int n)
{
	void *block = NULL;

	int rc = posix_memalign(&block, BLOCK_SIZE, n * BLOCK_SIZE);
	check(rc == 0, "Allocating block error occured.");

	return block;
error:
	return NULL;
	/*
	List_push(allocator->block_list, block);

	return 0;
error:
	return -1;
	*/
}
