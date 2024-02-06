#include "W25Q32.h"

void FLASH_SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(FLASH_SPI_CS_GPIO_CLK|FLASH_SPI_SCK_GPIO_CLK|FLASH_SPI_MISO_GPIO_CLK|FLASH_SPI_MOSI_GPIO_CLK, ENABLE);
	FLASH_SPI_CLK_INIT(FLASH_SPI_CLK, ENABLE);
	
	// SPI SCK ���ų�ʼ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);
    // SPI MISO ���ų�ʼ��
    GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MISO_PIN;
    GPIO_Init(FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);
    // SPI MOSI ���ų�ʼ��
    GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MOSI_PIN;
    GPIO_Init(FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);
	// CS ���ų�ʼ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_Init(FLASH_SPI_CS_GPIO_PORT, &GPIO_InitStruct);
	
	// ��������
	GPIO_PinAFConfig(FLASH_SPI_SCK_GPIO_PORT, FLASH_SPI_SCK_PINSOURCE, FLASH_SPI_SCK_AF);
	GPIO_PinAFConfig(FLASH_SPI_MISO_GPIO_PORT, FLASH_SPI_MISO_PINSOURCE, FLASH_SPI_MISO_AF);
	GPIO_PinAFConfig(FLASH_SPI_MOSI_GPIO_PORT, FLASH_SPI_MOSI_PINSOURCE, FLASH_SPI_MOSI_AF);

	// SPI��ʼ��
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // ��Ƶ
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // ˫��ȫ˫��
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;        // SPI����
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;    // 8λ����
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;   // ��λ����
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;         // ��һ�����ز������������أ�
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;           // SCK����ʱΪ0
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;            // ���NSS
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(FLASH_SPI, &SPI_InitStruct);
	
	// ʹ��SPI
	SPI_Cmd(FLASH_SPI, ENABLE);
	
	FLASH_W_CS(1);
}

uint8_t FLASH_SPI_SwapByte(uint8_t data)
{
	while( SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) != SET );
	SPI_I2S_SendData(FLASH_SPI, data);
	while( SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) != SET );
	return SPI_I2S_ReceiveData(FLASH_SPI);
}

void W25Q32_Init(void)
{
	FLASH_SPI_Init();
}

uint32_t W25Q32_Read_JEDEC_ID()
{
	uint32_t JEDEC_ID = 0;
	W25Q32_SPI_Start();
	FLASH_SPI_SwapByte(CMD_JEDEC_ID);
	JEDEC_ID |= FLASH_SPI_SwapByte(0xFF); JEDEC_ID <<= 8;
	JEDEC_ID |= FLASH_SPI_SwapByte(0xFF); JEDEC_ID <<= 8;
	JEDEC_ID |= FLASH_SPI_SwapByte(0xFF);
	W25Q32_SPI_Stop();
	return JEDEC_ID;
}

uint8_t W25Q32_Read_SR(void)
{
	uint8_t temp;
	
	W25Q32_SPI_Start();
	
	FLASH_SPI_SwapByte(CMD_STATUS_REGISTER_1);
	
	temp = FLASH_SPI_SwapByte(0xFF);
	
	W25Q32_SPI_Stop();
	
	return temp;
}

void W25Q32_Wait_Busy()
{
	while ((W25Q32_Read_SR()&0x01)==0x01);
}

void W25Q32_Write_Able(uint8_t status)
{
	W25Q32_SPI_Start();
	if (status != 0)
	{
		FLASH_SPI_SwapByte(CMD_WRITE_ENABLE);
	}
	else
	{
		FLASH_SPI_SwapByte(CMD_WRITE_DISENABLE);
	}
	W25Q32_SPI_Stop();
}

// ����W25Q32��һ������
// addr: Ҫ�����ĵ�ַ��W25Q32���Զ��Ѹõ�ַ���ڵ���������
void W25Q32_Sector_Erase(uint32_t addr)
{
	W25Q32_Write_Able(1); // ��дʹ��
	
	W25Q32_SPI_Start();
	FLASH_SPI_SwapByte(CMD_SECTOR_ERASE_4KB);
	FLASH_SPI_SwapByte(addr >> 16);
	FLASH_SPI_SwapByte(addr >> 8);
	FLASH_SPI_SwapByte(addr);
	W25Q32_SPI_Stop();
	W25Q32_Wait_Busy();
}

// ��W25Q32д��һҳ������256�ֽ�
// addr: д��ĵ�ַ
// data: д������ݻ�����
// count: д����ֽ����������256�ֽ�
void W25Q32_Write_Page(uint32_t addr, uint8_t* data, uint32_t count)
{
	W25Q32_Write_Able(1); // ��дʹ��
	
	W25Q32_SPI_Start();
	FLASH_SPI_SwapByte(CMD_PAGE_WRITE); // ����дҳ��ָ��
	FLASH_SPI_SwapByte(addr >> 16);
	FLASH_SPI_SwapByte(addr >> 8);
	FLASH_SPI_SwapByte(addr);
	
	for (uint32_t i = 0; i < count; i++)
	{
		FLASH_SPI_SwapByte(data[i]);
	}

	W25Q32_SPI_Stop();
	
	W25Q32_Wait_Busy();
}

// ��W25Q32д������
// addr: д��ĵ�ַ
// data: д������ݻ�����
// count: д����ֽ����������2^32�ֽ�
void W25Q32_Write_Data(uint32_t addr, uint8_t* data, uint32_t count)
{
	uint16_t page_number = 256;               // ��ǰҳʣ��д����ֽ�
	uint8_t temp = 0;
	while(1)
	{
		if (count < 256) 
		{
			page_number = count;     // ���д���С��256�ֽ�
			temp = 1;
		}
		W25Q32_Write_Page(addr, data, page_number);
		if (temp == 1) break; // д�����
		addr += 256;
		data += 256;
		count -= 256;
	}
}

// ��ȡW25Q32������
// addr: ��ȡ�ĵ�ַ
// data: ��ȡ�����ݻ�����
// count: ��ȡ���ֽ����������2^32�ֽ�
void W25Q32_Read_Data(uint32_t addr, uint8_t* data, uint32_t count)
{
	W25Q32_SPI_Start();
	FLASH_SPI_SwapByte(CMD_READ);
	FLASH_SPI_SwapByte(addr >> 16);
	FLASH_SPI_SwapByte(addr >> 8);
	FLASH_SPI_SwapByte(addr);

	for (uint32_t i = 0; i < count; i++)
	{
		data[i] = FLASH_SPI_SwapByte(0xFF);
	}
	
	W25Q32_SPI_Stop();
}

void W25Q32_Sector_Chip(void)
{
	W25Q32_Write_Able(1); // ��дʹ��
	
	W25Q32_SPI_Start();
	FLASH_SPI_SwapByte(CMD_SECTOR_CHIP);
	W25Q32_SPI_Stop();
	W25Q32_Wait_Busy();
}
