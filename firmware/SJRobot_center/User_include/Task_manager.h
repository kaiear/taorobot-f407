// ############################################################
//  FILE:  Task_manager_H
//  Created on: 2021Äê8ÔÂ14ÈÕ
//  Author: lee
//  summary: Header file  and definition
// ############################################################

#ifndef _Task_manager_H_
#define _Task_manager_H_
#include "sys.h"

typedef void (*FTimer_P)(void);

typedef struct
{
	u16 Task_Period; // time out for calling function
	u16 Task_Count;
	FTimer_P Task_Function; // Send function defines in application
} TaskTime;

void Timer_Task_Count(void);
void Execute_Task_List_RUN(void);
void Task_Manage_List_Init(void);

void Task_LED(void);
void HFPeriod_1msTask(void);
void task_send_Rece(void);
void Balance_Control(void);

void BIKE_balance(void);

#endif // Task_manager_H
