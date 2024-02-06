#ifndef __W25Q32_H
#define __W25Q32_H
#include "stm32f4xx.h"                  // Device header

/* FLASH 部分 begin **************************************************************************/

/* CS   -> PB8
 * SCK  -> PB3
 * MISO -> PB4
 * MOSI -> PB5
*/

/* FLASH 引脚定义 begin **********************************************************************/
#define FLASH_SPI                          SPI3
#define FLASH_SPI_CLK                      RCC_APB1Periph_SPI3
#define FLASH_SPI_CLK_INIT                 RCC_APB1PeriphClockCmd

#define FLASH_SPI_CS_PIN                   GPIO_Pin_8
#define FLASH_SPI_CS_GPIO_PORT             GPIOB
#define FLASH_SPI_CS_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define FLASH_SPI_CS_PINSOURCE             GPIO_PinSource8
#define FLASH_SPI_CS_AF                    GPIO_AF_SPI3

#define FLASH_SPI_SCK_PIN                  GPIO_Pin_3
#define FLASH_SPI_SCK_GPIO_PORT            GPIOB
#define FLASH_SPI_SCK_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define FLASH_SPI_SCK_PINSOURCE            GPIO_PinSource3
#define FLASH_SPI_SCK_AF                   GPIO_AF_SPI3

#define FLASH_SPI_MISO_PIN                 GPIO_Pin_4
#define FLASH_SPI_MISO_GPIO_PORT           GPIOB
#define FLASH_SPI_MISO_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define FLASH_SPI_MISO_PINSOURCE           GPIO_PinSource4
#define FLASH_SPI_MISO_AF                  GPIO_AF_SPI3

#define FLASH_SPI_MOSI_PIN                 GPIO_Pin_5
#define FLASH_SPI_MOSI_GPIO_PORT           GPIOB
#define FLASH_SPI_MOSI_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define FLASH_SPI_MOSI_PINSOURCE           GPIO_PinSource5
#define FLASH_SPI_MOSI_AF                  GPIO_AF_SPI3
/* FLASH 引脚定义 end ************************************************************************/

// CS 引脚
#define FLASH_W_CS(x) GPIO_WriteBit(FLASH_SPI_CS_GPIO_PORT, FLASH_SPI_CS_PIN, (BitAction)x)

void FLASH_SPI_Init(void);

/* SPI（无DMA） */
uint8_t FLASH_SPI_SwapByte(uint8_t data);

/* FLASH 部分 end ****************************************************************************/

#define CMD_UNIQUE_ID 0x4B       // 唯一标识
#define CMD_JEDEC_ID  0x9F       // 型号信息
#define CMD_WRITE_ENABLE 0x06    // 写使能
#define CMD_WRITE_DISENABLE 0x04 // 写失能
#define CMD_STATUS_REGISTER_1 0x05 // 状态寄存器1
#define CMD_STATUS_REGISTER_2 0x35 // 状态寄存器2
#define CMD_SECTOR_ERASE_4KB 0x20  // 擦除扇区
#define CMD_SECTOR_CHIP 0xC7       // 擦除整个芯片
#define CMD_PAGE_WRITE 0x02        // 写一页
#define CMD_READ 0x03              // 读数据

#define W25Q32_SPI_Start() FLASH_W_CS(0)
#define W25Q32_SPI_Stop()  FLASH_W_CS(1)

void W25Q32_Init(void);
uint32_t W25Q32_Read_JEDEC_ID(void);
uint8_t W25Q32_Read_SR(void);
void W25Q32_Wait_Busy(void);
void W25Q32_Write_Able(uint8_t status);
void W25Q32_Sector_Erase(uint32_t addr);
void W25Q32_Sector_Chip(void);
void W25Q32_Write_Page(uint32_t addr, uint8_t* data, uint32_t count);
void W25Q32_Write_Data(uint32_t addr, uint8_t* data, uint32_t count);
void W25Q32_Read_Data(uint32_t addr, uint8_t* data, uint32_t count);

#endif

