#include "minunit.h"
#include <cmm/cmm.h>

typedef struct SimpleStruct {
	int simple_int;
} SimpleStruct;

typedef struct ComplexStruct {
	char simple_char;
	SimpleStruct* ss;
} ComplexStruct;

char *test()
{
	REF(ComplexStruct, cs1) = NEW_OBJ(ComplexStruct);

	NEW_FLD(SimpleStruct, cs1, ss);

	//REF(ComplexStruct, cs2) = NEW_OBJ(ComplexStruct);

	//ASS(cs2->ss, cs1->ss);

	return NULL;
}

char *all_tests() {
	mu_suite_start();

	mu_run_test(test);

	return NULL;
}

RUN_TESTS(all_tests);
