#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

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
#define BLUE_ON() 	    (GPIO_SetBits(GPIOD, GPIO_PIN_BLUE))
#define BLUE_OFF()      (GPIO_ResetBits(GPIOD, GPIO_PIN_BLUE))

uint16_t delay_count = 0;

void SysTick_Handler(void) {
	if (delay_count > 0)
		delay_count--;
}

void delay_on_tim (uint16_t ms) {
	delay_count = ms;
	while(delay_count);
}

void reset_delay (void) {
	delay_count = 0;
}

void init_GPIO (void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
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
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, & GPIO_InitStructure);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
}

char mode_1_state = 1,
		 mode_2_state = 1,
		 mode_3_state = 1,
		 mode_4_state = 1,
		 num_mode = 1;

void EXTI0_IRQHandler (void) {
	EXTI_ClearITPendingBit(EXTI_Line0);
	if (num_mode != 4)
		num_mode++;
	else
		num_mode = 1;
	
	reset_delay();
	GREEN_OFF();
	YELLOW_OFF();
	BLUE_OFF();
	RED_OFF();
}

void mode_1 (void) {
	if (mode_1_state == 1) {
		RED_ON();
		GREEN_ON();
		YELLOW_OFF();
		BLUE_OFF();
		mode_1_state = 2;
		delay_on_tim(500);
	}
	else if (mode_1_state == 2) {
		RED_OFF();
		GREEN_OFF();
		YELLOW_ON();
		BLUE_ON();
		mode_1_state = 1;
		delay_on_tim(500);
	}
}

void mode_2 (void) {
	if (mode_2_state == 1) {
		RED_ON();
		mode_2_state = 2;
		delay_on_tim(500);
	
	}
	else if (mode_2_state == 2) {
		RED_OFF();
		mode_2_state = 1;
		delay_on_tim(500);
	}
}

void mode_3 (void) {
	if (mode_3_state == 1) {
		BLUE_ON();
		mode_3_state = 2;
		delay_on_tim(500);
	
	}
	else if (mode_3_state == 2) {
		BLUE_OFF();
		mode_3_state = 1;
		delay_on_tim(500);
	}
}

void mode_4 (void) {
	if (mode_4_state == 1) {
		YELLOW_OFF();
		RED_ON();
		mode_4_state = 2;
		delay_on_tim(250);
	}
	else 	if (mode_4_state == 2) {
		RED_OFF();
		GREEN_ON();
		mode_4_state = 3;
		delay_on_tim(250);
	}
	else 	if (mode_4_state == 3) {
		GREEN_OFF();
		BLUE_ON();
		mode_4_state = 4;
		delay_on_tim(250);
	}
	else 	if (mode_4_state == 4) {
		BLUE_OFF();
		YELLOW_ON();
		mode_4_state = 1;
		delay_on_tim(250);
	}
}

int main (void) {
	SysTick_Config(SystemCoreClock/1000);
	init_GPIO();
	
	while(1) {
		if (num_mode == 1)
			mode_1();
		else if(num_mode == 2)
			mode_2();
		else if(num_mode == 3)
			mode_3();
		else if(num_mode == 4)
			mode_4();
	}
}
