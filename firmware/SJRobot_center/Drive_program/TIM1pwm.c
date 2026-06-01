
//############################################################
// FILE:  TIM1PWM.c
// Created on: 2019年7月5日
// Author: lee
// summary: TIM1_PWM_Init
//############################################################

#include "main.h"

void TIM1_PWM_Init(void)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef    TIM_OCInitStructure;	
  TIM_BDTRInitTypeDef    TIM1_BDTRInitStructure;
	NVIC_InitTypeDef    NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//开启TIM1时钟
 
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;  // 互补中心对称
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //分频系数 Timer clock = sysclock /(TIM_Prescaler+1) = 168M
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = REP_RATE;
	TIM_TimeBaseInitStructure.TIM_Period = PWM_PERIOD;    //设置周期
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);
		
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // pwm模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //使能CHx
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//使能CHxN
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_High; //高有效
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;// TIM_OCNPolarity_High 
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCNIdleState_Reset;//空闲时复位
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

	TIM_OC1Init(TIM1,&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);

  TIM1_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM1_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM1_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 

  TIM1_BDTRInitStructure.TIM_DeadTime = DEADTIME;  //死区时间
  TIM1_BDTRInitStructure.TIM_Break =TIM_Break_Enable;  // 过流立即停车，封锁PWM  TIM_Break_Disable TIM_Break_Enable
  TIM1_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;// 
  TIM1_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;  

  TIM_BDTRConfig(TIM1, &TIM1_BDTRInitStructure);

	TIM_ClearITPendingBit(TIM1, TIM_IT_Break);  //清中断标志位
  TIM_ITConfig(TIM1,TIM_IT_Break ,ENABLE); //使能中断 
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);  
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清中断标志位
  TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); //打开中断 
 
 	TIM_Cmd(TIM1,DISABLE);//计数开始
	TIM_CtrlPWMOutputs(TIM1,DISABLE);	
 
/* Configure one bit for preemption priority */
	//选择优先级分组
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//配置TIM1的刹车中断使能
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn; // TIM1_BRK_TIM9_IRQHandler
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//指定抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//指定抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
	//配置TIM1的更新中断使能
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;  // TIM1_UP_TIM10_IRQHandler
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//指定抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//指定响应优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Start_PWM(void)  // 启动函数
{
   //使能PWM输出通道OC1/OC1N/OC2/OC2N/OC3/OC3N
    TIM1->CCER|=0x5555;	
		TIM_CtrlPWMOutputs(TIM1,ENABLE);
}
 
void Stop_PWM(void)  // 停止函数
{
   //不使能PWM输出通道OC1/OC1N/OC2/OC2N/OC3/OC3N
	 TIM1->CCR1=0;
   TIM1->CCR2=0;
   TIM1->CCR3=0;
   TIM1->CCER&=0xAAAA;	
}

