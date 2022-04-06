//
// Created by Trisoil on 2022/3/30.
//
#include "main.h"

#ifndef LOCATION_MODULE_C_LOCATION_MODULE_RECIEVE_H
#define LOCATION_MODULE_C_LOCATION_MODULE_RECIEVE_H


extern uint8_t Locator_Rx_Data[2][99];

extern void USART1_Rx_Init();
extern void Received_Data_Dealer(const uint8_t *sbus_buf);

#endif //LOCATION_MODULE_C_LOCATION_MODULE_RECIEVE_H
