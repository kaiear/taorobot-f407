

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART1_H
#define __UART1_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

void UART1_Init(uint32_t baud);  
void uart1_send_str(u8 *s);
#endif 

/******************* (C) ��Ȩ 2019 XTARK **************************************/
