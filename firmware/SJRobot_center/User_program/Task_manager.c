// ############################################################
//  FILE:  Task_manager.c
//  Created on: 2021Äê8ÔÂ14ÈÕ
//  Author: lee
//  summary: Task_manager
// ############################################################

#include "main.h"
#include "stdio.h"
#include "usbh_usr.h"

#define Task_Num 5

#define HFPeriod_COUNT 1	// 5ms
#define FaulPeriod_COUNT 10 // 10ms
#define Robot_COUNT 5		// 10ms
#define KEY_COUNT 80		// 10ms
#define LEDPeriod_COUNT 600 // 500ms
#define filter_N 12

TaskTime TasksPare[Task_Num];
float dir_anglecmd = 135.0;

extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
extern __ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;

void Timer_Task_Count(void) // 1msµÄÖĞ¶Ï¼ÆÊı
{
	u16 Task_Count = 0;

	for (Task_Count = 0; Task_Count < Task_Num; Task_Count++) // TASK_NUM=5
	{
		if ((TasksPare[Task_Count].Task_Count < TasksPare[Task_Count].Task_Period) && (TasksPare[Task_Count].Task_Period > 0))
		{
			TasksPare[Task_Count].Task_Count++; // ¼ÆÊı ÊÂ¼şÈÎÎñ¼ÆÊı
		}
	}
}

void Execute_Task_List_RUN(void)
{
	uint16_t Task_Count = 0;

	for (Task_Count = 0; Task_Count < Task_Num; Task_Count++)
	{
		if ((TasksPare[Task_Count].Task_Count >= TasksPare[Task_Count].Task_Period) && (TasksPare[Task_Count].Task_Period > 0))
		{
			TasksPare[Task_Count].Task_Function(); // ÔËĞĞ¼ÆÊıµÄÊ±¼äÈÎÎñº¯Êı
			TasksPare[Task_Count].Task_Count = 0;
		}
	}
}

void HFPeriod_1msTask(void) // ±¸ÓÃ
{
	static uint8_t temp_flag = 0;
	TaoV2_SafetyTick();
	app_ps2();
	USBH_Process(&USB_OTG_Core_dev, &USB_Host);

	/* USBÉè±¸ÒÑ¾­Á¬½Ó³É¹¦¡£ÎÒÃÇÊ¹ÓÃps2£¬¾ÍÊÇps2Á¬½Ó³É¹¦£¬
		ĞèÒª¶Ï¿ªÓëÊ÷İ®ÅÉµÄÁ¬½Ó£¬¾ÍÊÇ½ûÖ¹´®¿Ú2½ÓÊÕ 
	*/
	if ((bDeviceState == 1) && (temp_flag == 0))
	{
		temp_flag = 1;
		// ¹Ø±Õ´®¿Ú½ÓÊÕÖĞ¶Ï
		//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}
	else if ((bDeviceState == 0) && (temp_flag == 1))
	{
		temp_flag = 0;
		// ¿ªÆô´®¿Ú½ÓÊÕÖĞ¶Ï
		//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ¿ªÆôÏà¹ØÖĞ¶Ï
	}
	
}

void task_send_Rece(void) // Êı¾İ·¢ËÍÈÎÎñ ½«×îĞÂµÄÊı¾İ·¢ËÍ³öÈ¥¡£10ms·¢ËÍÒ»´Î
{
	Robot_Task();
}

void Robot_Control(void) // »úÆ÷ÈË¿ØÖÆÈÎÎñ£¬º¬µçÁ¿¹ÜÀí
{
	MPU_Get_Gyroscope(imu_gyro_data);
	MPU_Get_Accelerometer(imu_acc_data);
	Robot_Task();
}

void KEY_RUN(void) // Íâ²¿°´¼ü´¦ÀíÈÎÎñ
{
}

void Task_LED(void) // ×îĞ¡ÏµÍ³500msµÄLEDµÄÉÁË¸
{
	GPIO_ToggleBits(GPIOB, GPIO_Pin_6); //  500msµÄLEDµÄÉÁË
	//printf("A%d B%d C%d  \r\n ",Vel.TG_IX, Vel.TG_IY, Vel.TG_IW );
}

void Task_Manage_List_Init(void)
{
	TasksPare[0].Task_Period = HFPeriod_COUNT; // PERIOD_COUNT=5    5ms
	TasksPare[0].Task_Count = 1;			   // Task_CountµÄ³õÖµ²»Ò»Ñù£¬±ÜÃâ500msµ½Ê±£¬ËùÓĞÈÎÎñ¶¼Ö´ĞĞÒ»±é¡£
	TasksPare[0].Task_Function = HFPeriod_1msTask;

	TasksPare[1].Task_Period = FaulPeriod_COUNT; // 10ms
	TasksPare[1].Task_Count = 8;
	TasksPare[1].Task_Function = task_send_Rece; //

	TasksPare[2].Task_Period = Robot_COUNT; // 20ms
	TasksPare[2].Task_Count = 10;
	TasksPare[2].Task_Function = Robot_Control; //

	TasksPare[3].Task_Period = KEY_COUNT; // 100ms
	TasksPare[3].Task_Count = 80;
	TasksPare[3].Task_Function = KEY_RUN; //

	TasksPare[4].Task_Period = LEDPeriod_COUNT; // 500ms
	TasksPare[4].Task_Count = 300;
	TasksPare[4].Task_Function = Task_LED; // 500msµÄLEDµÄÉÁË¸
}
// USER CODE END
