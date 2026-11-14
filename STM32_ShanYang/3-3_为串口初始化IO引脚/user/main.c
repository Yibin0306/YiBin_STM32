// 引入 STM32F10x 标准外设库，提供 RCC、GPIO 和 USART 等外设接口
#include "stm32f10x.h"

// 主函数：初始化 USART1 及其重映射后的收发引脚
int main(void)
{
	// 1. 开启 USART1 的外设时钟
	// USART1 挂载在 APB2 总线上，使用前必须先为它提供时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// 定义 USART 初始化结构体，用于保存波特率、数据位等通信参数
	USART_InitTypeDef USART_InitStruct;
	
	// 2. 配置 USART1 的通信参数
	// 设置波特率为 115200 bit/s，通信双方必须使用相同的波特率
	USART_InitStruct.USART_BaudRate = 115200;
	// 同时启用发送和接收功能
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	// 设置每帧数据长度为 8 位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	// 不使用奇偶校验
	USART_InitStruct.USART_Parity = USART_Parity_No;
	// 使用 1 个停止位；上述配置通常简称为 8N1
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	
	// 3. 将结构体中的通信参数写入 USART1 的相关寄存器
	USART_Init(USART1, &USART_InitStruct);
	
	// 定义 GPIO 初始化结构体，用于配置串口的发送和接收引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*
	以下是 USART1 使用默认引脚时的配置：
	PA9 为发送引脚 TX，PA10 为接收引脚 RX。
	由于当前程序将 USART1 重映射到 GPIOB，因此这段代码被整体注释，不会执行。
	*/
	/*	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	*/
	
	// 4. 开启复用功能 AFIO 的时钟，引脚重映射功能由 AFIO 外设管理
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 将 USART1 的默认引脚从 PA9/PA10 重映射到 PB6/PB7
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	// 5. 配置 PB6 为 USART1 的发送引脚 TX
	// 开启 GPIOB 的外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// 指定要配置的引脚为 PB6
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	// 设置为复用推挽输出模式，由 USART1 外设控制引脚输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	// 设置 GPIO 最大输出速度为 10 MHz
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	// 将上述配置写入 GPIOB 的 PB6
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 6. 配置 PB7 为 USART1 的接收引脚 RX
	// 再次确保 GPIOB 的外设时钟处于开启状态
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// 指定要配置的引脚为 PB7
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	// 设置为上拉输入模式，用于接收外部串口数据
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	// 将上述配置写入 GPIOB 的 PB7
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 主循环：后续可在这里添加串口发送、接收或数据处理代码
	while(1)
	{
		// 当前循环体为空，程序会持续停留在此处
	}
}
