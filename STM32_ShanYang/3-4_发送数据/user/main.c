// 引入 STM32F10x 标准外设库，提供 RCC、GPIO 和 USART 等外设接口
#include "stm32f10x.h"

// 函数声明：通过指定的 USART 外设连续发送一组字节
// USARTx：使用的串口；pData：待发送数据的首地址；Size：要发送的字节数
void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);

// 主函数：完成 USART1、重映射引脚和通信参数的初始化，然后发送测试数据
int main(void)
{
	// 定义 GPIO 初始化结构体，后续用它配置 USART1 的 TX 和 RX 引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*
	USART1 默认使用 PA9 作为 TX、PA10 作为 RX。
	当前程序准备把 USART1 重映射到 PB6/PB7，所以默认引脚配置被注释，不会执行。
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
	
	// 1. 配置 USART1 引脚重映射
	// 开启 AFIO 时钟，因为 STM32F1 的引脚重映射功能由 AFIO 外设管理
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 将 USART1 的默认引脚从 PA9/PA10 重映射到 PB6/PB7
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	// 2. 配置 PB6 为 USART1_TX 发送引脚
	// PB6、PB7 都属于 GPIOB，因此使用前先开启 GPIOB 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// 选择 PB6
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	// TX 是输出引脚，设置为复用推挽输出，由 USART1 外设控制电平
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	// 设置 PB6 的最大输出速度为 10 MHz
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	// 将结构体中的配置应用到 PB6
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 3. 配置 PB7 为 USART1_RX 接收引脚
	// GPIOB 时钟在上方已经开启；再次开启不会改变当前配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// 选择 PB7
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	// RX 是输入引脚，设置为上拉输入以接收外部串口信号
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	// 将结构体中的配置应用到 PB7
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 4. 开启 USART1 时钟
	// USART1 挂载在 APB2 总线上，使用串口前必须先开启对应时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// 定义 USART 初始化结构体，用来保存串口的各项通信参数
	USART_InitTypeDef USART_InitStruct;
	
	// 5. 配置 USART1：115200 波特率、8 位数据、无校验、1 个停止位（8N1）
	// 设置通信速率为 115200 bit/s，发送端和接收端的波特率必须一致
	USART_InitStruct.USART_BaudRate = 115200;
	// 同时打开串口的发送和接收功能
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	// 每帧包含 8 位有效数据
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	// 关闭奇偶校验
	USART_InitStruct.USART_Parity = USART_Parity_No;
	// 每帧使用 1 个停止位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	
	// 把上述通信参数写入 USART1 的配置寄存器
	USART_Init(USART1, &USART_InitStruct);
	
	// 6. 使能 USART1；执行后串口才开始按照设定参数工作
	USART_Cmd(USART1, ENABLE);
	
	// 准备 5 个待发送的原始字节：0x01、0x02、0x03、0x04、0x05
	// 它们不是字符 '1'～'5'；字符 '1'～'5' 对应的 ASCII 值为 0x31～0x35
	uint8_t bytesToSend[] = {1,2,3,4,5};
	// 通过 USART1 依次发送数组中的 5 个字节
	My_USART_SendBytes(USART1, bytesToSend, 5);
	
	
	// 数据只会在进入主循环前发送一次，之后程序一直停留在主循环中
	while(1)
	{
		// 当前没有需要重复执行的任务
	}
}

// 连续发送多个字节
// 每写入一个字节前等待 TXE，全部写完后再等待 TC，确保最后一位已经发送完成
void My_USART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
	// 遍历待发送数组，每次发送一个字节
	for(uint32_t i = 0; i < Size; i++){
		// 等待 TXE（发送数据寄存器空）变为 SET
		// while 末尾的分号表示循环体为空，CPU会在这里持续查询标志位
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		// 将当前字节写入 USART 的发送数据寄存器
		USART_SendData(USARTx, pData[i]);
	}
	
	// 等待 TC（发送完成）变为 SET，确认最后一个字节已经完全从 TX 引脚发出
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}
