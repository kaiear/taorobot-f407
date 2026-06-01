
#include "y_servo.h"

servo_t duoji_doing[DJ_NUM];

/**
 * @简  述  舵机接口初始化
 * @参  数  无
 * @返回值  无
 */
void SERVO_Init(void)
{
    u8 i = 0;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(SERVO0_GPIO_CLK | SERVO1_GPIO_CLK | SERVO2_GPIO_CLK | SERVO3_GPIO_CLK | SERVO4_GPIO_CLK | SERVO5_GPIO_CLK, ENABLE); /* 使能 舵机 端口时钟 */

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     /*复用*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    /*推挽输出*/
    GPIO_InitStructure.GPIO_Pin = SERVO0_PIN;         /*PF7*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      /*上拉*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /**/
    GPIO_Init(SERVO0_GPIO_PORT, &GPIO_InitStructure); /*初始化IO*/

    GPIO_InitStructure.GPIO_Pin = SERVO1_PIN;
    GPIO_Init(SERVO1_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SERVO2_PIN;
    GPIO_Init(SERVO2_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SERVO3_PIN;
    GPIO_Init(SERVO3_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SERVO4_PIN;
    GPIO_Init(SERVO4_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SERVO5_PIN;
    GPIO_Init(SERVO5_GPIO_PORT, &GPIO_InitStructure);

    for (i = 0; i < DJ_NUM; i++)
    {
        duoji_doing[i].aim = 1500;
        duoji_doing[i].cur = 1500;
        duoji_doing[i].inc = 0;
        duoji_doing[i].time = 5000;
    }
}

/***********************************************
    功能介绍：	设置舵机引脚电平
    函数参数1：	index 要设置的舵机引脚索引
    函数参数2：	level 要设置的舵机引脚电平，1为高，0为低
    返回值：无
 ***********************************************/
void servo_pin_set(u8 index, BitAction level)
{
    switch (index)
    {
    case 0:
        SERVO0_PIN_SET(level);
        break;
    case 1:
        SERVO1_PIN_SET(level);
        break;
    case 2:
        SERVO2_PIN_SET(level);
        break;
    case 3:
        SERVO3_PIN_SET(level);
        break;
    case 4:
        SERVO4_PIN_SET(level);
        break;
    case 5:
        SERVO5_PIN_SET(level);
        break;
    default:
        break;
    }
}

/***********************************************
    功能介绍：	设置舵机控制参数函数
    函数参数：	index 舵机编号 aim 执行目标 time 执行时间(如果aim 执行目标==0，视为舵机停止)
    返回值：		无
 ***********************************************/
void duoji_doing_set(u8 index, int aim, int time)
{
    /* 限制输入值大小 */
    if (index >= DJ_NUM)
        return;
		
		if(index == 3)/* 3号PWM舵机和总线舵机相反 */
		{
			aim = 3000-aim;
		}

    if (aim == 0)
    {
        duoji_doing[index].inc = 0;
        duoji_doing[index].aim = duoji_doing[index].cur;
        return;
    }

    if (aim > 2490)
        aim = 2490;
    else if (aim < 510)
        aim = 510;

    if (time > 10000)
        time = 10000;

    if (duoji_doing[index].cur == aim)
    {
        aim = aim + 0.0077;
    }

    if (time < 20) /* 执行时间太短，舵机直接以最快速度运动 */
    {
        duoji_doing[index].aim = aim;
        duoji_doing[index].cur = aim;
        duoji_doing[index].inc = 0;
    }
    else
    {
        duoji_doing[index].aim = aim;
        duoji_doing[index].time = time;
        duoji_doing[index].inc = (duoji_doing[index].aim - duoji_doing[index].cur) / (duoji_doing[index].time / 20.000);
    }
}

/* 设置舵机每次增加的偏移量 */
void servo_inc_offset(u8 index)
{
    uint16_t aim_temp;

    if (duoji_doing[index].inc != 0)
    {
        
        aim_temp = duoji_doing[index].aim;

        if (aim_temp > 2490)
        {
            aim_temp = 2490;
        }
        else if (aim_temp < 500)
        {
            aim_temp = 500;
        }

        if (abs_float((float)aim_temp - duoji_doing[index].cur) <= abs_float(duoji_doing[index].inc + duoji_doing[index].inc))
        {
            duoji_doing[index].cur = aim_temp;
            duoji_doing[index].inc = 0;
        }
        else
        {
            duoji_doing[index].cur += duoji_doing[index].inc;
        }
    }
}

// 通用定时器3中断初始化
// arr：自动重装值。
// psc：时钟预分频数
// 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=定时器工作频率,单位:Mhz
// 这里使用的是定时器3!
void TIM14_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); /// 使能TIM14时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStructure); // 初始化TIM14
    TIM_ARRPreloadConfig(TIM2, DISABLE);
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE); // 允许定时器3更新中断

    NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;              // 定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM14, ENABLE); // 使能定时器3
}

// 定时器3中断服务函数
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    static u8 flag = 0;
    static u8 duoji_index1 = 0;
    u16 temp;

    if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) // 溢出中断
    {
        /* 通过改变重装载值和舵机下标索引，每个舵机定时2500（2.5ms），执行8个舵机后完成一个周期20000（20ms） */
        if (duoji_index1 == 8)
        {
            duoji_index1 = 0;
        }

        if (flag == 0)
        {
            temp = ((u16)(duoji_doing[duoji_index1].cur));
            TIM14->ARR = temp; /* BUG  不能使用变量？ */
            servo_pin_set(duoji_index1, Bit_SET);
            servo_inc_offset(duoji_index1);
        }
        else
        {
            temp = 2500 - ((u16)(duoji_doing[duoji_index1].cur));
            TIM14->ARR = temp;
            servo_pin_set(duoji_index1, Bit_RESET);
            duoji_index1++;
        }
        flag = !flag;
    }
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update); // 清除中断标志位
}

/******************* (C) 版权 2022 XTARK **************************************/
