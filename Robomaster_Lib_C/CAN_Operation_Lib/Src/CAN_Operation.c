//
// Created by Trisoil on 2021/12/13.
//

#include "CAN_Operation.h"
#include "gpio.h"
#include "can.h"
#include "main.h"
#include "string.h"

// 初次使用请设置Board_ID

#define Board_ID 0x301

int16_t Now_Speed_Normal[5];
int16_t Now_Angle_Normal[5];
int16_t Now_Speed_M6020[5];
int16_t Now_Angle_M6020[5];
int16_t cnt = 0;
uint8_t Motor_Data_Box[8];
uint8_t CAN_Recieved_Message[8];
CAN_RxHeaderTypeDef Motor_Message_Box;
CAN_RxHeaderTypeDef Recieved_Message_Box;

void CAN_CMD_Current(int16_t Current_A,int16_t Current_B,int16_t Current_C,int16_t Current_D)
{
	uint32_t Mail_Box;
	CAN_TxHeaderTypeDef TX_Message;
	uint8_t Message_Box[8];
	memset(Message_Box,0,sizeof(Message_Box));
	TX_Message.DLC = 0x08;
	TX_Message.StdId = 0x200;
	TX_Message.IDE = CAN_ID_STD;
	TX_Message.RTR = CAN_RTR_DATA;
	Message_Box[0] = Current_A >> 8;
	Message_Box[1] = Current_A;
	Message_Box[2] = Current_B >> 8;
	Message_Box[3] = Current_B;
	Message_Box[4] = Current_C >> 8;
	Message_Box[5] = Current_C;
	Message_Box[6] = Current_D >> 8;
	Message_Box[7] = Current_D;
	HAL_CAN_AddTxMessage(&hcan1,&TX_Message,Message_Box,&Mail_Box);
}

void CAN_CMD_Voltage(uint8_t ID_Num, int16_t Voltage)
{
	uint32_t Mail_Box;
	CAN_TxHeaderTypeDef TX_Message;
	uint8_t Message_Box[8];
	memset(Message_Box,0,sizeof(Message_Box));
	TX_Message.DLC = 0x08;
	TX_Message.StdId = 0x1FF;
	TX_Message.IDE = CAN_ID_STD;
	TX_Message.RTR = CAN_RTR_DATA;
	Message_Box[2*ID_Num-2] = Voltage >> 8;
	Message_Box[2*ID_Num-1] = Voltage;
	HAL_CAN_AddTxMessage(&hcan1,&TX_Message,Message_Box,&Mail_Box);
}

void CAN_Transmit_Message(char tag, int Data_A,int Data_B, int Data_C,int Data_D,int StdID)
{
	uint32_t Mail_Box;
	uint8_t Message_Box[8];
	CAN_TxHeaderTypeDef Tx_Data;
	Tx_Data.DLC = 0x08;
	Tx_Data.StdId = StdID;
	Tx_Data.RTR = CAN_RTR_DATA;
	Tx_Data.IDE = CAN_ID_STD;
	Message_Box[0] = tag;
	Message_Box[1] = Data_A >> 8;
	Message_Box[2] = Data_A;
	Message_Box[3] = Data_B >> 8;
	Message_Box[4] = Data_B;
	Message_Box[5] = Data_C >> 8;
	Message_Box[6] = Data_C;
	Message_Box[7] = Data_D;
	HAL_CAN_AddTxMessage(&hcan2,&Tx_Data,Message_Box,&Mail_Box);
}

void CAN1_Filter_Init(void)
{
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* 启动CAN */
	if (HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
	/* 使能CAN的FIFO0接收通知（中断） */
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}

}

void CAN2_Filter_Init(void)
{
	/* CAN Filter for CAN1 */
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterBank = 14;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* 启动CAN */
	if (HAL_CAN_Start(&hcan2) != HAL_OK)
	{
		Error_Handler();
	}
	/* 使能CAN的FIFO0接收通知（中断） */
	if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}
}

__weak void CAN_Recieved_Data_Dealer()
{
	memset(CAN_Recieved_Message,0,sizeof(CAN_Recieved_Message));
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (hcan == &hcan1){
		HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&Motor_Message_Box,Motor_Data_Box);
		uint8_t ID = Motor_Message_Box.StdId - (0x204);
		if (ID <= 0){
			ID += 4;
			Now_Speed_Normal[ID] = (uint16_t)(Motor_Data_Box[2] << 8 | Motor_Data_Box[3]);
			Now_Angle_Normal[ID] = (uint16_t)(Motor_Data_Box[0] << 8 | Motor_Data_Box[1]);
		}else {
			Now_Speed_M6020[ID] = (uint16_t)(Motor_Data_Box[2] << 8 | Motor_Data_Box[3]);
			Now_Angle_M6020[ID] = (uint16_t)(Motor_Data_Box[0] << 8 | Motor_Data_Box[1]);
		}
	}else if (hcan == &hcan2){
		HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&Recieved_Message_Box,CAN_Recieved_Message);
		if (Recieved_Message_Box.StdId != Board_ID){
			HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
			return;
		}
		CAN_Recieved_Data_Dealer();
	}
	HAL_CAN_ActivateNotification(hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
}