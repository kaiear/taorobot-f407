//############################################################
// FILE: Common_Math.h
// Created on: 2019年7月10日
// Author: lee
// summary:    Header file  and definition
//############################################################

#ifndef _Common_Math_H_
#define _Common_Math_H_

#define Abs(A)    ((A>=0)?A:-A)  // 绝对值函数
#define Min(A,B)  ((A<=B)?A:B)   // 求最小函数
#define Max(A,B)  ((A>=B)?A:B)   // 求最大函数

#include "stdint.h"

typedef struct
{
	uint16_t  table_Angle;
	float table_Sin;
	float table_Cos;
}Ang_SinCos, *p_Ang_SinCos;

#define  Ang_SinCos_DEFAULTS    {0,0.0,0.0}

typedef struct 	{
	        float    Alpha; 	//二相静止坐标系 Alpha 轴
	        float    Beta;		//二相静止坐标系 Beta 轴
	        float    IQTan;		//IQ格式正切 45度正切是1，IQ的格式是
  			  int16_t  IQAngle;	//IQ格式角度值 0---65536 == 0---360度
         } IQAtan , *p_IQAtan;

#define IQAtan_DEFAULTS  {0.0,0.0,0.0,0}  // 初始化参数

typedef struct
 {  //指令参数的斜率处理
  float  XieLv_X;   // 指令参数斜率输入变量x
	float  XieLv_Y;
	float  XieLv_Grad;
	uint16_t    Timer_Count;
	uint16_t    Grad_Timer;
 }GXieLv, *p_GXieLv;

 #define  GXieLv_DEFAULTS    {0.0,0.0,0.0,0,0}


void LookUp_CosSin(void);
float  Limit_Sat( float Uint,float U_max, float U_min); //限制赋值函数
uint32_t   IQSqrt(uint32_t  M);
void Atan_Cale(p_IQAtan pV);  // 求取求反正弦函数
void SinCos_Table(p_Ang_SinCos PV);
void Grad_XieLv(p_GXieLv pV);

#endif /* SIN_COS_TABLE1_H_ */
