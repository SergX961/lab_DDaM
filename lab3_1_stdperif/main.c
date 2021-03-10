#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define SECOND 1000
#define PRESSED 1
#define DELAY 128000000 / 16
#define RED_ON() 			(GPIO_SetBits(GPIOD, GPIO_Pin_14))
#define RED_OFF() 		(GPIO_ResetBits(GPIOD, GPIO_Pin_14))
#define GREEN_ON() 		(GPIO_SetBits(GPIOD, GPIO_Pin_12))
#define GREEN_OFF()	  (GPIO_ResetBits(GPIOD, GPIO_Pin_12))
#define YELLOW_ON() 	(GPIO_SetBits(GPIOD, GPIO_Pin_13))
#define YELLOW_OFF()  (GPIO_ResetBits(GPIOD, GPIO_Pin_13))

#define STATE_BUTTON() (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))

uint16_t delay_count = 0;

void SysTick_Handler(void) {
	if (delay_count > 0)
		delay_count--;
}

void delay_on_tim (uint16_t ms) {
	delay_count = ms;
	while(delay_count);
}

void init_RCC (void) {
	RCC_DeInit();
	RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);
	RCC_HSEConfig(RCC_HSE_ON);
	RCC_PLLCmd(1);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void init_GPIO (void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
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
}

void traffic_light (void) {
	RED_ON();
	delay_on_tim(SECOND * 5);
	YELLOW_ON();
	delay_on_tim(SECOND * 3);
	RED_OFF();
	YELLOW_OFF();
	GREEN_ON();
	delay_on_tim(SECOND * 5);
	for(uint8_t k = 0; k < 3; k++) {
		GREEN_OFF();
		delay_on_tim(SECOND);
		GREEN_ON();
		delay_on_tim(SECOND);
	}
	GREEN_OFF();
	YELLOW_ON();
	delay_on_tim(SECOND * 3);;
	YELLOW_OFF();
}

int main (void) {
	SysTick_Config(SystemCoreClock/1000);
	init_RCC();
	init_GPIO();

	while(1) {
		traffic_light();
	}
}
