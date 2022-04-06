//
// Created by Trisoil on 2022/2/28.
//

#include "main.h"

#ifndef REMOTE_CONTROL_USART_PRINTF_H
#define REMOTE_CONTROL_USART_PRINTF_H

extern void Usart1_TX_DMA_Init(void);
extern void Usart_Printf(const char *fmt,...);
extern void usart1_tx_dma_enable(uint8_t *data, uint16_t len);

#endif //REMOTE_CONTROL_USART_PRINTF_H
