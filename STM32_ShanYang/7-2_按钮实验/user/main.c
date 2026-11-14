#include "stm32f10x.h"

void App_OnBoardLED(void);
void App_Button_Init(void);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	App_OnBoardLED();
	App_Button_Init();
	while(1)
	{
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
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}

void App_Button_Init(void)
{
	// 1.初始化PA5和PA6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// 2.为EXTI5和EXTI6分配引脚
	// 开启AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 线5选择端口A
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);
	// 线6选择端口A
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);
	
	// 3.初始化EXTI的线
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line5 | EXTI_Line6;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	// 4.配置NVIC模块
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line5) == SET){ // 获取EXTI5线的标志位的值
		EXTI_ClearFlag(EXTI_Line5); // 清除中断标志位
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	}
	if(EXTI_GetFlagStatus(EXTI_Line6) == SET){ // 获取EXTI6线的标志位的值
		EXTI_ClearFlag(EXTI_Line6); // 清除中断标志位
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	}
}
