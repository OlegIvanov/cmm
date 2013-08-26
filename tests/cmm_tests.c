#include "minunit.h"
#include <cmm/cmm.h>

typedef struct SimpleStruct {
	int simple_int;
} SimpleStruct;

typedef struct ComplexStruct {
	char simple_char;
	SimpleStruct *ss;
} ComplexStruct;

typedef struct MostComplexStruct {
	char simple_char;
	ComplexStruct *cs;
} MostComplexStruct;

char *test()
{
	REF(ComplexStruct, cs1);
	OBJECT(ComplexStruct, cs1);
	FIELD(SimpleStruct, cs1, ss);

	REF(MostComplexStruct, mcs1);
	OBJECT(MostComplexStruct, mcs1);
	FIELD(ComplexStruct, mcs1, cs);
	FIELD(SimpleStruct, mcs1->cs, ss);

	return NULL;
}

char *all_tests() {
	mu_suite_start();

	mu_run_test(test);

	return NULL;
}

RUN_TESTS(all_tests);
