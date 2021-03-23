#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_usart.h"
#include "misc.h"


#define SECOND 1000
#define PRESSED 1
#define DELAY 128000000 / 16
#define DELAY_LED_BLUE 250
#define DELAY_LED_RED 650

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
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
			traffic_light_state = 0;
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
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;  
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_Init(GPIOA, & GPIO_InitStructure);
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
uint8_t dataBuffer[DMA_BUFFER_SIZE] = "tx_str_hello";

void init_UART (void) {
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	DMA_StructInit(&dma);
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(UART4->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)&dataBuffer[0];
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma.DMA_BufferSize = DMA_BUFFER_SIZE;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Channel = DMA_Channel_4;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
  dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
  dma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream2, &dma);
	
	USART_StructInit(&uart4);
	uart4.USART_BaudRate = 115200;
	uart4.USART_WordLength = USART_WordLength_8b;
	uart4.USART_StopBits = USART_StopBits_1;
	uart4.USART_Parity = USART_Parity_No;
//	uart4.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;

	USART_Init(UART4, &uart4);
}
static uint8_t k = 0,
							 n = 0;
void UART4_IRQHandler (void) {
	n++;
	if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET) {
		USART_ReceiveData(UART4);
//		USART_ClearITPendingBit(UART4, USART_IT_IDLE);
//		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
	//	dataBuffer[k] = USART_ReceiveData(UART4);
		DMA_Cmd(DMA1_Stream2, DISABLE);
		DMA_Init(DMA1_Stream2, &dma);
		
		DMA_Cmd(DMA1_Stream2, ENABLE);
		k++;
	}
//	static uint8_t symbols_count = 0;
//	uint8_t symbol = USART_ReceiveData(UART4);
	
	
}

int main (void) {
	SysTick_Config(SystemCoreClock/1000);
	init_GPIO();
	init_UART();
	NVIC_EnableIRQ(UART4_IRQn);
	USART_Cmd(UART4, ENABLE);
	DMA_Cmd(DMA1_Stream2, ENABLE);//?
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	
	while(1) {
	}
}
