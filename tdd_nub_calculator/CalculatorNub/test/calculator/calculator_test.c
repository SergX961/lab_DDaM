#include "unity_fixture.h"

#include "calculator_internal.h"
#include <math.h>
#include <string.h>

#define SIZE_ANSWER 100
char answer_buf_str [SIZE_ANSWER];

TEST_GROUP (calculator);

TEST_SETUP (calculator) {
	memset(answer_buf_str, 0, SIZE_ANSWER);
}

TEST_TEAR_DOWN (calculator) {

}

TEST (calculator, calc_factorial) {
	int32_t answer = idle_calculator ("!6", answer_buf_str);
	TEST_ASSERT_EQUAL_INT32(720, answer);
}

TEST (calculator, calc_sum) {
	int32_t answer = idle_calculator ("-100 + 25", answer_buf_str);
	TEST_ASSERT_EQUAL_INT32(-75, answer);
}
TEST (calculator, calc_sub) {
	int32_t answer = idle_calculator ("10 - -20", answer_buf_str);
	TEST_ASSERT_EQUAL_INT32(30, answer);
}
TEST (calculator, calc_mult) {
	int32_t answer = idle_calculator (" 10  * 30", answer_buf_str);
	TEST_ASSERT_EQUAL_INT32(300, answer);

	answer = idle_calculator (" 1000  * 1000", answer_buf_str);
	TEST_ASSERT_EQUAL_INT32(1000000, answer);
}
TEST (calculator, calc_div) {
	int32_t answer = idle_calculator ("-40 / -4", answer_buf_str);
	TEST_ASSERT_EQUAL_INT32(10, answer);
	
	answer = idle_calculator ("-40 / 0", answer_buf_str);
	TEST_ASSERT_EQUAL_INT32(0, answer);

	TEST_ASSERT_EQUAL_STRING_LEN("INF", answer_buf_str, strlen("INF"));
}


