#include "stm32f10x.h"

void My_I2C_Init(void);

int main(void)
{
	My_I2C_Init();
	while(1)
	{
	}
}

void My_I2C_Init(void)
{
	// 1.IO引脚初始化
	
	// 对I2C1进行重映射
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	
	// 对PB8和PB9进行初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 2.初始化I2C1模块
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); // 开启I2C1的时钟
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE); // 施加复位信号
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE); // 释放复位信号
	
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_InitStruct.I2C_ClockSpeed = 400000; // 波特率400k
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C; // 标准的I2C
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; // 占空比2：1
	I2C_Init(I2C1, &I2C_InitStruct);
	
	I2C_Cmd(I2C1, ENABLE); // 闭合I2C1的总开关
}
