
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Y_SWITCH_H
#define __Y_SWITCH_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"
#include "main.h"

uint8_t KEY_Scan(void);
//X-SOFT 接口函数
void AX_SW_Init(void);       //拨码开关初始化
uint8_t AX_SW_GetS1(void);   //获取开关1状态
uint8_t AX_SW_GetS2(void);   //获取开关2状态
uint8_t AX_SW_GetS12(void);  //获取开关总状态

#endif 

/******************* (C) 版权 2022 XTARK **************************************/
