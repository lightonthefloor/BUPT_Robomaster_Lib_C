//
// Created by Trisoil on 2022/3/30.
//

#include "USART_Receive.h"
#include "usart.h"

// 设置数据接收长度
#define SBUS_RX_BUF_NUM 99u

uint8_t Locator_Rx_Data[2][SBUS_RX_BUF_NUM];

extern  DMA_HandleTypeDef hdma_usart1_rx;

void USART1_Receive_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num) {
	//enable the DMA transfer for the receiver request
	//使能DMA串口接收
	SET_BIT(huart1.Instance->CR3, USART_CR3_DMAR);

	//enalbe idle interrupt
	//使能空闲中断
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	//HAL_UART_Receive_IT()
	//disable DMA
	//失效DMA
	__HAL_DMA_DISABLE(&hdma_usart1_rx);
	while (hdma_usart1_rx.Instance->CR & DMA_SxCR_EN) {
		__HAL_DMA_DISABLE(&hdma_usart1_rx);
	}

	hdma_usart1_rx.Instance->PAR = (uint32_t) &(USART1->DR);
	//memory buffer 1
	//内存缓冲区1
	hdma_usart1_rx.Instance->M0AR = (uint32_t) (rx1_buf);
	//memory buffer 2
	//内存缓冲区2
	hdma_usart1_rx.Instance->M1AR = (uint32_t) (rx2_buf);
	//data length
	//数据长度
	hdma_usart1_rx.Instance->NDTR = dma_buf_num;
	//enable double memory buffer
	//使能双缓冲区
	SET_BIT(hdma_usart1_rx.Instance->CR, DMA_SxCR_DBM);

	//enable DMA
	//使能DMA
	__HAL_DMA_ENABLE(&hdma_usart1_rx);

}

void USART1_Rx_Init() {
	USART1_Receive_init(Locator_Rx_Data[0], Locator_Rx_Data[1], SBUS_RX_BUF_NUM);
}

__weak void Received_Data_Dealer(const uint8_t *sbus_buf)
{

}

void USART1_IRQHandler(void) {
	if (huart1.Instance->SR & UART_FLAG_RXNE)//接收到数据
	{
		__HAL_UART_CLEAR_PEFLAG(&huart1);

	} else if (USART1->SR & UART_FLAG_IDLE) {
		static uint16_t this_time_rx_len = 0;

		__HAL_UART_CLEAR_PEFLAG(&huart1);

		if ((hdma_usart1_rx.Instance->CR & DMA_SxCR_CT) == RESET) {
			/* Current memory buffer used is Memory 0 */

			//disable DMA
			//失效DMA
			__HAL_DMA_DISABLE(&hdma_usart1_rx);

			//get receive data length, length = set_data_length - remain_length
			//获取接收数据长度,长度 = 设定长度 - 剩余长度
			this_time_rx_len = SBUS_RX_BUF_NUM - hdma_usart1_rx.Instance->NDTR;

			//reset set_data_lenght
			//重新设定数据长度
			hdma_usart1_rx.Instance->NDTR = SBUS_RX_BUF_NUM;

			//set memory buffer 1
			//设定缓冲区1
			hdma_usart1_rx.Instance->CR |= DMA_SxCR_CT;

			//enable DMA
			//使能DMA
			__HAL_DMA_ENABLE(&hdma_usart1_rx);

			if (this_time_rx_len){
				Received_Data_Dealer(Locator_Rx_Data[0]);
			}
		} else {
			/* Current memory buffer used is Memory 1 */
			//disable DMA
			//失效DMA
			__HAL_DMA_DISABLE(&hdma_usart1_rx);

			//get receive data length, length = set_data_length - remain_length
			//获取接收数据长度,长度 = 设定长度 - 剩余长度
			this_time_rx_len = SBUS_RX_BUF_NUM - hdma_usart1_rx.Instance->NDTR;

			//reset set_data_length
			//重新设定数据长度
			hdma_usart1_rx.Instance->NDTR = SBUS_RX_BUF_NUM;

			//set memory buffer 0
			//设定缓冲区0
			DMA1_Stream1->CR &= ~(DMA_SxCR_CT);

			//enable DMA
			//使能DMA
			__HAL_DMA_ENABLE(&hdma_usart1_rx);

			if (this_time_rx_len){
				Received_Data_Dealer(Locator_Rx_Data[1]);
			}
		}
	}
}