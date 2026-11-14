#include "stm32f10x.h"

void APP_SPI1_Init(void);

int main(void)
{
	APP_SPI1_Init();
	while(1)
	{
	}
}

void APP_SPI1_Init(void)
{
	// 1.初始化IO引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// PB3 SCK AF_PP 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// PB4 MISO IPU
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// PB5 MOSI AF_PP 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// PA15 普通IO Out_PP 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 2.对SPI本身进行初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master; // 选择SPI通信的方向
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 设置两线全双工
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // 选择数据宽度
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High; // 选择极性
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge; // 选择相位
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB; // 选择传输顺序
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // 设置波特率为1Mz
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; // 设置NSS为软件NSS
	
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set); // NSS写入低电压
}
