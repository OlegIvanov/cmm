#define CMM

#include "minunit.h"
#include <cmm/cmm.h>

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

	mu_run_test(test_cascade);

	return NULL;
}

RUN_TESTS(all_tests);
