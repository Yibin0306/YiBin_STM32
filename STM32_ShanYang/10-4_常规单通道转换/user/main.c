#include "stm32f10x.h"

void App_ADC1_Init(void);
void App_OnBoardLED(void);

int main(void)
{
	App_ADC1_Init();
	App_OnBoardLED();
	while(1)
	{
		// 1.清除EOC标志位
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		// 2.软件启动发送脉冲
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		// 3.等待常规序列转换完成
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		// 4.读取DR寄存器的值
		uint16_t dr = ADC_GetConversionValue(ADC1);
		// 5.把结果转换为电压
		float voltage = dr * (3.3f / 4095);
		
		if(voltage > 1.5){
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		}else{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
		}
	}
}

void App_ADC1_Init(void)
{
	// 1.初始化PA0引脚，模拟模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 2.配置ADC模块时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置分频器的分频系数(6分频)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1的时钟
	
	// 3.初始化ADC的基本参数
	ADC_InitTypeDef ADC_InitStruct = {0};
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE; // 关闭连续模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; // 对齐方式(左/右)
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 选择常规序列的外部触发信号，软件启动
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; // 双ADC模式，选择独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 1; // 常规序列的通道数
	ADC_InitStruct.ADC_ScanConvMode = DISABLE; // 扫描模式
	ADC_Init(ADC1, &ADC_InitStruct);
	
	// 4.配置常规序列
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	
	// 5.闭合总开关
	ADC_Cmd(ADC1, ENABLE);
}

void App_OnBoardLED(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}
