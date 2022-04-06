# USART_Receive_Lib

## 1.CubeMX配置

- 始能USART1
- 波特率设置为115200，Word Length为8 Bits，无校验位，停止位为1
- 始能DMA -> USART1_RX，mode设置为Circular，priority看情况
- 始能NVIC中USART1 全局中断

## 2.代码使用

- 在main.c的初始化阶段调用USART1_Rx_Init()以初始化

- 根据接受数据大小与类型，在USART_Receive.c中设置SBUS_RX_BUF_NUM以确定接受数据长度

- 

- ```C
  void Received_Data_Dealer(const uint8_t *sbus_buf)
  ```

  该函数已经weak处理，请在其他地方重新声明编写，或直接修改USART_Receive.c中的该函数

- 如果对接受数据长度有要求，请对USART1_IRQHandler中的判断进行修改。