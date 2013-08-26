#include "minunit.h"
#include <cmm/cmm.h>

typedef struct SimpleStruct {
	int simple_int;
} SimpleStruct;

typedef struct ComplexStruct {
	char simple_char;
	SimpleStruct* ss;
} ComplexStruct;

typedef struct MostComplexStruct {
	char simple_char;
	ComplexStruct *cs;
} MostComplexStruct;

char *test()
{
	REF(ComplexStruct, cs1) = NEW_OBJ(ComplexStruct);
	NEW_FLD(SimpleStruct, cs1, ss);

	REF(MostComplexStruct, mcs1) = NEW_OBJ(MostComplexStruct);
	NEW_FLD(ComplexStruct, mcs1, cs);
	NEW_FLD(SimpleStruct, mcs1->cs, ss);

	return NULL;
}

char *all_tests() {
	mu_suite_start();

	mu_run_test(test);

	return NULL;
}

RUN_TESTS(all_tests);
