
#ifndef __UART2_H
#define __UART2_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

extern uint8_t ros_servo_data;

//X-SOFT 接口函数
void    UART2_Init(uint32_t baud);  //UART 调试串口初始化
uint8_t UART2_GetData(uint8_t *pbuf);  //UART 获取接收的数据
void    UART2_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num);  //UART 发送数据（X-Protocol协议）

#endif 

/******************* (C) 版权 2022 XTARK **************************************/
