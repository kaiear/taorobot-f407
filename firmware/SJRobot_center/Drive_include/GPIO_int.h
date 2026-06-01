//############################################################
// FILE: GPIO_int.c
// Created on: 2017年1月18日
// Author: lee
// summary: GPIO_int
//############################################################

#ifndef _GPIO_int_H
#define _GPIO_int_H
#include "Sys.h"     
 
//#define LED2_REV   GPIO_ToggleBits(GPIOB,GPIO_Pin_6)
void MOTOR_AB_Init(void); //电机PWM控制初始化
void MOTOR_CD_Init(void); //电机PWM控制初始化
void GPIO_LED_int(void);
void MOTOR_AB_Init(void);
void MOTOR_CD_Init(void);
void ENCODER_A_Init(void);
uint16_t ENCODER_A_GetCounter(void);
void ENCODER_A_SetCounter(uint16_t count);
void ENCODER_B_Init(void);
uint16_t ENCODER_B_GetCounter(void);
void ENCODER_B_SetCounter(uint16_t count);
void ENCODER_C_Init(void);
uint16_t ENCODER_C_GetCounter(void);
void ENCODER_C_SetCounter(uint16_t count);
void ENCODER_D_Init(void);
uint16_t ENCODER_D_GetCounter(void);
void ENCODER_D_SetCounter(uint16_t count);
void KEY_Init(void);
void VIN_Init(void);
uint16_t VIN_GetVol_X100(void);

uint32_t TIM_IsActiveFlag_UPDATE(TIM_TypeDef *TIMx);
uint32_t TIM_IsEnabledIT_UPDATE(TIM_TypeDef *TIMx);
uint32_t TIM_GetDirection(TIM_TypeDef *TIMx);

#endif   //  GPIO_int.h
//===========================================================================
// No more.
//===========================================================================
