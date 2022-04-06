# Can_Operation_Lib 使用说明

## 1. CubeMX 配置

- 时钟按照开发板C型正常配置
- 始能CAN1，CAN2（注意，对于开发板C型，CAN1对应的GPIO针脚为PD0与PD1）
- Prescaler设置为3
- Time Quanta in Bit Segment1 设置为10 Times
- Time Quanta in Bit Segment2 设置为3 Times
- NVIC中打开CAN1 RX0 interrupts与CAN2 RX0 interrupts

## 2. 代码使用说明

- 初次设置，请设置Board_ID，防止板间通讯冲突
- 本库中由于对M6020进行“弱支持”，即对于M3050与M2006电机的和仅支持四个，支持同时连接4个M6020。若想连接更多M3508或M2006电机，请对库进行相应的调整。
- 在main.c中初始化阶段，请调用CAN2_Filter_Init(void)与CAN1_Filter_Init(void)。另外，CAN1对应开发板与各个电机进行通讯，CAN2对应与各个开发板进行通讯。
- 对M3508或M2006电机进行发送电流，请调用CAN_CMD_Current()函数，并根据要求填写发送电流数据。注意，A，B，C，D分别对应设置ID为1，2，3，4的电机。
- 对M6020电机进行发送电压控制，请调用CAN_CMD_Voltage()函数，并根据要求填写发送的ID与电压值，发送电压值范围参见官方PDF手册。
- 对于板间通讯，发送数据请调用CAN_Transmit_Message（）函数，并注意根据要求填写相应数据。tag为标识符，用于识别命令；后面A，B，C，D为发送的四个数据，其中，A，B，C为大数据，即可传输int数据范围的数据，且对应至发送数组中，有高八位与低八位发送。解析样例代码如下：

```C
void CAN_Recieved_Data_Dealer()
{
	//HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET);
	HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin);
	char tag = CAN_Recieved_Message[0];
	if (tag == 'M'){
		for (int i=0;i<3;i++) Current[i] = (CAN_Recieved_Message[2*i+1]<<8)|CAN_Recieved_Message[2*i+2];
		CAN_CMD_Current(Current[0],0,0,0);
	}
}
```



- 本库对CAN中断回调函数进行了重写，分别识别相应的CAN口通讯。对于CAN2口板间通讯，接受完信息后会回调CAN_Recieved_Data_Dealer()函数。可在其他地方对该函数进行重定义，该Lib中将其标为了__weak，内容为清空接收到的数据。对于CAN1口通讯，该库中接受了电机相应的转速与转动角度位置的信息。若需接受更多的信息，如电机温度等，请对该函数进行改编
- 另，该库中所有接受信息所用数组均设为全局变量，请书写代码时避免重名，并可在其余任何地方调用并改写其数据。

## 3. P.S.

- 请务必注意相应的CubeMX配置。若无法发送数据，请注意针脚相应始能；若无法接受数据进行闭环控制，请注意有无开启RX0中断。
- 对于不同环境下的库调用在此不再说明，请根据自身运用环境进行调整。