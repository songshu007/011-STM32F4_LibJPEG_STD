#ifndef __W25Q32_H
#define __W25Q32_H
#include "stm32f4xx.h"                  // Device header

/* FLASH ���� begin **************************************************************************/

/* CS   -> PB8
 * SCK  -> PB3
 * MISO -> PB4
 * MOSI -> PB5
*/

/* FLASH ���Ŷ��� begin **********************************************************************/
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
/* FLASH ���Ŷ��� end ************************************************************************/

// CS ����
#define FLASH_W_CS(x) GPIO_WriteBit(FLASH_SPI_CS_GPIO_PORT, FLASH_SPI_CS_PIN, (BitAction)x)

void FLASH_SPI_Init(void);

/* SPI����DMA�� */
uint8_t FLASH_SPI_SwapByte(uint8_t data);

/* FLASH ���� end ****************************************************************************/

#define CMD_UNIQUE_ID 0x4B       // Ψһ��ʶ
#define CMD_JEDEC_ID  0x9F       // �ͺ���Ϣ
#define CMD_WRITE_ENABLE 0x06    // дʹ��
#define CMD_WRITE_DISENABLE 0x04 // дʧ��
#define CMD_STATUS_REGISTER_1 0x05 // ״̬�Ĵ���1
#define CMD_STATUS_REGISTER_2 0x35 // ״̬�Ĵ���2
#define CMD_SECTOR_ERASE_4KB 0x20  // ��������
#define CMD_SECTOR_CHIP 0xC7       // ��������оƬ
#define CMD_PAGE_WRITE 0x02        // дһҳ
#define CMD_READ 0x03              // ������

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

