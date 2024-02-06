#include "ST7789V.h"
#include "Delay.h"
#include "ASCII_Font.h"

struct LCD_INFO LCD_Info;
struct FONT_INFO Font_Info;

DMA_InitTypeDef DMA_InitStruct;

/************************************************
 * LCD_SPI_Init
 * 功能：SPI初始化
 * 参数：无
 * 返回：无
*************************************************/
void LCD_SPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// 使能时钟
	RCC_AHB1PeriphClockCmd(LCD_SPI_CS_GPIO_CLK | LCD_SPI_SCK_GPIO_CLK | LCD_SPI_MOSI_GPIO_CLK | LCD_SPI_MISO_GPIO_CLK, ENABLE);
	LCD_SPI_CLK_INIT(LCD_SPI_CLK, ENABLE);
	
	// SPI引脚初始化 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = LCD_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(LCD_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_SPI_MISO_PIN;
	GPIO_Init(LCD_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_SPI_MOSI_PIN;
	GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = LCD_SPI_CS_PIN;
	GPIO_Init(LCD_SPI_CS_GPIO_PORT, &GPIO_InitStruct);
	
	// 复用引脚
	GPIO_PinAFConfig(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_PINSOURCE, LCD_SPI_SCK_AF);
	GPIO_PinAFConfig(LCD_SPI_MISO_GPIO_PORT, LCD_SPI_MISO_PINSOURCE, LCD_SPI_MISO_AF);
	GPIO_PinAFConfig(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_PINSOURCE, LCD_SPI_MOSI_AF);
	
	// SPI初始化
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 分频
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 双线全双工
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;        // SPI主机
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;   // 16位数据
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;   // 高位先行
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;         // 第一个边沿采样（奇数边沿）
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;           // SCK空闲时为0
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;            // 软件NSS
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(LCD_SPI, &SPI_InitStruct);
	
	// 使能SPI
	SPI_Cmd(LCD_SPI, ENABLE);
	
	// DMA时钟
	RCC_AHB1PeriphClockCmd(LCD_DMA_CLK, ENABLE);
	
	// TX
	DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.DMA_Channel = LCD_TX_DMA_CHANNEL;                  // 通道0
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&LCD_SPI->DR); // 外设地址
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;              // 内存->外设模式
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 外设数据宽度（16位）
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          // 内存数据宽度（16位）
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;                            // 单次模式
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;                      // 优先级为高
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;                   // FIFO禁用
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;              // 突发模式（单次）
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;      // 突发模式（单次）
}

/************************************************
 * LCD_SPI_SwapByte
 * 功能：SPI 发送并接收一个字节
 * 参数：data 要发送的字节
 * 返回：接收的字节
*************************************************/
uint8_t LCD_SPI_SwapByte(uint8_t data)
{
	SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_8b);
	
	while( SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) != SET );
	SPI_I2S_SendData(LCD_SPI, data);
	while( SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) != SET );
	return SPI_I2S_ReceiveData(LCD_SPI);
}

/************************************************
 * LCD_SPI_SwapHalfWord
 * 功能：SPI 发送并接收一个半字
 * 参数：data 要发送的半字
 * 返回：接收的半字
*************************************************/
uint16_t LCD_SPI_SwapHalfWord(uint16_t data)
{
	SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_16b);
	
	while( SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) != SET );
	SPI_I2S_SendData(LCD_SPI, data);
	while( SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) != SET );
	return SPI_I2S_ReceiveData(LCD_SPI);
}

/************************************************
 * LCD_SPI_DMATransmit
 * 功能：SPI 使用DMA发送数据
 * 参数：data     数据的首地址
 *       bufsize  数据的数目
 *       datasize 数据的宽度（比特）（8或16）
 * 返回：无
*************************************************/
void LCD_SPI_DMATransmit(uint8_t* data, uint32_t bufsize, uint8_t datasize)
{
	if (bufsize == 0) return;
	
	if (datasize == 8)
	{
		SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_8b);
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度（8位）
	    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // 内存数据宽度（8位）
	}
	else if (datasize == 16)
	{
		SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_16b);
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 外设数据宽度（16位）
	    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          // 内存数据宽度（16位）
	}
	
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)data;             // 内存地址
	DMA_InitStruct.DMA_BufferSize = bufsize;                         // 数据数目
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    // 外设地址不递增
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;             // 内存地址递增
	DMA_Init(LCD_TX_DMA_STREAM, &DMA_InitStruct);                         // 使用DMA1数据流4
	
	// 清除DMA数据传输完成标志
	DMA_ClearFlag(LCD_TX_DMA_STREAM, LCD_TX_DMA_IT_TCIF);
	// 使能数据流
	DMA_Cmd(LCD_TX_DMA_STREAM, ENABLE);
	// 等待DMA数据流有效
	while (DMA_GetCmdStatus(LCD_TX_DMA_STREAM) != ENABLE);
	
	// SPI向DMA发送传输请求
	SPI_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
	
}

/************************************************
 * LCD_SPI_DMARepeatTransmit
 * 功能：SPI 使用DMA重复发送一个数据
 * 参数：data     数据的地址
 *       repcount 重复发送多少次
 *       datasize 数据的宽度（比特）（8或16）
 * 返回：无
*************************************************/
void LCD_SPI_DMARepeatTransmit(uint8_t* data, uint32_t repcount, uint8_t datasize)
{
	if (repcount == 0) return;
	
	if (datasize == 8)
	{
		SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_8b);
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度（8位）
	    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // 内存数据宽度（8位）
	}
	else if (datasize == 16)
	{
		SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_16b);
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 外设数据宽度（16位）
	    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          // 内存数据宽度（16位）
	}

	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)(data);           // 内存地址
	DMA_InitStruct.DMA_BufferSize = repcount;                        // 数据数目
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    // 外设地址不递增
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;            // 内存地址不递增
	DMA_Init(LCD_TX_DMA_STREAM, &DMA_InitStruct);                         // 使用DMA1数据流4
	
	// 清除DMA数据传输完成标志
	DMA_ClearFlag(LCD_TX_DMA_STREAM, LCD_TX_DMA_IT_TCIF);
	// 使能数据流
	DMA_Cmd(LCD_TX_DMA_STREAM, ENABLE);
	// 等待DMA数据流有效
	while (DMA_GetCmdStatus(LCD_TX_DMA_STREAM) != ENABLE);
	
	// SPI向DMA发送传输请求
	SPI_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
}

void LCD_SetFontColor(uint16_t color)
{
	LCD_Info.LCD_FONT_COLOR = color;
}
void LCD_SetBackGroundColor(uint16_t color)
{
	LCD_Info.LCD_BACK_COLOR = color;
}

void ST7789_Write_Data8(uint8_t data)
{
	ST7789_DC_Set;
	LCD_SPI_SwapByte(data);
}

void ST7789_Write_Data16(uint16_t data)
{
	ST7789_DC_Set;
	ST7789_CS_Clr;
	LCD_SPI_SwapByte(data >> 8);
	LCD_SPI_SwapByte(data);
	ST7789_CS_Set;
}

void ST7789_Write_Reg(uint8_t data)
{
	ST7789_DC_Clr;
    ST7789_CS_Clr;
    LCD_SPI_SwapByte(data);
}

void LCD_SetAttributes(uint8_t Scan_dir)
{
    uint8_t MemoryAccessReg = 0x00;
	LCD_Info.Dir = Scan_dir;

    if (Scan_dir == HORIZONTAL) 
	{
		LCD_Info.HEIGHT = LCD_W;
		LCD_Info.WIDTH = LCD_H;
        MemoryAccessReg = 0X70;
    }
    else 
    {
		LCD_Info.HEIGHT = LCD_H;
		LCD_Info.WIDTH = LCD_W;
        MemoryAccessReg = 0X00;
    }

    // Set the read / write scan direction of the frame memory
    ST7789_Write_Reg(0x36); // MX, MY, RGB mode
    ST7789_Write_Data8(MemoryAccessReg); // 0x08 set RGB
}

void LCD_Init(void)
{
	LCD_SPI_Init();
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(LCD_RST_GPIO_CLK | LCD_DC_GPIO_CLK | LCD_BLK_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = LCD_RST_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(LCD_RST_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_DC_PIN;
	GPIO_Init(LCD_DC_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LCD_BLK_PIN;
	GPIO_Init(LCD_BLK_GPIO_PORT, &GPIO_InitStruct);
	
	LCD_Info.LCD_BACK_COLOR = BLACK;
	LCD_Info.LCD_FONT_COLOR = WHITE;
	
	ST7789_BLK_Set;
	ST7789_CS_Set;
	
	// 复位
	ST7789_RES_Set;
    Delay_ms(100);
    ST7789_RES_Clr;
    Delay_ms(100);
    ST7789_RES_Set;
    Delay_ms(100);
	
	ST7789_Write_Reg(0x36);
    ST7789_Write_Data8(0x00);

    ST7789_Write_Reg(0x3A);
    ST7789_Write_Data8(0x05);

    ST7789_Write_Reg(0xB2);
    ST7789_Write_Data8(0x0B);
    ST7789_Write_Data8(0x0B);
    ST7789_Write_Data8(0x00);
    ST7789_Write_Data8(0x33);
    ST7789_Write_Data8(0x35);

    ST7789_Write_Reg(0xB7);
    ST7789_Write_Data8(0x11);

    ST7789_Write_Reg(0xBB);
    ST7789_Write_Data8(0x35);

    ST7789_Write_Reg(0xC0);
    ST7789_Write_Data8(0x2C);

    ST7789_Write_Reg(0xC2);
    ST7789_Write_Data8(0x01);

    ST7789_Write_Reg(0xC3);
    ST7789_Write_Data8(0x0D);

    ST7789_Write_Reg(0xC4);
    ST7789_Write_Data8(0x20);

    ST7789_Write_Reg(0xC6);
    ST7789_Write_Data8(0x13);

    ST7789_Write_Reg(0xD0);
    ST7789_Write_Data8(0xA4);
    ST7789_Write_Data8(0xA1);

    ST7789_Write_Reg(0xD6);
    ST7789_Write_Data8(0xA1);

    ST7789_Write_Reg(0xE0);
    ST7789_Write_Data8(0xF0);
    ST7789_Write_Data8(0x06);
    ST7789_Write_Data8(0x0B);
    ST7789_Write_Data8(0x0A);
    ST7789_Write_Data8(0x09);
    ST7789_Write_Data8(0x26);
    ST7789_Write_Data8(0x29);
    ST7789_Write_Data8(0x33);
    ST7789_Write_Data8(0x41);
    ST7789_Write_Data8(0x18);
    ST7789_Write_Data8(0x16);
    ST7789_Write_Data8(0x15);
    ST7789_Write_Data8(0x29);
    ST7789_Write_Data8(0x2D);

    ST7789_Write_Reg(0xE1);
    ST7789_Write_Data8(0xF0);
    ST7789_Write_Data8(0x04);
    ST7789_Write_Data8(0x08);
    ST7789_Write_Data8(0x08);
    ST7789_Write_Data8(0x07);
    ST7789_Write_Data8(0x03);
    ST7789_Write_Data8(0x28);
    ST7789_Write_Data8(0x32);
    ST7789_Write_Data8(0x40);
    ST7789_Write_Data8(0x3B);
    ST7789_Write_Data8(0x19);
    ST7789_Write_Data8(0x18);
    ST7789_Write_Data8(0x2A);
    ST7789_Write_Data8(0x2E);

    ST7789_Write_Reg(0xE4);
    ST7789_Write_Data8(0x25);
    ST7789_Write_Data8(0x00);
    ST7789_Write_Data8(0x00);

    ST7789_Write_Reg(0x21);

    ST7789_Write_Reg(0x11);
    Delay_ms(120);
    ST7789_Write_Reg(0x29);
}

// 设置窗口位置
void LCD_SetCursor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{ 
	if (LCD_Info.Dir == VERTICAL) 
	{
        ST7789_Write_Reg(0x2A);
        ST7789_Write_Data8(x1 >> 8);
        ST7789_Write_Data8(x1);
        ST7789_Write_Data8((x2-1) >> 8);
        ST7789_Write_Data8(x2-1);

        // set the Y coordinates
        ST7789_Write_Reg(0x2B);
        ST7789_Write_Data8((y1+20) >> 8);
        ST7789_Write_Data8(y1+20);
        ST7789_Write_Data8((y2+20-1) >> 8);
        ST7789_Write_Data8(y2+20-1);
    } 
	else 
	{
        // set the X coordinates
        ST7789_Write_Reg(0x2A);
        ST7789_Write_Data8((x1+20) >> 8);
        ST7789_Write_Data8(x1+20);
        ST7789_Write_Data8((x2+20-1) >> 8);
        ST7789_Write_Data8(x2+20-1);

        // set the Y coordinates
        ST7789_Write_Reg(0x2B);
        ST7789_Write_Data8(y1 >> 8);
        ST7789_Write_Data8(y1);
        ST7789_Write_Data8((y2-1) >> 8);
        ST7789_Write_Data8(y2-1);
    }

	ST7789_Write_Reg(0x2C);			
}

void LCD_Clear(uint16_t color)
{
	LCD_Fill(0, 0, LCD_Info.WIDTH, LCD_Info.HEIGHT, color);
}

void LCD_Fill(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
	uint32_t temp = xEnd - xStart + 1;

    for (uint32_t i = yStart; i <= yEnd; i++)
    {
		LCD_SetCursor(xStart, i, xStart, i);
		ST7789_DC_Set;
		
        for (uint32_t j = 0; j < temp; j++)
        {
            LCD_SPI_SwapByte(color >> 8);
		    LCD_SPI_SwapByte(color);
        }
    }
	
}

void LCD_DrawPixel(int x, int y, uint16_t color)
{
	LCD_SetCursor(x, y, x, y);
	ST7789_Write_Data16(color);
}

void LCD_DrawBitmap(int x, int y, uint16_t w, uint16_t h, uint8_t* bmp)
{
	uint16_t* temp = (uint16_t*)bmp;
	
    for (uint32_t i = y; i < y+h; i++)
    {
		LCD_SetCursor(x, i, x, i);
		ST7789_DC_Set;
		
        for (uint32_t j = 0; j < w; j++)
        {
            ST7789_Write_Data16( *temp++ );
        }
    }
}

void LCD_DrawBitmap_DMA(int x, int y, uint16_t w, uint16_t h, uint8_t* bmp)
{
	LCD_SetCursor(x, y, x+w, y+h);
	ST7789_DC_Set;
	
	uint32_t left_byte = w*h; // 剩下的color
	
	while(1)
	{
		if (left_byte <= 65535)
		{
			LCD_SPI_DMATransmit(bmp, left_byte, 16);
			// 等待传输完成
			while (DMA_GetFlagStatus(LCD_TX_DMA_STREAM, LCD_TX_DMA_IT_TCIF) == DISABLE);
			return;
		}
		else
		{
			LCD_SPI_DMATransmit(bmp, 65535, 16);
			// 等待传输完成
			while (DMA_GetFlagStatus(LCD_TX_DMA_STREAM, LCD_TX_DMA_IT_TCIF) == DISABLE);
			left_byte -= 65535;
		}
	}
}

void LCD_Fill_DMA(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	LCD_SetCursor(x, y, x+w, y+h);
	ST7789_DC_Set;
	
	uint32_t left_byte = w*h; // 剩下的color
	
	while(1)
	{
		if (left_byte <= 65535)
		{
			LCD_SPI_DMARepeatTransmit((uint8_t*)&color, left_byte, 16);
			// 等待传输完成
			while (DMA_GetFlagStatus(LCD_TX_DMA_STREAM, LCD_TX_DMA_IT_TCIF) == DISABLE);
			return;
		}
		else
		{
			LCD_SPI_DMARepeatTransmit((uint8_t*)&color, 65535, 16);
			// 等待传输完成
			while (DMA_GetFlagStatus(LCD_TX_DMA_STREAM, LCD_TX_DMA_IT_TCIF) == DISABLE);
			left_byte -= 65535;
		}
	}
}

void LCD_ShowMat16x16(int x, int y, uint8_t* mat)
{
	uint16_t buf[16*16];
	uint16_t bitseek = 0, byteseek = 0;
	
	for (uint32_t yy = 0; yy < 16; yy++)
	{
		for (uint32_t bb = 0; bb < 2; bb++)
		{
			uint8_t c = mat[byteseek++];
			
			for (uint32_t i = 0; i < 8; i++)
			{
				if ((0x80 & c) == 0x80)
				{
					buf[bitseek++] = LCD_Info.LCD_FONT_COLOR;
				}
				else
				{
					buf[bitseek++] = LCD_Info.LCD_BACK_COLOR;
				}
				c <<= 1;
			}
		}
	}
	
	LCD_DrawBitmap_DMA(x, y, 16, 16, (uint8_t*)buf);
}

void LCD_ShowMat8x16(int x, int y, uint8_t* mat)
{
	uint16_t buf[8*16];
	uint16_t seek = 0;
	
	for (uint32_t yy = 0; yy < 16; yy++)
	{
		uint8_t c = mat[yy];
		
		for (uint32_t i = 0; i < 8; i++)
		{
			if ((0x80 & c) == 0x80)
			{
				buf[seek++] = LCD_Info.LCD_FONT_COLOR;
			}
			else
			{
				buf[seek++] = LCD_Info.LCD_BACK_COLOR;
			}
			c <<= 1;
		}
	}
	
	LCD_DrawBitmap_DMA(x, y, 8, 16, (uint8_t*)buf);
}

void _LCD_ShowChinese(int x, int y, uint8_t* str)
{
	uint8_t HB = *str;
	uint8_t LB = *(str + 1);
	uint32_t MAP = 0;

	// GBK 第一个字节 0x81~0xFE 第二个字节 分为两部分 0x40~0x7E 和 0x80~0xFE
	// 汉字 0x8140 ~ 0xFEFE 没有 0x**7F
	if (HB >= 0x81 && HB <= 0xFE)
	{
		if (LB >= 0x40 && LB <= 0x7E)
		{
			MAP = (((HB - 0x81) * 190) + LB - 0x40) * 32;
		}
		else if (LB >= 0x80 && LB <= 0xFE)
		{
			MAP = (((HB - 0x81) * 190) + LB - 0x41) * 32;
		}
		
		uint8_t buf[32] = { 0 };
		W25Q32_Read_Data(MAP, buf, 32);
		LCD_ShowMat16x16(x, y, buf);
	}
}

void _LCD_ShowASCII(int x, int y, uint8_t* str)
{
	LCD_ShowMat8x16(x, y, (uint8_t*)(ASCII_8x16 + (*str * 16)));
}

uint32_t LCD_ShowString(int x, int y, int w, int h, uint8_t* str)
{
	int str_x = x, str_y = y;
	uint32_t show_byte = 0; // 累计显示了多少字节，包括终止符
	
	while( *str != 0x00 )
	{
		uint8_t HB = *str;
		
		if (HB >= 0x81 && HB <= 0xFE)
		{
			// 中文
			// 边界检测
			if (str_x+8 > w+x)
			{
				str_y += 16;
				str_x = x;
			}
			if (str_y+16 > h+y) break;
			
			_LCD_ShowChinese(str_x, str_y, str);
			
			str_x += 16;
			str+=2;
			show_byte+=2;
		}
		else
		{
			// ASCII
			// 换行符
			if (*str == '\n')
			{
				str_y += 16;
				str_x = x;
				str+=1;
				show_byte++;
				continue;
			}
			
			// 边界检测
			if (str_x+8 > w+x)
			{
				str_y += 16;
				str_x = x;
			}
			if (str_y+16 > h+y) break;
			
			// 显示
			_LCD_ShowASCII(str_x, str_y, str);
			
			str_x += 8;
			str++;
			show_byte++;
		}
	}
//	show_byte++;
	
	return show_byte;
}

