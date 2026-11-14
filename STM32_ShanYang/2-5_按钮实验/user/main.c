// 引入 STM32F10x 标准外设库，提供 RCC、GPIO 等外设的类型和函数声明
#include "stm32f10x.h"

// 主函数：初始化 LED 和按键所使用的 GPIO，并持续读取按键状态
int main(void)
{
	// 1. 开启 GPIOA 的外设时钟
	// GPIOA 挂载在 APB2 总线上，使用 GPIOA 前必须先为它提供时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 定义 GPIO 初始化结构体，后续通过修改其成员来配置不同引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// 2. 配置 PA0：作为推挽输出，用于输出高、低电平
	// 指定要配置的引脚为 PA0
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	// 设置为通用推挽输出模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	// 设置 GPIO 最大输出速度为 2 MHz
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	// 将上述配置写入 GPIOA 的 PA0
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 3. 配置 PA1：作为上拉输入，用于读取按键状态
	// 复用同一个初始化结构体，将目标引脚改为 PA1
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	// 上拉输入模式：按键未按下时通常读取高电平，按下时读取低电平
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	// 将上述配置写入 GPIOA 的 PA1
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 此语句已被注释，因此当前不会执行；原作用是让 PA0 输出高电平
	// GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
	// 主循环：单片机上电后不断读取按键状态并控制 PA0 输出
	while(1)
	{
		// 读取 PA1：上拉输入检测到低电平，表示按键被按下
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_RESET){
			// 按键按下时，让 PA0 输出高电平
			GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
		}else{
			// 按键松开时，让 PA0 输出低电平
			GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
		}
	}
}
