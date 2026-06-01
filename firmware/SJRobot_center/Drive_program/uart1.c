
#include "main.h"

/**
 * @简  述  UART 串口初始化
 * @参  数  baud： 波特率设置
 * @返回值	 无
 */
// 串口1，自动下载电路
void UART1_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* 串口 USART1配置 */
	// 打开GPIO和USART部件的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// USART1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	// USART1 端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART1参数配置
	USART_InitStructure.USART_BaudRate = baud; // 波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	// 使能 USART， 配置完毕
	USART_Cmd(USART1, ENABLE);

	// 规避第一个字符不能输出的BUG
	//	USART_ClearFlag(USART1, USART_FLAG_TC);
}


/***********************************************
	功能介绍：	串口1发送字节
	函数参数：	dat 发送的字节
	返回值：		无
 ***********************************************/
void uart1_send_byte(u8 dat)
{
	USART_SendData(USART1, dat);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
}

/***********************************************
	功能介绍：	串口1发送字符串
	函数参数：	*s 发送的字符串
	返回值：		无
 ***********************************************/
void uart1_send_str(u8 *s)
{
	while (*s)
	{
		uart1_send_byte(*s++);
	}
}

/**************************串口打印相关函数重定义********************************/
/**
 * @简  述  重定义putc函数（USART1）
 */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t)ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}

	return ch;
}

/**
 * @简  述  重定义getc函数（USART1）
 */
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{
	}

	return (int)USART_ReceiveData(USART1);
}

/******************* (C) 版权 2022  **************************************/
