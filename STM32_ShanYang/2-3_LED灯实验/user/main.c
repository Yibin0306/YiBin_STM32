// 引入 STM32F10x 标准外设库，提供芯片寄存器定义和外设控制接口
#include "stm32f10x.h"
// 引入延时模块，提供 Delay() 延时函数
#include "delay.h"

// 主函数：完成 LED 所用 GPIO 的初始化，并在循环中控制 LED 闪烁
int main(void)
{
	// 1.开启GPIOx的时钟
	// GPIOC 挂载在 APB2 总线上，使用 GPIOC 前必须先开启对应时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	// 2.初始化IO引脚，PC13 通用输出开漏模式 2MHz
	// 定义 GPIO 初始化结构体，并将所有成员初始化为 0
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	// 指定需要配置的引脚为 PC13
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	// 将 PC13 设置为通用开漏输出模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	// 设置 GPIO 的最大输出速度为 2 MHz
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	// 根据以上参数初始化 GPIOC
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	// 无限循环，使 PC13 持续在高、低电平之间切换
	while(1)
	{
		// PC13 输出高电平
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); // 写1，亮灯
		// 保持当前状态 100 ms
		Delay(100); // 延迟100ms
		// PC13 输出低电平
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);// 写0，灭灯
		// 再保持 100 ms，形成一个完整的闪烁周期
		Delay(100); // 延迟100ms
	}
}
