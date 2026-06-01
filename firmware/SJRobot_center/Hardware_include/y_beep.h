

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BEEP_H
#define __BEEP_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"
#include "main.h"

// 接口函数
void BEEP_Init(void);

// 蜂鸣器操作函数宏定义
#define BEEP_Off()       GPIO_ResetBits(GPIOC, GPIO_Pin_13)       //蜂鸣器不鸣
#define BEEP_On()		GPIO_SetBits(GPIOC, GPIO_Pin_13)     //蜂鸣器鸣叫
#define BEEP_Toggle()    GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction) (1 - GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)))    //蜂鸣器状态翻转

void beep_on_times(int times, int delay);

#endif 

/******************* (C) 版权 2022 XTARK **************************************/

