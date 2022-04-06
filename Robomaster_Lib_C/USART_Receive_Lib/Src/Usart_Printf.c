//
// Created by Trisoil on 2022/2/28.
//

#include "Usart_Printf.h"
#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "stdarg.h"

extern DMA_HandleTypeDef hdma_usart1_tx;

void Usart1_TX_DMA_Init(void)
{
  //enable the DMA transfer for the receiver request
  //使能DMA串口接收
  SET_BIT(huart1.Instance->CR3, USART_CR3_DMAT);
}

void usart1_tx_dma_enable(uint8_t *data, uint16_t len)
{

  //disable DMA
  //失效DMA
  __HAL_DMA_DISABLE(&hdma_usart1_tx);
  while(hdma_usart1_tx.Instance->CR & DMA_SxCR_EN)
  {
    __HAL_DMA_DISABLE(&hdma_usart1_tx);
  }
	hdma_usart1_tx.Instance->PAR = (uint32_t) & (USART1->DR);
  //clear flag
  //清除标志位
  __HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_HISR_TCIF7);
  __HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_HISR_HTIF7);

  //set data address
  //设置数据地址
  hdma_usart1_tx.Instance->M0AR = (uint32_t)(data);
  //set data length
  //设置数据长度
  hdma_usart1_tx.Instance->NDTR = len;

	SET_BIT(huart1.Instance->CR3, USART_CR3_DMAT);
  //enable DMA
  //使能DMA
  __HAL_DMA_ENABLE(&hdma_usart1_tx);
}

void Usart_Printf(const char *fmt,...)
{
  static uint8_t tx_buf[256] = {0};
  static va_list ap;
  static uint16_t len;
  va_start(ap, fmt);

  //return length of string
  //返回字符串长度
  len = vsprintf((char *)tx_buf, fmt, ap);

  va_end(ap);

	usart1_tx_dma_enable(tx_buf,len);
}



