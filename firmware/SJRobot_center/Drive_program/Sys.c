// ############################################################
//  FILE: GPIO_int.c
//  Created on: 2021年8月14日
//  Author: lee
// ############################################################
#include "main.h"

void SysTickConfig(void)
{
  /* Setup SysTick Timer for 1ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000)) //  1ms
  {
    /* Capture error */
    while (1)
      ;
  }
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void Delay_us(uint32_t t) // Delay_us
{
  uint32_t delay_count1, delay_count2;
  for (delay_count2 = 0; delay_count2 < t; delay_count2++)
    for (delay_count1 = 0; delay_count1 < 41; delay_count1++)
      ;
}

void Delay_ms(uint32_t t) // Delay_ms
{
  uint32_t delay_count1, delay_count2;
  for (delay_count2 = 0; delay_count2 < t; delay_count2++)
    for (delay_count1 = 0; delay_count1 < 41000; delay_count1++)
      ;
}

/* QSA */
static u32 systick_ms = 0;
u32 millis(void)
{
	return systick_ms;
}

void SysTick_Handler(void) // 1ms中断
{
  Timer_Task_Count();
	systick_ms++;
}
