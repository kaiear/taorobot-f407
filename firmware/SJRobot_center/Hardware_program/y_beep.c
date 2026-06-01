#include "y_beep.h"

/**
  * @简  述  初始化
  * @参  数  无
  * @返回值  无
  */
void BEEP_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//GPIO配置 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	
	//配置GPIO  推挽输出模式 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//关闭蜂鸣器
	GPIO_ResetBits(GPIOC,  GPIO_Pin_13);	

}	

/**
 * @函数描述: 蜂鸣器鸣叫时间，单位ms
 * @param {uint16_t} times
 * @return {*}
 */
void beep_on_times(int times, int delay)
{
    int i;
    for (i = 0; i < times; i++)
    {
        BEEP_On();
        Delay_ms(delay);
        BEEP_Off();
        Delay_ms(delay);
    }
}

/******************* (C) 版权 2022  **************************************/
