#ifndef __MAIN_H
#define __MAIN_H	 
#include <stdio.h>
#include <math.h>

#include "stm32f4xx.h" 
#include "arm.h"
//#include "Usart.h"

#include "stdio.h"

#include "Sys.h"
#include "TIM1pwm.h"
#include "GPIO_int.h"
#include "can.h"
#include "uart1.h"
#include "uart2.h"
#include "tao_protocol_v2.h"
#include "y_zx_uart3.h"

#include "y_motor.h" 
#include "y_switch.h"
#include "y_servo.h"
#include "y_mpu6050.h"
#include "y_inv_mpu.h"
#include "y_w25q64.h"

#include "Task_manager.h"
#include "robot.h"
#include "kinematics.h"

#include "y_arm_kinematics.h"
#include "y_global.h"
#include "app_ps2.h"

u8 MPU_Init(void);
u8 mpu_dmp_init(void);

void soft_reset(void);
#endif












