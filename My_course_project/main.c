#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "calculator.h"

void init_GPIO (void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// init pin uart
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);
}

USART_InitTypeDef uart4;
void init_UART (void) {
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	USART_StructInit(&uart4);
	uart4.USART_BaudRate = 115200;
	uart4.USART_WordLength = USART_WordLength_8b;
	uart4.USART_StopBits = USART_StopBits_1;
	uart4.USART_Parity = USART_Parity_No;

	USART_Init(UART4, &uart4);
}

#define MAX_RX_COUNT 30
uint8_t rx_uart_buffer [MAX_RX_COUNT + 1] = {0};
_Bool rx_allow = true,
      command_recieve = false;
 
void UART4_IRQHandler (void) {
	static uint8_t symbols_count = 0;
	uint8_t rx_symbol = USART_ReceiveData(UART4);
	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
	
	if (!rx_allow)
		return;
	
	if ((rx_symbol != '\r') & (symbols_count < MAX_RX_COUNT)) {
	rx_uart_buffer[symbols_count] = rx_symbol;
	symbols_count++;
	} else {
		rx_allow = false;
		command_recieve = true;
		symbols_count = 0;
	}
	
}
void transmit_str (char * str) {
	if (str == 0)
		return;
		
	uint16_t len = strlen((const char *)str);
	for (uint16_t k = 0; k < len; k++) {
		USART_ClearFlag(UART4, USART_FLAG_TC);
		USART_SendData(UART4, str[k]);
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) != SET);
	}
	USART_ClearFlag(UART4, USART_FLAG_TC);
}

void return_error (void) {
	transmit_str("Error occured\r\n");
}

#define SIZE_ANSWER 40
void parse_command (void) {
	uint8_t * command_str = rx_uart_buffer;
	char answer_str[SIZE_ANSWER] = {0};
	int32_t calc_answer = idle_calculator((char *)command_str, answer_str);
	
	if (*answer_str == 0) {
		transmit_str(answer_str);
	}else {
		sprintf(answer_str, "answer = %d", calc_answer);
		transmit_str(answer_str);
	}
	
	return;
}

int main (void) {
	init_GPIO();
	init_UART();
	USART_Cmd(UART4, ENABLE);
	NVIC_EnableIRQ(UART4_IRQn);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	while(1) {
		if (command_recieve) {
			parse_command();
			memset(rx_uart_buffer, 0, MAX_RX_COUNT);
			command_recieve = false;
			rx_allow = true;
		}
	}
}
