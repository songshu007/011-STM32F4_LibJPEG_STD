#include "stm32f4xx.h"
#include "Delay.h"
#include "sram.h"
#include "decode.h"
#include "jpg.h"
#include "ST7789V.h"
#include "W25Q32.h"

/* 2024-2-6
 * ���ߣ�SongShu007
 * ˵����LibJPEG���뱣���ڵ�Ƭ���ڲ�Flash��jpgͼ��
*/

void System_ClockInit(void);
void HardFault_Handler(void);

uint8_t _aucLine[4096];

#define IMAGE_WIDTH  280
#define IMAGE_HEIGHT 240

typedef struct RGB
{
  uint8_t B;
  uint8_t G;
  uint8_t R;
}RGB_typedef;
RGB_typedef *RGB_matrix;

unsigned short int rgb888Torgb565(unsigned int rgb888Pixel)
{
    int red   = (rgb888Pixel >> 16) & 0xff;
    int green = (rgb888Pixel >> 8 ) & 0xff;
    int blue  =  rgb888Pixel        & 0xff;

    unsigned short  b =   (blue  >> 3) & 0x001f;
    unsigned short  g = ( (green >> 2) & 0x003f ) << 5;
    unsigned short  r = ( (red   >> 3) & 0x001f ) << 11;

    return (unsigned short int) (r | g | b);
}

int pixelx = 0, pixely = 0;
/* 
 * JPEG���ݵĻص�������ÿ����һ�����ص�ͻ���øú���
*/
static uint8_t Jpeg_CallbackFunction(uint8_t* Row, uint32_t DataLength)
{
    RGB_matrix =  (RGB_typedef*)Row;
    uint32_t  ARGB32Buffer[IMAGE_WIDTH];
    uint32_t counter = 0;
	
    for(counter = 0; counter < IMAGE_WIDTH; counter++)
    {
        ARGB32Buffer[counter]  = (uint32_t)
        (
          ((RGB_matrix[counter].B << 16)|
           (RGB_matrix[counter].G << 8)|
           (RGB_matrix[counter].R) | 0xFF000000)
        );

		// ����
        LCD_DrawPixel( pixelx, pixely, rgb888Torgb565(ARGB32Buffer[counter]) );
		pixelx++;
		if (pixelx >= IMAGE_WIDTH) { pixelx = 0; pixely++; }
    }
	
	return 0;
}

int main(void)
{
	System_ClockInit();
	
	// 2λ��ռ���ȼ�2λ�����ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	LCD_Init();
	
	LCD_SetAttributes(HORIZONTAL);
	LCD_Fill_DMA(0, 0, 280, 240, BLACK);
	
	while(1)
	{
		jpeg_decode((uint8_t*)data, sizeof(data), IMAGE_WIDTH, _aucLine, Jpeg_CallbackFunction);
		pixelx = 0; pixely = 0;
		Delay_ms(1000);
		LCD_Fill_DMA(0, 0, 280, 240, BLACK);
		
	    jpeg_decode((uint8_t*)data2, sizeof(data2), IMAGE_WIDTH, _aucLine, Jpeg_CallbackFunction);
		pixelx = 0; pixely = 0;
		Delay_ms(1000);
		LCD_Fill_DMA(0, 0, 280, 240, BLACK);
	}
}

void System_ClockInit(void)
{
	uint32_t Status;
	
	RCC_DeInit();                     // ��λʱ��
	RCC_HSEConfig(RCC_HSE_ON);        // ʹ���ⲿʱ��
	Status = RCC_WaitForHSEStartUp(); // �ȴ��ⲿʱ���ȶ�
	
	if (Status == SUCCESS)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1); // AHBԤ��Ƶ����
		RCC_PCLK1Config(RCC_HCLK_Div2);  // APB1Ԥ��Ƶ����
		RCC_PCLK2Config(RCC_HCLK_Div2);  // APB2Ԥ��Ƶ����
		
		RCC_PLLConfig(RCC_PLLSource_HSE, 4, 168, 2, 4);      // ��Ƶ168M

		RCC_PLLCmd(ENABLE);                                  // ����PLL
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // �ȴ��ȶ�
		
		FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
	
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);           // ��PLLʱ��ѡΪϵͳʱ��
		while (RCC_GetSYSCLKSource() != 0x08);               // �ȴ�
	}
	else
	{
		// ʧ��
		HardFault_Handler();
	}
}
