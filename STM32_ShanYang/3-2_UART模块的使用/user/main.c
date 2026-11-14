// 引入 STM32F10x 标准外设库，提供 RCC 和 USART 等外设的类型与控制函数
#include "stm32f10x.h"

// 主函数：配置 USART1 的通信参数，然后进入主循环
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
	
	// 主循环：后续可在这里加入串口发送、接收或数据处理代码
	while(1)
	{
		// 当前循环体为空，程序会持续停留在此处
	}
}
