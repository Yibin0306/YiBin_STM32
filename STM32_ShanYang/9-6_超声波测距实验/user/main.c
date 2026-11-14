#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"

void App_USART1_Init(void);
void App_HCSR04_Init(void);

int main(void)
{
	App_USART1_Init();
	App_HCSR04_Init();
	while(1)
	{
		// 1.向CNT写0
		TIM_SetCounter(TIM1, 0);
		// 2.对cc1和cc2清零
		TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
		TIM_ClearFlag(TIM1, TIM_FLAG_CC2);
		// 3.开启定时器
		TIM_Cmd(TIM1, ENABLE);
		// 4.向TRIG引脚发送10us的脉冲
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
		DelayUs(10);
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
		// 5.等待cc1和cc2
		while(TIM_GetFlagStatus(TIM1, TIM_FLAG_CC1) == RESET);
		while(TIM_GetFlagStatus(TIM1, TIM_FLAG_CC2) == RESET);
		// 6.关闭定时器
		TIM_Cmd(TIM1, DISABLE);
		
		// 读取CCR1的值
		uint16_t ccr1 = TIM_GetCapture1(TIM1);
		// 读取CCR2的值
		uint16_t ccr2 = TIM_GetCapture2(TIM1);
		// 计算距离 = （ccr2-ccr1）*分辨率*声速*2
		float distance = (ccr2 - ccr1) * 1.0e-6f * 340.0f / 2;
		// 打印结果
		My_USART_Printf(USART1, "Distance = %.4f\r\n", distance);
		// 延迟
		Delay(100);
	}
}

void App_USART1_Init(void)
{
	// 1.初始化IO引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	// PA9 AF_PP
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	// PA10 IPU
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 2.初始化USART模块
	// 2.1开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	// 2.2初始化USART1的参数
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	
	// 3.闭合总开关
	USART_Cmd(USART1, ENABLE);
}

void App_HCSR04_Init(void)
{
	// 1.初始化时基单元
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = 65535;
	TIM_InitStruct.TIM_Prescaler = 71;
	TIM_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_InitStruct);
	
	// 2.初始化输入捕获
	// 2.1初始化IO引脚 PA8 IPD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	// 2.2初始化输入捕获的通道1
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 0;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1, &TIM_ICInitStruct);
	// 2.3初始化输入捕获的通道2
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter = 0;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInit(TIM1, &TIM_ICInitStruct);
	
	// 3.初始化Trig引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}
