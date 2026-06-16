#include "main.h"


static uint8_t uart2_tx_buf[40];

uint8_t ros_servo_data=0;


void UART2_Init(uint32_t baud)
{
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOD, &GPIO_InitStructure);

USART_InitStructure.USART_BaudRate = baud;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
USART_Init(USART2, &USART_InitStructure);

USART_Cmd(USART2, ENABLE);
USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
}


void USART2_IRQHandler(void)
{
uint8_t Res;

if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
{
Res = USART_ReceiveData(USART2);
TaoV2_OnByte(Res);
USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}
}


void UART2_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num)
{
uint8_t i, cnt;
uint8_t tx_checksum = 0;

if(len <= 39)
{
uart2_tx_buf[0] = 0xAA;
uart2_tx_buf[1] = 0x55;
for(i = 0; i < len; i++)
{
uart2_tx_buf[2+i] = *(pbuf+i);
}

cnt = 2+len;
for(i = 0; i < cnt; i++)
{
tx_checksum = tx_checksum + uart2_tx_buf[i];
}
uart2_tx_buf[i] = tx_checksum;
uart2_tx_buf[35] = 0x7D;

cnt = 4+len;
for(i = 0; i < cnt; i++)
{
USART_SendData(USART2, uart2_tx_buf[i]);
while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}
}
}
