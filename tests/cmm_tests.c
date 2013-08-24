#include "minunit.h"
#include <cmm/cmm.h>

typedef struct SimpleStruct {
	int simple_field;
} SimpleStruct;

char *test()
{
	OBJECT(SimpleStruct, ss_ref) = NEW(SimpleStruct);

	return NULL;
}

char *all_tests() {
	mu_suite_start();

	mu_run_test(test);

	return NULL;
}

RUN_TESTS(all_tests);
