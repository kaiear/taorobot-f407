
#ifndef __CAN_H
#define __CAN_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//X-SOFT 接口函数
void CAN1_Mode_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode);  //CAN初始化
uint8_t CAN_SendMsg(uint8_t* msg,uint8_t num);    //CAN发送数据
uint8_t CAN_RecvMsg(uint8_t *msg);    //CAN接收数据

#endif

/******************* (C) 版权 2022 XTARK **************************************/
