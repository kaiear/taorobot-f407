

#include "y_sbus.h"
#include <stdio.h>

#include "robot.h"

static uint8_t  uart_sbus_rx_con=0;       //接收计数器
static uint8_t  uart_sbus_rx_buf[40];     //接收缓冲，数据内容小于等于32Byte
static uint16_t sbus_buf[10];             //通道数据

/**
  * @简  述  SBUS串口初始化
  * @参  数  无
  * @返回值	 无
  */
void SBUS_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	/* 串口USART配置 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	//USART对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2, GPIO_AF_UART5); 

	//USART 端口配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	//USART参数配置
	USART_InitStructure.USART_BaudRate = 100000;    //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	USART_Init(UART5, &USART_InitStructure);
	
    //USART3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器	
	
	//开启串口接收中断
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	
		//USART使能
	USART_Cmd(UART5, ENABLE); 
	
}

/**
  * @简  述  UART 串口中断服务函数
  * @参  数  无 
  * @返回值  无
  */
void UART5_IRQHandler(void)
{
	uint8_t Res;
	
	//接收中断
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  
	{
		Res =USART_ReceiveData(UART5);	
		
		uart_sbus_rx_buf[uart_sbus_rx_con++] = Res;
		
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	}
	
    //总线空闲终端
	else if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)
	{
		//接收完成，恢复初始状态
		uart_sbus_rx_con = 0;
		
		//转换到对应通道
		sbus_buf[1] = ((int16_t)uart_sbus_rx_buf[ 1] >> 0 | ((int16_t)uart_sbus_rx_buf[ 2] << 8 )) & 0x07FF;
		sbus_buf[2] = ((int16_t)uart_sbus_rx_buf[ 2] >> 3 | ((int16_t)uart_sbus_rx_buf[ 3] << 5 )) & 0x07FF;
		sbus_buf[3] = ((int16_t)uart_sbus_rx_buf[ 3] >> 6 | ((int16_t)uart_sbus_rx_buf[ 4] << 2 )  | (int16_t)uart_sbus_rx_buf[ 5] << 10 ) & 0x07FF;
		sbus_buf[4] = ((int16_t)uart_sbus_rx_buf[ 5] >> 1 | ((int16_t)uart_sbus_rx_buf[ 6] << 7 )) & 0x07FF;
		sbus_buf[5] = ((int16_t)uart_sbus_rx_buf[ 6] >> 4 | ((int16_t)uart_sbus_rx_buf[ 7] << 4 )) & 0x07FF;
		sbus_buf[6] = ((int16_t)uart_sbus_rx_buf[ 7] >> 7 | ((int16_t)uart_sbus_rx_buf[ 8] << 1 )  | (int16_t)uart_sbus_rx_buf[9] <<  9 ) & 0x07FF;
		sbus_buf[7] = ((int16_t)uart_sbus_rx_buf[9] >> 2 | ((int16_t)uart_sbus_rx_buf[10] << 6 )) & 0x07FF;
		sbus_buf[8] = ((int16_t)uart_sbus_rx_buf[10] >> 5 | ((int16_t)uart_sbus_rx_buf[11] << 3 )) & 0x07FF;
		
		//控制逻辑，通道5拨杆到下方生效
		if(sbus_buf[5] > 1500)
		{
			//运动控制
			Vel.TG_IX = (int16_t)(  1.0 * (sbus_buf[2] - 992));
			Vel.TG_IY = (int16_t)( -0.8 * (sbus_buf[1] - 992));
			
			//如果是阿克曼机器人
			#if (ROBOT_TYPE == ROBOT_AKM)
				ax_akm_angle = (int16_t)( -0.6 * (sbus_buf[4] - 992));
			#else
				Vel.TG_IW = (int16_t)( -5.0 * (sbus_buf[4] - 992));
			#endif
		}
		
		USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	}
}

/******************* (C) 版权 2022 XTARK **************************************/













