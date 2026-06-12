
#include "y_servo.h"

servo_t duoji_doing[DJ_NUM];

/**
 * @��  ��  ����ӿڳ�ʼ��
 * @��  ��  ��
 * @����ֵ  ��
 */
void SERVO_Init(void)
{
    u8 i = 0;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(SERVO0_GPIO_CLK | SERVO1_GPIO_CLK | SERVO2_GPIO_CLK | SERVO3_GPIO_CLK | SERVO4_GPIO_CLK | SERVO5_GPIO_CLK, ENABLE); /* ʹ�� ��� �˿�ʱ�� */

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     /*����*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    /*�������*/
    GPIO_InitStructure.GPIO_Pin = SERVO0_PIN;         /*PF7*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      /*����*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /**/
    GPIO_Init(SERVO0_GPIO_PORT, &GPIO_InitStructure); /*��ʼ��IO*/

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
    ���ܽ��ܣ�	���ö�����ŵ�ƽ
    ��������1��	index Ҫ���õĶ����������
    ��������2��	level Ҫ���õĶ�����ŵ�ƽ��1Ϊ�ߣ�0Ϊ��
    ����ֵ����
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
    ���ܽ��ܣ�	���ö�����Ʋ�������
    ����������	index ������ aim ִ��Ŀ�� time ִ��ʱ��(���aim ִ��Ŀ��==0����Ϊ���ֹͣ)
    ����ֵ��		��
 ***********************************************/
void duoji_doing_set(u8 index, int aim, int time)
{
    /* ��������ֵ��С */
    if (index >= DJ_NUM)
        return;
		
		if(index == 3)/* 3��PWM��������߶���෴ */
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

    if (time < 20) /* ִ��ʱ��̫�̣����ֱ��������ٶ��˶� */
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

/* ���ö��ÿ�����ӵ�ƫ���� */
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
        else if (aim_temp < 510)
        {
            aim_temp = 510;
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

// ͨ�ö�ʱ��3�жϳ�ʼ��
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
// ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=��ʱ������Ƶ��,��λ:Mhz
// ����ʹ�õ��Ƕ�ʱ��3!
void TIM14_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); /// ʹ��TIM14ʱ��

    TIM_TimeBaseInitStructure.TIM_Period = arr;                     // �Զ���װ��ֵ
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  // ��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStructure); // ��ʼ��TIM14
    TIM_ARRPreloadConfig(TIM2, DISABLE);
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE); // ������ʱ��3�����ж�

    NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;              // ��ʱ��3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // ��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;        // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM14, ENABLE); // ʹ�ܶ�ʱ��3
}

// ��ʱ��3�жϷ�����
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    static u8 flag = 0;
    static u8 duoji_index1 = 0;
    u16 temp;

    if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) // ����ж�
    {
        /* ͨ���ı���װ��ֵ�Ͷ���±�������ÿ�������ʱ2500��2.5ms����ִ��8����������һ������20000��20ms�� */
        if (duoji_index1 == 8)
        {
            duoji_index1 = 0;
        }

        if (flag == 0)
        {
            temp = ((u16)(duoji_doing[duoji_index1].cur));
            TIM14->ARR = temp; /* BUG  ����ʹ�ñ����� */
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
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update); // ����жϱ�־λ
}

/******************* (C) ��Ȩ 2022 XTARK **************************************/
