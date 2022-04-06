//
// Created by Trisoil on 2021/12/13.
//
#include "gpio.h"

#ifndef UNTITLED_CAN_OPERATION_H
#define UNTITLED_CAN_OPERATION_H

extern int16_t Now_Speed_Normal[5];
extern int16_t Now_Speed_M6020[5];
extern int16_t Now_Angle_Normal[5];
extern int16_t Now_Angle_M6020[5];
extern uint8_t CAN_Recieved_Message[8];

void CAN1_Filter_Init(void);
void CAN2_Filter_Init(void);
void CAN_CMD_Current(int16_t Current_A,int16_t Current_B,int16_t Current_C,int16_t Current_D);
void CAN_Transmit_Message(char tag, int Data_A,int Data_B, int Data_C,int Data_D,int StdID);
void CAN_CMD_Voltage(uint8_t ID_Num, int16_t Voltage);

void CAN_Recieved_Data_Dealer();

#endif //UNTITLED_CAN_OPERATION_H
