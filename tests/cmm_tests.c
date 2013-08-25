#include "minunit.h"
#include <cmm/cmm.h>

typedef struct SimpleStruct {
	int simple_field;
} SimpleStruct;

char *test()
{
 	REF(SimpleStruct, ref1) = NEW(SimpleStruct);

	REF(SimpleStruct, ref2) = NULL;

	ASS(ref2, ref1);

	REF(SimpleStruct, ref3) = NULL;

	ASS(ref3, ref1);

	REF(SimpleStruct, arr[100]) = {NULL};

	ASS(arr[0], ref1);

	return NULL;
}
/*
char *test2()
{
	REF(SimpleStruct, ref1) = NEW(SimpleStruct);

	REF(SimpleStruct, arr[100]) = {NULL};

	arr[0] = NEW(SimpleStruct);
	arr[1] = NEW(SimpleStruct);
	arr[99] = NEW(SimpleStruct);

	return NULL;
}
*/
char *all_tests() {
	mu_suite_start();

	mu_run_test(test);

	return NULL;
}

RUN_TESTS(all_tests);
