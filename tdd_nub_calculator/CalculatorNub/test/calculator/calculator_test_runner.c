#include "unity_fixture.h"

TEST_GROUP_RUNNER (calculator) {
	RUN_TEST_CASE (calculator, calc_factorial);
	RUN_TEST_CASE (calculator, calc_sum);
	RUN_TEST_CASE (calculator, calc_sub);
	RUN_TEST_CASE (calculator, calc_mult);
	RUN_TEST_CASE (calculator, calc_div);
}

