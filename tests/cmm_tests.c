#define CMM

#include "minunit.h"
#include <cmm/cmm.h>

GC *gc;

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
	mu_assert(MAX_OFFSET == 1024, "Invalid MAX_OFFSET value.");
	
	int i = 0;

	for(i = 0; i < SIZE_SZ - 1; i++) {
		mu_assert(gc->obj_map[i][0] == 0, "Invalid offset value.");

		uint16_t obj_max_offset = gc->size_map[i] / WORD_SIZE_BYTES - 1;

		mu_assert(gc->obj_map[i][MAX_OFFSET - 1] == obj_max_offset, "Invalid offset value.");
	}

	return NULL;
}

char *test_cascade()
{
	Ref(B, b);

	New(B, b);
	New(A, b->F_A);

	printf("\nb:\t\t%p\n", b);
	printf("\n&b->F_A:\t%p\n", &b->F_A);
	printf("\nb->F_A:\t\t%p\n", b->F_A);
	printf("----------------------------");
	
	return NULL;
}

char *all_tests() {
	mu_suite_start();

	test_setup();

	mu_run_test(test_gc_init_size_map);
	mu_run_test(test_gc_init_obj_map);
	mu_run_test(test_cascade);

	return NULL;
}

RUN_TESTS(all_tests);
