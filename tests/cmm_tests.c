#include "minunit.h"
#include <cmm/cmm.h>

char *test()
{
	return NULL;
}

char *all_tests() {
	mu_suite_start();

	mu_run_test(test);

	return NULL;
}

RUN_TESTS(all_tests);
