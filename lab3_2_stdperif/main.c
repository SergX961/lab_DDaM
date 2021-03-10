#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

#define SECOND 1000
#define PRESSED 1
#define DELAY 128000000 / 16
#define DELAY_LED_BLUE 500
#define DELAY_LED_RED 1300

#define RED_TOGGLE()  (GPIO_ToggleBits(GPIOD, GPIO_Pin_14))
#define BLUE_TOGGLE() (GPIO_ToggleBits(GPIOD, GPIO_Pin_15))
#define RED_ON() 			(GPIO_SetBits(GPIOD, GPIO_Pin_14))
#define RED_OFF() 		(GPIO_ResetBits(GPIOD, GPIO_Pin_14))
#define GREEN_ON() 		(GPIO_SetBits(GPIOD, GPIO_Pin_12))
#define GREEN_OFF()	  (GPIO_ResetBits(GPIOD, GPIO_Pin_12))
#define YELLOW_ON() 	(GPIO_SetBits(GPIOD, GPIO_Pin_13))
#define YELLOW_OFF()  (GPIO_ResetBits(GPIOD, GPIO_Pin_13))

#define STATE_BUTTON() (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))

uint16_t delay_count = 0,
         delay_led_red = 0,
				 delay_led_blue = 0;

void SysTick_Handler(void) {
	if (delay_count > 0)
		delay_count--;
	
	if (delay_led_blue > 0) {
		delay_led_blue--;
		if (delay_led_blue == 0) {
			delay_led_blue = DELAY_LED_BLUE;
			RED_TOGGLE();
		}
	}
	
	if (delay_led_red > 0) {
		delay_led_red--; 
		if (delay_led_red == 0) {
			delay_led_red = DELAY_LED_RED;
			BLUE_TOGGLE();
		}
	}
}

void delay_on_tim (uint16_t ms) {
	delay_count = ms;
	while(delay_count);
}


void init_GPIO (void) {
	GPIO_InitTypeDef GPIO_InitStructure;
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
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
}

void init_TIM(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_OCInitTypeDef timerPWM;
	TIM_TimeBaseInitTypeDef base_timer;
  
	TIM_TimeBaseStructInit(&base_timer);
  base_timer.TIM_Prescaler = 0;
  base_timer.TIM_Period = 83;
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

void leds_on (void) {
	delay_led_blue = DELAY_LED_BLUE;
	delay_led_red = DELAY_LED_RED;
}

int main (void) {
	SysTick_Config(SystemCoreClock/1000);
	init_GPIO();
	init_TIM();
	
	leds_on();
	TIM_Cmd(TIM4, ENABLE);
 static	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	while(1) {
		
	}
}
