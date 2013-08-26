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
	REF(ComplexStruct, cs) = NEW_OBJ(ComplexStruct);

	NEW_FLD(SimpleStruct, cs, cs->ss);

	return NULL;
}

char *all_tests() {
	mu_suite_start();

	mu_run_test(test);

	return NULL;
}

RUN_TESTS(all_tests);
