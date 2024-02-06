/**
  ******************************************************************************
  * @file    LibJPEG/LibJPEG_Decoding/Inc/decode.h 
  * @author  MCD Application Team
  * @brief   header of decode.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DECODE_H
#define __DECODE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "jpeglib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
 * 解码JPEG
 * inbuf：JPEG所在的内存，需要整个大小
 * insize：JPEG占用的字节
 * width：JPEG的宽
 * buff：
 * callback：回调函数
*******************************************************************************/ 
void jpeg_decode(uint8_t* inbuf, unsigned long insize, uint32_t width, uint8_t * buff, uint8_t (*callback)(uint8_t*, uint32_t));

#endif /* __DECODE_H */
