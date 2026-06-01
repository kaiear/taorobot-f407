/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Y_MOTOR_H
#define __Y_MOTOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "main.h"

//X-SOFT 接口函数

//电机PID闭环速度控制函数
int16_t SPEED_PidCtlA(float spd_target, float spd_current);   //PID控制函数，电机A
int16_t SPEED_PidCtlB(float spd_target, float spd_current);    //PID控制函数，电机B
int16_t SPEED_PidCtlC(float spd_target, float spd_current);    //PID控制函数，电机C
int16_t SPEED_PidCtlD(float spd_target, float spd_current);    //PID控制函数，电机D

void MOTOR_A_SetSpeed(int16_t speed);   //电机A控制
void MOTOR_B_SetSpeed(int16_t speed);   //电机B控制

void MOTOR_C_SetSpeed(int16_t speed);   //电机C控制
void MOTOR_D_SetSpeed(int16_t speed);   //电机D控制

#endif

/******************* (C) 版权 2022 XTARK **************************************/
