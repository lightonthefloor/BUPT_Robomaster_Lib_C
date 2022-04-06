# USART_Printf_Lib 使用说明

## 1. CubeMX配置

- 始能USART1
- 波特率设置为115200
- Word Length设置为8 Bits
- 无校验位，停止符为1
- 始能DMA -> USART1_TX
- mode选择为Normal，priority自选
- NVIC中开启USART1全局中断

## 2.函数调用介绍

- 在main.c的初始化阶段，调用函数Usart1_TX_DMA_Init()
- 其余地方像使用Printf一样使用Usart_Printf一样即可

