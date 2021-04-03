#include "calculator.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

char * skip_spaces(char * str) {
	while (isspace(*str)) {
		str++;
	}
	return str;
}

char * skip_digit(char * str) {
	if (*str == '-')
		str++;
	while (isdigit(*str)) {
		str++;
	}
	return str;
}

int factorial (int n)
{
  return (n < 2) ? 1 : n * factorial (n - 1);
}

int32_t idle_calculator (char * expression_str, char * answer_str) {
	char operation = 0;
	int32_t first_number = 0,
            second_number = 0;
	_Bool operation_fac = 0;

	expression_str = skip_spaces(expression_str);
	
	if (*expression_str == '!') {
		operation_fac = true;
		expression_str++;
		expression_str = skip_spaces(expression_str);
	}
	
	if (isdigit(*expression_str) | *expression_str == '-') {
		first_number = atoi((const char *)expression_str);
		expression_str = skip_digit(expression_str);
		expression_str = skip_spaces(expression_str);
	}else {
		sprintf(answer_str, "ERROR\r\n");
		return 0;
	}

	if (operation_fac){
		if (first_number > 12) {
			sprintf(answer_str, "INF\r\n");
			return 0;
		}else if (first_number < 0){
			sprintf(answer_str, "ERROR\r\n");
			return 0;
		}
		return factorial(first_number);
	}
	
	operation = *expression_str;
	expression_str++;

	expression_str = skip_spaces(expression_str);

	if (isdigit(*expression_str) | *expression_str == '-') {
		second_number = atoi((const char *)expression_str);
	}else {
		sprintf(answer_str, "ERROR\r\n");
		return 0;
	}

	second_number = atoi((const char *)expression_str);

	int32_t answer = 0;

	switch(operation) {
		case '+': return first_number + second_number;
		case '-': return first_number - second_number;
		case '*': answer = first_number * second_number;
			if((answer <= 1000000) & (answer >= -1000000))
				return answer;
			else {
				sprintf(answer_str, "INF\r\n");
				return 0;
			}			
		case '/': 
			if (second_number != 0)
				return first_number / second_number;
			else {
				sprintf(answer_str, "INF\r\n");
				return 0;
			}
		default: sprintf(answer_str, "ERROR\r\n");
			return 0;
	}
	return 0;
}