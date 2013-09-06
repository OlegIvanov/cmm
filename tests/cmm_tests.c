#include "minunit.h"
#include <cmm/arp.h>

GC *gc;

#define ref(obj) RetainCount(obj)

void test_setup()
{
	gc = __GC__;
}

typedef struct A {
	char F_char;
} A;

typedef struct B {
	char F_char;
	A *F_A;
} B;

typedef struct C {
	char F_char;
	B *F_B;
} C;

char *test_gc_init_size_map()
{
	mu_assert(SIZE_SZ == 9, "Invalid SIZE_SZ value.");

	mu_assert(gc->size_map[0] == 16, "Invalid min. object size.");
	mu_assert(gc->size_map[SIZE_SZ - 1] == 4096, "Invalid max. object size.");

	return NULL;
}

char *test_gc_init_obj_map()
{	
	mu_assert(MAX_BLOCK_OFFSET_WORDS_SZ == 1024, "Invalid max_block_offset_words_sz value.");

	int i = 0;

	for(i = 0; i < SIZE_SZ; i++) {
		uint16_t offset = *(gc->obj_map + i * MAX_BLOCK_OFFSET_WORDS_SZ);
		mu_assert(offset == 0, "Invalid offset value.");

		offset = *(gc->obj_map + i * MAX_BLOCK_OFFSET_WORDS_SZ + MAX_BLOCK_OFFSET_WORDS_SZ - 1);
		uint16_t obj_max_offset = gc->size_map[i] / WORD_SIZE_BYTES - 1;

		mu_assert(offset == obj_max_offset, "Invalid offset value.");
	}

	return NULL;
}

char *test_cascade()
{
	C *c = NULL;
	mu_assert(ref(c) == -1, "Invalid reference count.");

	New(C, c);
	mu_assert(ref(c) == 1, "Invalid reference count.");

	New(B, c->F_B);
	mu_assert(ref(c->F_B) == 1, "Invalid reference count.");

	New(A, c->F_B->F_A);
	mu_assert(ref(c->F_B->F_A) == 1, "Invalid reference count.");
	
	Release(c->F_B);
	mu_assert(ref(c->F_B) == -1, "Invalid reference count.");
	mu_assert(ref(c->F_B->F_A) == -1, "Invalid reference count.");

	return NULL;
}

char *test_arp()
{
	ARP()
		B *b = NULL;

		New(B, b);
		Autorelease(b);

		New(A, b->F_A);
	}

	return NULL;
}

char *test_copy()
{
	B *b = NULL;

	New(B, b);
	New(A, b->F_A);

	A *a = NULL;	

	Copy(a, b->F_A);
	Copy(a, NULL);

	return NULL;
}

char *all_tests() {
	mu_suite_start();

	test_setup();

	mu_run_test(test_gc_init_size_map);
	mu_run_test(test_gc_init_obj_map);
	mu_run_test(test_cascade);
	mu_run_test(test_arp);
	mu_run_test(test_copy);

	return NULL;
}

RUN_TESTS(all_tests);
