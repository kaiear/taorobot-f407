#ifndef __Y_ZX_UART_H__
#define __Y_ZX_UART_H__

#include "main.h"
#include "stm32f4xx.h"

#include "y_global.h"

#define readOnly(x)	x->CR1 |= 4;	x->CR1 &= 0xFFFFFFF7;		//串口x配置为只读，CR1->RE=1, CR1->TE=0
#define sendOnly(x)	x->CR1 |= 8;	x->CR1 &= 0xFFFFFFFB;		//串口x配置为只写，CR1->RE=0, CR1->TE=1


void USART3_Init(uint32_t baudrate);

void uart3_send_str(uint8_t *s);


#endif /* __APP_UART_SERVO_H__ */
