#define CMM

#include "minunit.h"
#include <cmm/cmm.h>

GC *gc;

void test_setup()
{
	gc = __GC__;
}

typedef struct A {
	int F_int;
} A;

typedef struct B {
	char Fchar;
	A *F_A;
} B;

typedef struct C {
	int F_int_arr[10];
	char Fchar;
	B *F_B;
} C;

char *test_gc_init_size_map()
{
	mu_assert(SIZE_SZ == 9, "Invalid SIZE_SZ value.");

	mu_assert(gc->size_map[0] == 16, "Invalid min. object size");
	mu_assert(gc->size_map[SIZE_SZ - 1] == 4096, "Invalid max. object size");

	return NULL;
}

char *test_gc_init_obj_map()
{
	gc = __GC__;
	char *objmap = gc->obj_map;
/*
	mu_assert(MAX_OFFSET == 1024, "Invalid MAX_OFFSET value.");
	mu_assert(SIZE_SZ == 9, "Invalid SIZE_SZ value.");
*/
	mu_assert(objmap[0] == 0, "Invalid offset value.");
	mu_assert(objmap[MAX_OFFSET - 1] == 1, "Invalid offset value.");

	mu_assert(objmap[MAX_OFFSET] == 0, "Invalid offset value.");
	mu_assert(objmap[2 * MAX_OFFSET - 1] == 3, "Invalid offset value.");

	mu_assert(objmap[SIZE_SZ * MAX_OFFSET] == 0, "Invalid offset value.");
	//mu_assert(objmap[(SIZE_SZ + 1) * MAX_OFFSET - 1] == 3, "Invalid offset value.");

	/*
	int i = 0;

	for(i = 0; i < SIZE_SZ; i++) {
	*/
		/*
		mu_assert(__GC__->obj_map[i * MAX_OFFSET] == 0, "Invalid offset value.");

		printf("\n%d\n", __GC__->obj_map[i * MAX_OFFSET - 1]);
		*/
		
		//printf("\n%d\n", __GC__->size_map[i] / WORDSIZEBYTE);



		/*
		mu_assert(__GC__->obj_map[(i + 1) * MAX_OFFSET - 1] == __GC__->size_map[i] / (int)WORDSIZEBYTE - 1, "Invalid offset value.");
		*/
	//}

	return NULL;
}

char *test_cascade()
{
	Ref(C, c);

	New(C, c);
	New(B, c->F_B);
	New(A, c->F_B->F_A);
	
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
