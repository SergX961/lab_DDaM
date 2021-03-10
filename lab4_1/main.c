#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "misc.h"

#define SECOND 1000
#define RED_TOGGLE()  (GPIO_ToggleBits(GPIOD, GPIO_Pin_14))
#define BLUE_TOGGLE() (GPIO_ToggleBits(GPIOD, GPIO_Pin_15))
#define RED_ON() 			(GPIO_SetBits(GPIOD, GPIO_Pin_14))
#define RED_OFF() 		(GPIO_ResetBits(GPIOD, GPIO_Pin_14))
#define GREEN_ON() 		(GPIO_SetBits(GPIOD, GPIO_Pin_12))
#define GREEN_OFF()	  (GPIO_ResetBits(GPIOD, GPIO_Pin_12))
#define YELLOW_ON() 	(GPIO_SetBits(GPIOD, GPIO_Pin_13))
#define YELLOW_OFF()  (GPIO_ResetBits(GPIOD, GPIO_Pin_13))

#define STATE_BUTTON() (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
uint8_t traffic_light_state = 0;
uint16_t delay_count = 0,
				 button_delay = 0;

_Bool button_delay_end = 1;
void SysTick_Handler(void) {
	if (delay_count > 0)
		delay_count--;
	
	if(button_delay > 0)
		button_delay--;
	else if (button_delay_end == 0){
		button_delay_end = 1;
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
			RED_TOGGLE();
			GREEN_ON();
		}
		else
			GREEN_OFF();	
	}
}

void delay_on_tim (uint16_t ms) {
	delay_count = ms;
	while(delay_count);
}

void init_GPIO (void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
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
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

void EXTI0_IRQHandler (void) {
	EXTI_ClearITPendingBit(EXTI_Line0);
	button_delay = 50;
	button_delay_end = 0;
}
int main (void) {
	SysTick_Config(SystemCoreClock/1000);
	init_GPIO();
	
	while(1) {
		
	}
}
