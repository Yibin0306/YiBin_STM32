#include "stm32f10x.h"
#include "delay.h"
uint32_t BlinInterval = 1000;

void App_OnBoardLED(void);
void App_USART1_Init(void);
void USART1_IRQHandler(void); // 中断响应函数

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	App_OnBoardLED();
	App_USART1_Init();
	
	while(1)
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); // 亮灯
		Delay(BlinInterval);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); // 灭灯
		Delay(BlinInterval);
	}
}

void App_OnBoardLED(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void App_USART1_Init(void)
{
	
	// 1.初始化IO引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	// PA9 AF_PP
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	// PA10 IPU
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 2.初始化USART1
	// 开启USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	// 初始化USART1
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	// 闭合开关
	USART_Cmd(USART1, ENABLE);
	
	// 3.配置中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// 4.配置NVIC模块
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void USART1_IRQHandler(void) // 中断响应函数
{
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET){
		uint8_t byte = USART_ReceiveData(USART1);
		if(byte == '0') BlinInterval = 1000; // 慢
		if(byte == '1') BlinInterval = 200;  // 中
		if(byte == '2') BlinInterval = 50;   // 快
	}
}
