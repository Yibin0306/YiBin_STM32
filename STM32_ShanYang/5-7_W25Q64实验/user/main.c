#include "stm32f10x.h"
#include "button.h"
Button_TypeDef Button; // 声明一个按钮

void APP_SPI1_Init(void);
void APP_SPI_MasterTreasmitReceive(SPI_TypeDef *SPIx, const uint8_t *pDataTX, uint8_t *pDataRX, uint16_t size);

void APP_W25Q64_SaveByte(uint8_t Byte); // 使用W25Q64保存一个字节 
uint8_t APP_W25Q64_LoadByte(void); // 把保存的字节读出来

void APP_OnBoardLED_Init(void);
void APP_Button_Init(void);
void button_clicked_cb(uint8_t clicks);

int main(void)
{
	APP_SPI1_Init();
	APP_OnBoardLED_Init();
	APP_Button_Init();
	
	uint8_t byte = APP_W25Q64_LoadByte();
	if(byte == 0){
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	}else{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	}
	while(1)
	{
		My_Button_Proc(&Button); // 进程
	}
}

void button_clicked_cb(uint8_t clicks)
{
	if(clicks == 1){
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
			APP_W25Q64_SaveByte(0x01); // 保存0x01
		}else{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
			APP_W25Q64_SaveByte(0x00); // 保存0x00
		}
	}
}

void APP_Button_Init(void)
{
	Button_InitTypeDef Button_InitStruct = {0};
	Button_InitStruct.GPIOx = GPIOA;
	Button_InitStruct.GPIO_Pin = GPIO_Pin_0;
	Button_InitStruct.button_clicked_cb = button_clicked_cb;
	My_Button_Init(&Button, &Button_InitStruct);
}

void APP_OnBoardLED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void APP_SPI1_Init(void)
{
	// 1.初始化IO引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
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
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);
	
	// 2.对SPI本身进行初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master; // 选择SPI通信的方向
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 设置两线全双工
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // 选择数据宽度
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; // 选择极性
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge; // 选择相位
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB; // 选择传输顺序
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // 设置波特率为1Mz
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; // 设置NSS为软件NSS
	
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set); // NSS写入低电压
}

void APP_SPI_MasterTreasmitReceive(SPI_TypeDef *SPIx, const uint8_t *pDataTX, uint8_t *pDataRX, uint16_t size)
{
	SPI_Cmd(SPIx, ENABLE); // 1.闭合总开关
	SPI_I2S_SendData(SPIx, pDataTX[0]); // 2.发送第一个字节
	// 3.
	for(uint16_t i=0; i<size-1; i++){
		// 发送第一个字节
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPIx, pDataTX[i+1]);
		// 接收第一个字节
		while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
		pDataRX[i] = SPI_I2S_ReceiveData(SPIx);
	}
	// 4.读出最后一个字节
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	pDataRX[size-1] = SPI_I2S_ReceiveData(SPIx);
	// 5.断开总开关
	SPI_Cmd(SPIx, DISABLE);
}

void APP_W25Q64_SaveByte(uint8_t Byte)
{
	uint8_t buffer[10];
	// 1.写使能
	buffer[0] = 0x06;
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); // NSS=0
	APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 1);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); // NSS=1
	
	// 2.扇区擦除
	buffer[0] = 0x20;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); // NSS=0
	APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 4);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); // NSS=1
	
	// 3.等待空闲
	while(1){
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); // NSS=0
		buffer[0] = 0x05;
		APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 1);
		buffer[0] = 0xff;
		APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); // NSS=1
		if((buffer[0] & 0x01) == 0) break;
	}
	
	// 4.写使能
	buffer[0] = 0x06;
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); // NSS=0
	APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 1);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); // NSS=1
	
	// 5.页编程
	buffer[0] = 0x02;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;
	buffer[4] = Byte;
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); // NSS=0
	APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 5);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); // NSS=1
	
	// 6.等待空闲
	while(1){
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); // NSS=0
		buffer[0] = 0x05;
		APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 1);
		buffer[0] = 0xff;
		APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); // NSS=1
		if((buffer[0] & 0x01) == 0) break;
	}
}

uint8_t APP_W25Q64_LoadByte(void)
{
	uint8_t buffer[10];
	buffer[0] = 0x03;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); // NSS=0
	APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 4); // 发0x03+24位地址
	buffer[0] = 0xff;
	APP_SPI_MasterTreasmitReceive(SPI1, buffer, buffer, 1); // 收一个字节
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); // NSS=1
	
	return buffer[0];
}
