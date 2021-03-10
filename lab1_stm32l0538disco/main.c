#include "stm32l053xx.h"

#define DELAY 128000000 / 320
void user_delay (uint32_t delay) {
	uint32_t tick = 0;
	while (tick < delay) 
		tick++;
}

void RCC_Init (void) {
	RCC -> CFGR |= RCC_CFGR_PLLDIV2 | RCC_CFGR_PLLMUL8 | RCC_CFGR_PLLSRC_HSE;
	RCC -> CR |= RCC_CR_HSEON | RCC_CR_PLLON;
	FLASH -> ACR |= FLASH_ACR_LATENCY;
	RCC -> CFGR |= RCC_CFGR_SW_PLL;
}

void GPIO_Init (void) {
	RCC -> IOPENR |= RCC_IOPENR_IOPAEN;
	GPIOA -> MODER &= (~GPIO_MODER_MODE5_Msk) | GPIO_MODER_MODE5_0;
}

int main (void) {
	RCC_Init();
	GPIO_Init();
	
	user_delay(DELAY);
	SystemCoreClockUpdate();
	
	while(1) {
		GPIOA -> BSRR = GPIO_BSRR_BS_5;
		user_delay(DELAY);
		GPIOA -> BSRR = GPIO_BSRR_BR_5;
		user_delay(DELAY);
	}
}
