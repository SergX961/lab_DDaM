#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"


#define DELAY 128000000 / 16
void user_delay (uint32_t delay) {
	uint32_t tick = 0;
	while (tick < delay) 
		tick++;
}
void init_RCC (void) {
	RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);
	RCC_HSEConfig(RCC_HSE_ON);
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
	
}

#define RED_ON() (GPIO_SetBits(GPIOD, GPIO_Pin_14))
#define RED_OFF() (GPIO_ResetBits(GPIOD, GPIO_Pin_14))
#define GREEN_ON() (GPIO_SetBits(GPIOD, GPIO_Pin_12))
#define GREEN_OFF() (GPIO_ResetBits(GPIOD, GPIO_Pin_12))
#define YELLOW_ON() (GPIO_SetBits(GPIOD, GPIO_Pin_13))
#define YELLOW_OFF() (GPIO_ResetBits(GPIOD, GPIO_Pin_13))

int main (void) {
	init_RCC();
	init_GPIO();
	
	user_delay(DELAY);
	SystemCoreClockUpdate();
	
	while(1) {
		RED_ON();
		user_delay(DELAY * 5);
		YELLOW_ON();
		user_delay(DELAY * 3);
		RED_OFF();
		YELLOW_OFF();
		GREEN_ON();
		user_delay(DELAY * 5);
		for(uint8_t k = 0; k < 3; k++) {
			GREEN_OFF();
			user_delay(DELAY * 1);
			GREEN_ON();
			user_delay(DELAY * 1);
		}
		GREEN_OFF();
		YELLOW_ON();
		user_delay(DELAY * 3);
		YELLOW_OFF();
	}
}
