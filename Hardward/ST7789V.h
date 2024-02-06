#ifndef __ST7789V_H
#define __ST7789V_H
#include "stm32f4xx.h"                  // Device header

/* CS   -> PC5
 * SCK  -> PB10
 * MOSI -> PC3
 * MISO -> PC2
*/

/* LCD 引脚定义 begin **********************************************************************/
#define LCD_SPI                          SPI2
#define LCD_SPI_CLK                      RCC_APB1Periph_SPI2
#define LCD_SPI_CLK_INIT                 RCC_APB1PeriphClockCmd

#define LCD_SPI_CS_PIN                   GPIO_Pin_5
#define LCD_SPI_CS_GPIO_PORT             GPIOC
#define LCD_SPI_CS_GPIO_CLK              RCC_AHB1Periph_GPIOC
#define LCD_SPI_CS_PINSOURCE             GPIO_PinSource5
#define LCD_SPI_CS_AF                    GPIO_AF_SPI2

#define LCD_SPI_SCK_PIN                  GPIO_Pin_10
#define LCD_SPI_SCK_GPIO_PORT            GPIOB
#define LCD_SPI_SCK_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define LCD_SPI_SCK_PINSOURCE            GPIO_PinSource10
#define LCD_SPI_SCK_AF                   GPIO_AF_SPI2

#define LCD_SPI_MISO_PIN                 GPIO_Pin_2
#define LCD_SPI_MISO_GPIO_PORT           GPIOC
#define LCD_SPI_MISO_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define LCD_SPI_MISO_PINSOURCE           GPIO_PinSource2
#define LCD_SPI_MISO_AF                  GPIO_AF_SPI2

#define LCD_SPI_MOSI_PIN                 GPIO_Pin_3
#define LCD_SPI_MOSI_GPIO_PORT           GPIOC
#define LCD_SPI_MOSI_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define LCD_SPI_MOSI_PINSOURCE           GPIO_PinSource3
#define LCD_SPI_MOSI_AF                  GPIO_AF_SPI2

#define LCD_RST_PIN                      GPIO_Pin_4
#define LCD_RST_GPIO_PORT                GPIOC
#define LCD_RST_GPIO_CLK                 RCC_AHB1Periph_GPIOC

#define LCD_DC_PIN                       GPIO_Pin_0
#define LCD_DC_GPIO_PORT                 GPIOC
#define LCD_DC_GPIO_CLK                  RCC_AHB1Periph_GPIOC

#define LCD_BLK_PIN                      GPIO_Pin_9
#define LCD_BLK_GPIO_PORT                GPIOF
#define LCD_BLK_GPIO_CLK                 RCC_AHB1Periph_GPIOF

#define LCD_DMA                          DMA1
#define LCD_DMA_CLK                      RCC_AHB1Periph_DMA1
#define LCD_TX_DMA_CHANNEL               DMA_Channel_0
#define LCD_TX_DMA_STREAM                DMA1_Stream4
#define LCD_TX_DMA_IT_TCIF               DMA_IT_TCIF4
/* LCD 引脚定义 end ************************************************************************/

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色

#define LCD_W 240
#define LCD_H 280

#define HORIZONTAL 0   // 水平
#define VERTICAL   1   // 垂直

#define ST7789_RES_Set GPIO_WriteBit(LCD_RST_GPIO_PORT, LCD_RST_PIN, Bit_SET)
#define ST7789_RES_Clr GPIO_WriteBit(LCD_RST_GPIO_PORT, LCD_RST_PIN, Bit_RESET)

#define ST7789_DC_Set  GPIO_WriteBit(LCD_DC_GPIO_PORT, LCD_DC_PIN, Bit_SET)
#define ST7789_DC_Clr  GPIO_WriteBit(LCD_DC_GPIO_PORT, LCD_DC_PIN, Bit_RESET)

#define ST7789_CS_Set  GPIO_WriteBit(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_PIN, Bit_SET)
#define ST7789_CS_Clr  GPIO_WriteBit(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_PIN, Bit_RESET)

#define ST7789_BLK_Set  GPIO_WriteBit(LCD_BLK_GPIO_PORT, LCD_BLK_PIN, Bit_SET)
#define ST7789_BLK_Clr  GPIO_WriteBit(LCD_BLK_GPIO_PORT, LCD_BLK_PIN, Bit_RESET)

struct LCD_INFO
{
	uint16_t WIDTH;
	uint16_t HEIGHT;
	uint8_t Dir;
	uint16_t LCD_FONT_COLOR;
	uint16_t LCD_BACK_COLOR;
};
extern struct LCD_INFO LCD_Info;

struct FONT_INFO
{
	uint16_t def_Chinese_Font_Width;
	uint16_t def_Chinese_Font_Height;
	uint16_t def_English_Font_Width;
	uint16_t def_English_Font_Height;
};
extern struct FONT_INFO Font_Info;

/* SPI函数 begin *****************************************************************************/

/* SPI初始化 */
void LCD_SPI_Init(void);

/* SPI发送接收函数（无DMA） */
uint8_t LCD_SPI_SwapByte(uint8_t data);
uint16_t LCD_SPI_SwapHalfWord(uint16_t data);

/* SPI DMA函数 */
void LCD_SPI_DMATransmit(uint8_t* data, uint32_t bufsize, uint8_t datasize);
void LCD_SPI_DMARepeatTransmit(uint8_t* data, uint32_t repcount, uint8_t datasize);

/* SPI函数 end ******************************************************************************/


/* LCD初始化 */
void LCD_Init(void);

/* LCD功能函数 */
void LCD_SetAttributes(uint8_t Scan_dir);
void LCD_SetCursor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_SetFontColor(uint16_t color);
void LCD_SetBackGroundColor(uint16_t color);

/* LCD画图函数（无DMA） */
void LCD_Clear(uint16_t color);
void LCD_Fill(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void LCD_DrawBitmap(int x, int y, uint16_t w, uint16_t h, uint8_t* bmp);
void LCD_DrawPixel(int x, int y, uint16_t color);

/* LCD画图函数（DMA） */
void LCD_Fill_DMA(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_DrawBitmap_DMA(int x, int y, uint16_t w, uint16_t h, uint8_t* bmp);
uint32_t LCD_ShowString(int x, int y, int w, int h, uint8_t* str);

#endif
