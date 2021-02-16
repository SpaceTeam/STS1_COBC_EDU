#include "stm32f30x_conf.h"

int main(void){
	// INIT structures
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	// Enable GPIO banks A and C
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	// Everything with GPIOA Pin 5 and Timer 2 is just for the blinking LED to see if the MC is running
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_1);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
  TIM_TimeBaseInitStructure.TIM_Prescaler=7999;                                      
  TIM_TimeBaseInitStructure.TIM_Period= 999;             
                                                        
  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
  TIM_OCInitStructure.TIM_Pulse=500;                         
  TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;            
  TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;    
    
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    
  TIM_Cmd(TIM2, ENABLE);
	// End of blinking LED stuff
	
	// USART -> use GPIO pins as alternate functions
	// USART2 connects MC to PC via ST-Link
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3; 
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;

	// Init GPIOA
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// Change pins, USART pins on GPIOC are 10 and 11 and init GPIOC
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Set alternate functions according to data sheet of STM32F334R8
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7); // 2 ... TX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7); // 3 ... RX
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_7); // 10 ... TX
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_7); // 11 ... RX

	// Enable USART peripheries on APB1 Bus
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// USART properties
  USART_InitStructure.USART_BaudRate=9600;                                        
  USART_InitStructure.USART_WordLength=USART_WordLength_8b;                       
  USART_InitStructure.USART_StopBits=USART_StopBits_1;                            
  USART_InitStructure.USART_Parity=USART_Parity_No;                               
  USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;   
  USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;                  
    
	// Init and enable
	USART_Init(USART2, &USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	// Init Interrupt channels
  NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_Init(&NVIC_InitStructure);
	
	
	while(1){
			
	}
}

// PC(USART2) sends to MC which sends to PI(USART3)
void USART2_IRQHandler()
{
    int recvd;
    
    recvd=USART_ReceiveData(USART2);

		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE)!=SET){};
    USART_SendData(USART3, recvd);
    
}

// MC receives from PI (USART3) and sends to PC(USART2)
void USART3_IRQHandler()
{
	int recvd;
	recvd=USART_ReceiveData(USART3);
	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)!=SET){};
  USART_SendData(USART2, recvd);
	
}
