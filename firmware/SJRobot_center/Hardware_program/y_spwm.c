

#include "y_spwm.h"
#include <stdio.h>

#include "robot.h"

#define  PWM_DEAD_ZONE   10  //PWM零位死区

static uint8_t   ch1_status = 0;           //输入捕获标志
static uint16_t  ch1_tim_up;               //捕获到上升沿时间
static uint16_t  ch1_tim_dowm;             //捕获到下降沿时间
static int32_t   ch1_tim_value = 1500;     //通道时间数值
static int16_t   ch1_value;                //通道数值

static uint8_t   ch2_status = 0;           //输入捕获标志
static uint16_t  ch2_tim_up;               //捕获到上升沿时间
static uint16_t  ch2_tim_dowm;             //捕获到下降沿时间
static int32_t   ch2_tim_value = 1500;     //通道数值
static int16_t   ch2_value; 

static uint8_t   ch3_status = 0;           //输入捕获标志
static uint16_t  ch3_tim_up;               //捕获到上升沿时间
static uint16_t  ch3_tim_dowm;             //捕获到下降沿时间
int32_t   ch3_tim_value = 1500;     //通道数值

static uint8_t   ch4_status = 0;           //输入捕获标志
static uint16_t  ch4_tim_up;               //捕获到上升沿时间
static uint16_t  ch4_tim_dowm;             //捕获到下降沿时间
static int32_t   ch4_tim_value = 1500;     //通道数值

static uint8_t   spwm_ctl_flag = 0;        //遥控器控制标志位


/**
  * @简  述  PWM解码初始化
  * @参  数  无
  * @返回值	 无
  */
void SPWM_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	
	//时钟使能    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	
	
	//复用功能配置
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM8);
	
	//配置IO口为复用功能-定时器通道
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;       //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	
    //初始化定时器  TIM8_Cap_Init(9999,168-1);  //高级定时器TIM8的时钟频率为168M    
    TIM_DeInit(TIM8);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 168-1;  //168分频，频率为1M  
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;    //
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	
	//初始化输入捕获通道
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	
	TIM_ICInitStructure.TIM_ICFilter = 0x06;	  
	TIM_ICInit(TIM8, &TIM_ICInitStructure); 
 
 	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	
	TIM_ICInitStructure.TIM_ICFilter = 0x06;	  
	TIM_ICInit(TIM8, &TIM_ICInitStructure); 
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	
	TIM_ICInitStructure.TIM_ICFilter = 0x06;	  
	TIM_ICInit(TIM8, &TIM_ICInitStructure); 
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	
	TIM_ICInitStructure.TIM_ICFilter = 0x06;	  
	TIM_ICInit(TIM8, &TIM_ICInitStructure); 
 
    //配置NVIC 
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器	 
 
 
    //使能捕获中断CC1IE,CC2IE,CC3IE,CC4IE
	TIM_ITConfig(TIM8, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,	ENABLE); 
	
	//高级定时器输出必须使能这句	
	TIM_CtrlPWMOutputs(TIM8,ENABLE); 	
	
	//使能定时器
	TIM_Cmd(TIM8, ENABLE);    	
}

/**
  * @简  述  定时器输入捕获中断
  * @参  数  无 
  * @返回值  无
  */
void TIM8_CC_IRQHandler(void)
{
	//通道1发生捕获事件
	if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET) 
	{
		//清除中断标志位
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC1); 
		
		//捕获到一个上升沿
		if (ch1_status == 0x00)
		{
			//记录此时的定时器计数值
			ch1_tim_up = TIM_GetCapture1(TIM8); 
	
			//标记已捕获到上升沿
			ch1_status = 0x01;
			
			//设置为下降沿捕获
			TIM_OC1PolarityConfig(TIM8, TIM_ICPolarity_Falling); 				
		}
		
		//捕获到下降沿
		else 
		{
			//记录此时的定时器计数值
			ch1_tim_dowm = TIM_GetCapture1(TIM8); 
			
			
			//计数器发生溢出
			if (ch1_tim_dowm < ch1_tim_up)
			{
				//计算高电平的时间
				ch1_tim_value = ch1_tim_dowm - ch1_tim_up + 0xFFFF;
			}
			else
			{
				//计算高电平的时间
				ch1_tim_value = ch1_tim_dowm - ch1_tim_up;	
			}
					
			//捕获完毕，执行动作		
			//printf("@ %d  \r\n",ch1_tim_value );
			
			//控制标志有效
			if(spwm_ctl_flag != 0)
			{
				//W方向运动控制	
				if(ch1_tim_value > (1500 + PWM_DEAD_ZONE))
					ch1_value = (int16_t)(ch1_tim_value - (1500 + PWM_DEAD_ZONE));
				else if(ch1_tim_value < (1500 - PWM_DEAD_ZONE))
					ch1_value = (int16_t)(ch1_tim_value - (1500 - PWM_DEAD_ZONE));	
				else
					ch1_value = 0;
				
				//如果是阿克曼机器人
				#if (ROBOT_TYPE == ROBOT_AKM)
					ax_akm_angle = (int16_t)( -3.2 * ch1_value);
				#else
					Vel.TG_IW = (int16_t)( -10.0 * ch1_value);
				#endif
			}
			
			//捕获标志清零
			ch1_status = 0; 
			
			//设置为上升沿捕获
			TIM_OC1PolarityConfig(TIM8, TIM_ICPolarity_Rising); 
		}
	}
	
	//通道2发生捕获事件
	if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET) 
	{
		//清除中断标志位
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC2); 
		
		//捕获到一个上升沿
		if (ch2_status == 0x00)
		{
			//记录此时的定时器计数值
			ch2_tim_up = TIM_GetCapture2(TIM8); 
	
			//标记已捕获到上升沿
			ch2_status = 0x01;
			
			//设置为下降沿捕获
			TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Falling); 				
		}
		
		//捕获到下降沿
		else 
		{
			//记录此时的定时器计数值
			ch2_tim_dowm = TIM_GetCapture2(TIM8); 
			
			
			//计数器发生溢出
			if (ch2_tim_dowm < ch2_tim_up)
			{
				//计算高电平的时间
				ch2_tim_value = ch2_tim_dowm - ch2_tim_up + 0xFFFF;
			}
			else
			{
				//计算高电平的时间
				ch2_tim_value = ch2_tim_dowm - ch2_tim_up;	
			}
					
			//捕获完毕，执行动作		
			printf("@ %d  \r\n",ch2_tim_value );
			
			//控制标志有效
			if(spwm_ctl_flag != 0)
			{
				//X方向运动控制
				if(ch2_tim_value > (1500 + PWM_DEAD_ZONE))
					ch2_value = (int16_t)(ch2_tim_value - (1500 + PWM_DEAD_ZONE));
				else if(ch2_tim_value < (1500 - PWM_DEAD_ZONE))
					ch2_value = (int16_t)(ch2_tim_value - (1500 - PWM_DEAD_ZONE));	
				else
					ch2_value = 0;
				
				Vel.TG_IX = 1.2 * ch2_value; 
			}
			
			//捕获标志清零
			ch2_status = 0; 
			
			//设置为上升沿捕获
			TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Rising); 
		}
	}
	
	//通道3发生捕获事件
	if (TIM_GetITStatus(TIM8, TIM_IT_CC3) != RESET) 
	{
		//清除中断标志位
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC3); 
		
		//捕获到一个上升沿
		if (ch3_status == 0x00)
		{
			//记录此时的定时器计数值
			ch3_tim_up = TIM_GetCapture3(TIM8); 
	
			//标记已捕获到上升沿
			ch3_status = 0x01;
			
			//设置为下降沿捕获
			TIM_OC3PolarityConfig(TIM8, TIM_ICPolarity_Falling); 				
		}
		
		//捕获到下降沿
		else 
		{
			//记录此时的定时器计数值
			ch3_tim_dowm = TIM_GetCapture3(TIM8); 
			
			
			//计数器发生溢出
			if (ch3_tim_dowm < ch3_tim_up)
			{
				//计算高电平的时间
				ch3_tim_value = ch3_tim_dowm - ch3_tim_up + 0xFFFF;
			}
			else
			{
				//计算高电平的时间
				ch3_tim_value = ch3_tim_dowm - ch3_tim_up;	
			}
					
			//捕获完毕，执行动作		
			//printf("@ %d  \r\n",ch3_tim_value );
			
			//捕获标志清零
			ch3_status = 0; 
			
			//设置为上升沿捕获
			TIM_OC3PolarityConfig(TIM8, TIM_ICPolarity_Rising); 
		}
	}
	
	//通道4发生捕获事件
	if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET) 
	{
		//清除中断标志位
		TIM_ClearITPendingBit(TIM8, TIM_IT_CC4); 
		
		//捕获到一个上升沿
		if (ch4_status == 0x00)
		{
			//记录此时的定时器计数值
			ch4_tim_up = TIM_GetCapture4(TIM8); 
	
			//标记已捕获到上升沿
			ch4_status = 0x01;
			
			//设置为下降沿捕获
			TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Falling); 				
		}
		
		//捕获到下降沿
		else 
		{
			//记录此时的定时器计数值
			ch4_tim_dowm = TIM_GetCapture4(TIM8); 
			
			
			//计数器发生溢出
			if (ch4_tim_dowm < ch4_tim_up)
			{
				//计算高电平的时间
				ch4_tim_value = ch4_tim_dowm - ch4_tim_up + 0xFFFF;
			}
			else
			{
				//计算高电平的时间
				ch4_tim_value = ch4_tim_dowm - ch4_tim_up;	
			}
					
			//捕获完毕，执行动作	
			//printf("@ %d  \r\n",ch4_tim_value );
			
			//通道4最高档，允许控制			
			if(ch4_tim_value > 1800)
			{
				spwm_ctl_flag = 1;
			}
			else
			{
				spwm_ctl_flag = 0;
			}
			
			//捕获标志清零
			ch4_status = 0; 
			
			//设置为上升沿捕获
			TIM_OC4PolarityConfig(TIM8, TIM_ICPolarity_Rising); 
		}
	}
	
	


}

/**************************************************************************
Function: TIM1 Update Interrupt
Input   : none
Output  : none
函数功能：定时器8更新中断
入口参数：无
返回  值：无 
**************************************************************************/
void TIM8_UP_TIM13_IRQHandler(void) 
{ 
	//Clear the interrupt flag bit
	//清除中断标志位 
  TIM8->SR&=~(1<<0);	    
}

/******************* (C) 版权 2022 XTARK **************************************/













