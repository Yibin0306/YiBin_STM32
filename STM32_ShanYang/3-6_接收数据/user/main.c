#include "stm32f10x.h"

void My_USART1_Init(void);
void My_OnBoardLED_Init(void);

int main(void)
{
	My_USART1_Init();
	My_OnBoardLED_Init();
	while(1)
	{
		// 1.等待接收数据寄存器RDR非空
		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		
		// 2.读取数据
		uint8_t byteRcvd = USART_ReceiveData(USART1);
		
		// 3.对数据进行处理
		if(byteRcvd == '0')
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); //亮灯
		else if(byteRcvd == '1')
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); //灭灯
	}
}

/*
	@简介：对USART1进行初始化
		   PB6 - Tx, PB7 - Rx
		   115200, 8, 1, None, 双向
	@参数 USARTx：填写串口的名称
	@参数 pData：要发送的数据
	@参数 Size：要发送的数据的数量，单位是字节
*/

void My_USART1_Init(void)
{
	// 1.初始化PB6和PB7
	GPIO_InitTypeDef GPIO_InitStruct;
	
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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	// PB6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// PB7
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
	// 2.初始化USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_InitTypeDef USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	
	USART_Init(USART1, &USART_InitStruct);
	
	USART_Cmd(USART1, ENABLE);// 闭合总开关
}

void My_OnBoardLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}