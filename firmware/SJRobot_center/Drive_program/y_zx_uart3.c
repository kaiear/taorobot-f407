#include "y_zx_uart3.h"

/************************************************
函数名称 ： USART3_Send_ArrayU8
功    能 ： 总线舵机的串口设置
参    数 ： pData ---- 字符串
            Length --- 长度
返 回 值 ： 无
*************************************************/
/* QSA */
void USART3_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* 串口 USART1配置 */
    // 打开GPIO和USART部件的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // USART3对应引脚复用映射
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    // USART3 端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3参数配置
    USART_InitStructure.USART_BaudRate = baud; // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
	USART_HalfDuplexCmd(USART3, ENABLE);  	//注意这个，启动半双工模式

    // 使能 USART， 配置完毕
    USART_Cmd(USART3, ENABLE);

    // 规避第一个字符不能输出的BUG
    USART_ClearFlag(USART3, USART_FLAG_TC);
}
void USART3_Send_U8(uint8_t Data)
{
    USART_SendData(USART3, Data);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}


/***********************************************
	函数名称:	uart3_send_str()
	功能介绍:	串口3发送字符串
	函数参数:	*s 发送的字符串
	返回值:		无
 ***********************************************/
void uart3_send_str(uint8_t *s)
{
	while (*s)
	{
		USART3_Send_U8(*s++);
	}
}
