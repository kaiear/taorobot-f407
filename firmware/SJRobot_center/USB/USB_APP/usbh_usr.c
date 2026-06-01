/**
 ******************************************************************************
 * @file    usbh_usr.c
 * @author  MCD Application Team
 * @version V2.2.1
 * @date    17-March-2018
 * @brief   This file includes the user application layer
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
#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include "usb_hcd_int.h"
// 表示USB连接状态
// 0,没有连接;
// 1,已经连接;
vu8 bDeviceState = 0; // 默认没有连接

u8 USB_FIRST_PLUGIN_FLAG = 0; // USB第一次插入标志,如果为1,说明是第一次插入


/* Points to the DEVICE_PROP structure of current device */
/* The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_Callbacks = {
    USBH_USR_Init,
    USBH_USR_DeInit,
    USBH_USR_DeviceAttached,
    USBH_USR_ResetDevice,
    USBH_USR_DeviceDisconnected,
    USBH_USR_OverCurrentDetected,
    USBH_USR_DeviceSpeedDetected,
    USBH_USR_Device_DescAvailable,
    USBH_USR_DeviceAddressAssigned,
    USBH_USR_Configuration_DescAvailable,
    USBH_USR_Manufacturer_String,
    USBH_USR_Product_String,
    USBH_USR_SerialNum_String,
    USBH_USR_EnumerationDone,
    USBH_USR_UserInput,
    0,
    USBH_USR_DeviceNotSupported,
    USBH_USR_UnrecoveredError};

/**
 * @brief  USBH_USR_Init
 *         Displays the message on LCD for host lib initialization
 * @param  None
 * @retval None
 */
void USBH_USR_Init(void)
{

  printf("> USB Host library started.\n");
  printf("     USB Host Library v2.2.1");
}

/**
 * @brief  USBH_USR_DeviceAttached
 *         Displays the message on LCD on device attached
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAttached(void)
{
  printf("USB device is detected inserted!\r\n");
}

/**
 * @brief  USBH_USR_UnrecoveredError
 * @param  None
 * @retval None
 */
void USBH_USR_UnrecoveredError(void)
{
  printf("Unrecoverable error!!!\r\n\r\n");
}

/**
 * @brief  USBH_DisconnectEvent
 *         Device disconnect event
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceDisconnected(void)
{
  printf("USB device unplugged!\r\n");
  bDeviceState = 0; // USB设备拔出了
}

/**
 * @brief  USBH_USR_ResetUSBDevice
 *         Reset USB Device
 * @param  None
 * @retval None
 */
void USBH_USR_ResetDevice(void)
{
  printf("USB RESET...\r\n");
}

/**
 * @brief  USBH_USR_DeviceSpeedDetected
 *         Displays the message on LCD for device speed
 * @param  Devicespeed : Device Speed
 * @retval None
 */
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if (DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    printf("High speed (HS)USB devices!\r\n");
  }
  else if (DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    printf("Full speed (FS)USB device!\r\n");
  }
  else if (DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
    printf("Low speed (LS)USB device!\r\n");
  }
  else
  {
    printf("Equipment error!\r\n");
  }
}

/**
 * @brief  USBH_USR_Device_DescAvailable
 *         Displays the message on LCD for device descriptor
 * @param  DeviceDesc : device descriptor
 * @retval None
 */
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;
  printf("VID: %04Xh\r\n", (uint32_t)(*hs).idVendor);
  printf("PID: %04Xh\r\n", (uint32_t)(*hs).idProduct);
}

/**
 * @brief  USBH_USR_DeviceAddressAssigned
 *         USB device is successfully assigned the Address
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceAddressAssigned(void)
{
  printf("Slave address allocation succeeded!\r\n");
}

/**
 * @brief  USBH_USR_Conf_Desc
 *         Displays the message on LCD for configuration descriptor
 * @param  ConfDesc : Configuration descriptor
 * @retval None
 */
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef *cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  id = itfDesc;
  if ((*id).bInterfaceClass == 0x08)
  {
    printf("Removable memory device!\r\n");
  }
  else if ((*id).bInterfaceClass == 0x03)
  {
    printf("HID device!\r\n");
  }
}

/**
 * @brief  USBH_USR_Manufacturer_String
 *         Displays the message on LCD for Manufacturer String
 * @param  ManufacturerString : Manufacturer String of Device
 * @retval None
 */
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  printf("Manufacturer: %s\r\n", (char *)ManufacturerString);
}

/**
 * @brief  USBH_USR_Product_String
 *         Displays the message on LCD for Product String
 * @param  ProductString : Product String of Device
 * @retval None
 */
void USBH_USR_Product_String(void *ProductString)
{
  printf("Product: %s\r\n", (char *)ProductString);
}

/**
 * @brief  USBH_USR_SerialNum_String
 *         Displays the message on LCD for SerialNum_String
 * @param  SerialNumString : SerialNum_String of device
 * @retval None
 */
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  printf("Serial Number: %s\r\n", (char *)SerialNumString);
}

/**
 * @brief  EnumerationDone
 *         User response request is displayed to ask for
 *         application jump to class
 * @param  None
 * @retval None
 */
void USBH_USR_EnumerationDone(void)
{
  /* Enumeration complete */
  printf("Device enumeration is complete!\r\n\r\n");
}

/**
 * @brief  USBH_USR_DeviceNotSupported
 *         Device is not supported
 * @param  None
 * @retval None
 */
void USBH_USR_DeviceNotSupported(void)
{
  printf("> Device not supported.\n");
}

/**
 * @brief  USBH_USR_UserInput
 *         User Action for application state entry
 * @param  None
 * @retval USBH_USR_Status : User response for key button
 */
USBH_USR_Status USBH_USR_UserInput(void)
{

  printf("Skip the user confirmation step!\r\n");
  bDeviceState = 1; // USB设备已经连接成功
  return USBH_USR_RESP_OK;
}

/**
 * @brief  USBH_USR_OverCurrentDetected
 *         Device Overcurrent detection event
 * @param  None
 * @retval None
 */
void USBH_USR_OverCurrentDetected(void)
{
  printf("Overcurrent detected.\n");
}

/**
 * @brief  USR_MOUSE_Init
 *         Init Mouse window
 * @param  None
 * @retval None
 */
void USR_MOUSE_Init(void)
{
  printf("reinitialize!!!\r\n");
}

/**
 * @brief  USR_MOUSE_ProcessData
 *         Process Mouse data
 * @param  data : Mouse data to be displayed
 * @retval None
 */
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{
}

/**
 * @brief  USR_KEYBRD_Init
 *         Init Keyboard window
 * @param  None
 * @retval None
 */
void USR_KEYBRD_Init(void)
{

  printf("> Use Keyboard to tape characters: \n\n");
  printf("\n\n\n\n\n\n");
}

/**
 * @brief  USR_KEYBRD_ProcessData
 *         Process Keyboard data
 * @param  data : Keyboard data to be displayed
 * @retval None
 */
void USR_KEYBRD_ProcessData(uint8_t data)
{
   u8 buf[4];
   sprintf((char *)buf, "%02X", data);
   printf("usb buf= %s", buf); // 显示键值
}

/**
 * @brief  USBH_USR_DeInit
 *         Deinit User state and associated variables
 * @param  None
 * @retval None
 */
void USBH_USR_DeInit(void)
{
  printf("reinitialize!!!\r\n");
}

//游戏手柄初始化
void USR_GAMEPAD_Init(void)
{
	printf("GAMEPAD discover\r\n");
}
void  USR_GAMEPAD_ProcessData(uint8_t data)
{
//	u8 i;
//	if(data)
//	{
//		POINT_COLOR=BLUE;
//		LCD_ShowNum(30+56,180,data,3,16);//显示键值
//		for(i=0;i<8;i++)
//		{
//			if(data&(1<<i))
//			{
//				LCD_Fill(30+56,200,30+56+48,200+16,WHITE);//清除之前的显示
//				LCD_ShowString(30+56,200,200,16,16,(u8*)JOYPAD_SYMBOL_TBL[i]);//显示符号
//			}		
//		}		    
//	}
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

/**
 * @}
 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
