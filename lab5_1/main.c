#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


#define COLOR_GREEN 1
#define COLOR_RED 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4



#define SECOND 1000
#define PRESSED 1
#define DELAY 128000000 / 16
#define DELAY_LED_BLUE 250
#define DELAY_LED_RED 650

#define GPIO_PIN_GREEN GPIO_Pin_12
#define GPIO_PIN_YELLOW GPIO_Pin_13
#define GPIO_PIN_RED GPIO_Pin_14
#define GPIO_PIN_BLUE GPIO_Pin_15

#define GREEN_TOGGLE()  (GPIO_ToggleBits(GPIOD, GPIO_PIN_GREEN))
#define YELLOW_TOGGLE() (GPIO_ToggleBits(GPIOD, GPIO_PIN_YELLOW))
#define RED_TOGGLE()    (GPIO_ToggleBits(GPIOD, GPIO_PIN_RED))
#define BLUE_TOGGLE()   (GPIO_ToggleBits(GPIOD, GPIO_PIN_BLUE))
#define RED_ON() 			  (GPIO_SetBits(GPIOD, GPIO_PIN_RED))
#define RED_OFF() 	    (GPIO_ResetBits(GPIOD, GPIO_PIN_RED))
#define GREEN_ON() 		  (GPIO_SetBits(GPIOD, GPIO_PIN_GREEN))
#define GREEN_OFF()	    (GPIO_ResetBits(GPIOD, GPIO_PIN_GREEN))
#define YELLOW_ON() 	  (GPIO_SetBits(GPIOD, GPIO_PIN_YELLOW))
#define YELLOW_OFF()    (GPIO_ResetBits(GPIOD, GPIO_PIN_YELLOW))

#define STATE_BUTTON() (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
uint8_t traffic_light_state = 0;
uint16_t delay_count = 0,
				 button_delay = 0;

uint32_t max_blue_delay_count = 500,
         max_red_delay_count = 500,
				 max_yellow_delay_count = 500,
				 max_green_delay_count = 500;


uint32_t blue_delay_count = 500,
         red_delay_count = 500,
				 yellow_delay_count = 500,
				 green_delay_count = 500;

_Bool button_delay_end = 1;
void SysTick_Handler(void) {
	if (delay_count > 0)
		delay_count--;
	
	if (blue_delay_count > 0) {
		blue_delay_count--;
		if (blue_delay_count == 0) {
			blue_delay_count = max_blue_delay_count;
			BLUE_TOGGLE();
		}
	}
	
	if (blue_delay_count > 0) {
		red_delay_count--;
		if (red_delay_count == 0) {
			red_delay_count = max_red_delay_count;
			RED_TOGGLE();
		}
	}

	if (blue_delay_count > 0) {
		yellow_delay_count--;
		if (yellow_delay_count == 0) {
			yellow_delay_count = max_yellow_delay_count;
			YELLOW_TOGGLE();
		}
	}
	
	if (blue_delay_count > 0) {
		green_delay_count--;
		if (green_delay_count == 0) {
			green_delay_count = max_green_delay_count;
			GREEN_TOGGLE();
		}
	}
	
}

void delay_on_tim (uint16_t ms) {
	delay_count = ms;
	while(delay_count);
}


void init_GPIO (void) {
	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStruct;
//	EXTI_InitTypeDef EXTI_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_GREEN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_YELLOW;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RED;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BLUE;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, & GPIO_InitStructure);
//	
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
//	
//	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	EXTI_Init(&EXTI_InitStruct);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);
}

void init_TIM(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_OCInitTypeDef timerPWM;
	TIM_TimeBaseInitTypeDef base_timer;
  
	TIM_TimeBaseStructInit(&base_timer);
  base_timer.TIM_Prescaler = 0;
  base_timer.TIM_Period = 84;
	base_timer.TIM_CounterMode = TIM_CounterMode_Up;
	base_timer.TIM_ClockDivision = 0;
  TIM_TimeBaseInit(TIM4, &base_timer);
	
	TIM_OCStructInit(&timerPWM);
	timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
	timerPWM.TIM_OutputState = TIM_OutputState_Enable;
	timerPWM.TIM_Pulse = 42;
	timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &timerPWM);
}


void traffic_light_state_0 (void) {
	traffic_light_state = 1;
	GREEN_OFF();
	YELLOW_OFF();
	RED_ON();
	delay_on_tim(SECOND * 5);
}
void traffic_light_state_1 (void) {
	traffic_light_state = 2;
	GREEN_OFF();
	RED_OFF();
	YELLOW_ON();
	delay_on_tim(SECOND * 3);
}
void traffic_light_state_2 (void) {
	traffic_light_state = 3;
	RED_OFF();
	YELLOW_OFF();
	GREEN_ON();
	delay_on_tim(SECOND * 5);
}
void traffic_light_state_3 (void) {
	traffic_light_state = 4;
	RED_OFF();
	YELLOW_OFF();
	for(uint8_t k = 0; k < 3; k++) {
		if (traffic_light_state != 3)
			return;
		GREEN_OFF();
		delay_on_tim(SECOND);
		GREEN_ON();
		delay_on_tim(SECOND);
	}
}
void traffic_light_state_4 (void) {
	traffic_light_state = 0;
	RED_OFF();
	GREEN_OFF();
	YELLOW_ON();
	delay_on_tim(SECOND * 3);
}

void traffic_light (void) {
	switch (traffic_light_state){
		case 0: traffic_light_state_0();
			break;
		case 1: traffic_light_state_1();
			break;
		case 2: traffic_light_state_2();
			break;
		case 3: traffic_light_state_3();
			break;
		case 4: traffic_light_state_4();
			break;
	}
}

void EXTI0_IRQHandler (void) {
	EXTI_ClearITPendingBit(EXTI_Line0);
	button_delay = 50;
	button_delay_end = 0;
}

DMA_InitTypeDef dma;
USART_InitTypeDef uart4;
#define DMA_BUFFER_SIZE 20
uint8_t dataBuffer[DMA_BUFFER_SIZE] = {0};

void init_UART (void) {
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
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
void transmit_str (uint8_t * str) {
	if (str == 0)
		return;
	
//	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
	
	uint16_t len = strlen((const char *)str);
	for (uint16_t k = 0; k < len; k++) {
		USART_ClearFlag(UART4, USART_FLAG_TC);
		USART_SendData(UART4, str[k]);
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) != SET);
	}
	USART_ClearFlag(UART4, USART_FLAG_TC);
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
//	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
}

void return_error (void) {
	transmit_str("Error occured\n");
}

uint32_t conv_led_time_to_digit (const char * str) {
	if (isdigit(*str)) {
			 return atoi((const char *)str);
		} else {
			return 0;
		}
}

void set_led_time (uint8_t color, uint32_t time) {
	if (color == COLOR_GREEN) {
		max_green_delay_count = time;
		green_delay_count = 1;
	} else if (color == COLOR_RED) {
		max_red_delay_count = time;
		red_delay_count = 1;
	} else if (color == COLOR_YELLOW) {
		max_yellow_delay_count = time;
		yellow_delay_count = 1;
	} else if (color == COLOR_BLUE) {
		max_blue_delay_count = time;
		blue_delay_count = 1;
	}
}

void parse_command (void) {
	uint8_t * command_str = rx_uart_buffer;
	uint8_t command_len = strlen((const char *)command_str),
		      color = 0;
	uint32_t led_time = 0;
	
	if (!strncmp((const char *)command_str, "get_id", strlen("get_id"))){
		transmit_str("Device id: 1214\n");
	} else if (!strncmp((const char *)command_str, "set_time", strlen("set_time"))) {
		command_str += strlen("set_time") + 1;
		
		if(!strncmp((const char *)command_str, "green", strlen("green"))) {
			command_str += strlen("green") + 1;
			color = COLOR_GREEN;
			led_time = conv_led_time_to_digit ((const char *)command_str);
			if (led_time == 0) {
				return_error();
				return;
			}
			
		} else if(!strncmp((const char *)command_str, "red", strlen("red"))) {
			command_str += strlen("red") + 1;
			color = COLOR_RED;
			led_time = conv_led_time_to_digit ((const char *)command_str);
			if (led_time == 0) {
				return_error();
				return;
			}
			
		} else if(!strncmp((const char *)command_str, "yellow", strlen("yellow"))) {
			command_str += strlen("yellow") + 1;
			color = COLOR_YELLOW;
			led_time = conv_led_time_to_digit ((const char *)command_str);
			if (led_time == 0) {
				return_error();
				return;
			}
			
		} else if(!strncmp((const char *)command_str, "blue", strlen("blue"))) {
			command_str += strlen("blue") + 1;
			color = COLOR_BLUE;
			led_time = conv_led_time_to_digit ((const char *)command_str);
			if (led_time == 0) {
				return_error();
				return;
			}
			
		} else {
			return_error();
			return;
		}
		
		set_led_time(color,led_time);
	} else if (!strncmp((const char *)command_str, "get_state", strlen("get_state"))) {
		_Bool red_state = GPIO_ReadOutputDataBit(GPIOD, GPIO_PIN_RED),
	       	yellow_state = GPIO_ReadOutputDataBit(GPIOD, GPIO_PIN_YELLOW),
		     	green_state = GPIO_ReadOutputDataBit(GPIOD, GPIO_PIN_GREEN),
		     	blue_state = GPIO_ReadOutputDataBit(GPIOD, GPIO_PIN_BLUE),
		      button_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
		uint8_t state_str [100] = {0};
		sprintf((char *)state_str, "state leds:R%dY%dG%dB%d button:%d\n", red_state, yellow_state, green_state, blue_state,  button_state);
		transmit_str(state_str);
	} else {
		return_error();
		return;
	}
		
}


int main (void) {
	SysTick_Config(SystemCoreClock/1000);
	init_GPIO();
	init_UART();
	USART_Cmd(UART4, ENABLE);
	NVIC_EnableIRQ(UART4_IRQn);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	transmit_str("System started\n");
	
	while(1) {
		if (command_recieve) {
			parse_command();
			command_recieve = false;
			rx_allow = true;
		}
	}
}
