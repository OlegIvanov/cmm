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
	char Fchar;
	B *F_B;
} C;
/*
char *test_cascade()
{
	REF(B, b1);

	OBJECT(B, b1);
	FIELD(A, b1, F_A);

	REF(C, c1);

	OBJECT(C, c1);
	FIELD(B, c1, F_B);
	FIELD(A, c1->F_B, F_A);

	return NULL;
}

char *test_assign()
{
	REF(A, a1);
	OBJECT(A, a1);

	REF(B, b1);
	OBJECT(B, b1);

	ASSIGN(b1->F_A, a1);

	return NULL;
}
*/
char *all_tests() {
	//mu_suite_start();
/*
	mu_run_test(test_cascade);
	mu_run_test(test_assign);
*/
	return NULL;
}

RUN_TESTS(all_tests);
