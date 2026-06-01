/**
 ******************************************************************************
 * @file    usb_bsp.c
 * @author  MCD Application Team
 * @version V1.2.1
 * @date    17-March-2018
 * @brief   This file is responsible to offer board support package and is
 *          configurable by user.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                      <http://www.st.com/SLA0044>
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------ */
#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"

extern USB_OTG_CORE_HANDLE USB_OTG_Core_dev;
extern USBH_HOST USB_Host;

// USB OTG 中断服务函数
// 处理所有USB中断
void OTG_FS_IRQHandler(void)
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
}

// USB OTG 底层IO初始化
// pdev:USB OTG内核结构体指针
void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // 使能GPIOA时钟
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE); // 使能USB OTG时钟	钟
  // GPIOA11,A12设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; // PA11/12复用功能输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             // 复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           // 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;       // 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG_FS); // PA11,AF10(USB)
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG_FS); // PA12,AF10(USB)

}

/**
 * @brief  USB_OTG_BSP_EnableInterrupt
 *         Enable USB Global interrupt
 * @param  None
 * @retval None
 */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; // 抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		 // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 // 使能通道
	NVIC_Init(&NVIC_InitStructure);								 // 配置
}

//USB OTG 中断设置,开启USB FS中断
//pdev:USB OTG内核结构体指针
void USB_OTG_BSP_DisableInterrupt(void)
{ 
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE * pdev, uint8_t state)
{
}
/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE * pdev)
{
}


/**
* @brief  USB_OTG_BSP_uDelay
*         This function provides delay time in micro sec
* @param  usec : Value of delay required in micro sec
* @retval None
*/
/* This value is set for SYSCLK = 120 MHZ, User can adjust this value
* depending on used SYSCLK frequency */
//#define count_us 40
/* This value is set for SYSCLK = 168 MHZ, User can adjust this value
* depending on used SYSCLK frequency */
#define count_us 55
/* This value is set for SYSCLK = 72 MHZ, User can adjust this value
* depending on used SYSCLK frequency */
//#define count_us 12
void USB_OTG_BSP_uDelay(const uint32_t usec)
{
 uint32_t count = count_us * usec;
 do
 {
   if (--count == 0)
   {
     return;
   }
 } while (1);
}

/**
* @brief  USB_OTG_BSP_mDelay
*          This function provides delay time in milli sec
* @param  msec : Value of delay required in milli sec
* @retval None
*/
void USB_OTG_BSP_mDelay(const uint32_t msec)
{
 USB_OTG_BSP_uDelay(msec * 1000);
}




/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
