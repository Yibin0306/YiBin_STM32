 #include "stm32f10x.h"

volatile uint32_t CurrentTick = 0; // 用来记录当前时间，单位是ms

void App_Delay(uint32_t ms);
void APP_TIM3_TimeBaseInit(void);
void App_OnBoardLED(void);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	APP_TIM3_TimeBaseInit();
	App_OnBoardLED();
	while(1)
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		App_Delay(500);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
		App_Delay(500);
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

void App_Delay(uint32_t ms)
{
	uint32_t expireTime = CurrentTick + ms; // 延迟结束时间
	while(CurrentTick < expireTime); // 等待延迟结束
}  

void APP_TIM3_TimeBaseInit(void)
{
	// 1.开启定时器3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	// 2.配置时基单元的参数
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_Prescaler = 71;
	TIM_InitStruct.TIM_Period = 999;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_InitStruct);
	
	// 3.闭合开关
	TIM_Cmd(TIM3, ENABLE);
	
	// 4.使能Update中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	// 5.配置NVIC模块
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == SET) // 查标志位
	{
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		CurrentTick++;
	}
}
