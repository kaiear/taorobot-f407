#include "main.h"

#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;
extern HID_Machine_TypeDef HID_Machine;

#define ENABLE_MOTOR_FORCE_TEST 0
#define MOTOR_FORCE_TEST_SPEED  500

int main(void)
	
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	int16_t gyro_data[3];
	Delay_ms(5);
	SysTickConfig(); 
	Delay_ms(5);
	GPIO_LED_int(); 
	Delay_ms(5);

	SERVO_Init();
	Delay_ms(5);

	TIM14_Int_Init(5000 - 1, 84 - 1); 
	Delay_ms(5);
	//	KEY_Init();

	MOTOR_AB_Init();
	Delay_ms(5);
	MOTOR_CD_Init();
	Delay_ms(5);
	

	VIN_Init();
	Delay_ms(5);

	BEEP_Init();

	UART1_Init(115200);
	Delay_ms(5);

	UART2_Init(115200);
	Delay_ms(5);

	USART3_Init(115200);
	Delay_ms(5);

	ENCODER_A_Init();
	Delay_ms(5);
	ENCODER_B_Init();
	Delay_ms(5);
	ENCODER_C_Init();
	Delay_ms(5);
	ENCODER_D_Init();
	Delay_ms(5);

	MPU_Init();
	Delay_ms(5);
	mpu_dmp_init();
	Delay_ms(10);
	ros_servo.pwm[0]=1500;
	ros_servo.pwm[1]=1895;
	ros_servo.pwm[2]=2380;
	ros_servo.pwm[3]=950;
	ros_servo.pwm[4]=1500;
	ros_servo.pwm[5]=1160;
	ros_servo.time[0] = 1500;
	ros_servo.time[1] = 1500;
	ros_servo.time[2] = 1500;
	ros_servo.time[3] = 1500;
	ros_servo.time[4] = 1500;
	ros_servo.time[5] = 1500;
	duoji_set(arm_angle[0], arm_angle[1], arm_angle[2], arm_angle[3], arm_angle[4], arm_angle[5]);
	// BEEP_On();
	// Delay_ms(5);
	// BEEP_Off();
	// Delay_ms(5);
	float data[300],sumx,sumy,sumz;
	
	for (int i = 0; i < 10; i++)
	{
		
		MPU_Get_Gyroscope(imu_gyro_data);
		imu_gyro_offset[0] += gyro_data[0];
		imu_gyro_offset[1] += gyro_data[1];
		imu_gyro_offset[2] += gyro_data[2];
	}

	
	imu_gyro_offset[0] = -imu_gyro_offset[0] / 10;
	imu_gyro_offset[1] = -imu_gyro_offset[1] / 10;
	imu_gyro_offset[2] = -imu_gyro_offset[2] / 10;
	
	USBH_Init(&USB_OTG_Core_dev,
			  USB_OTG_FS_CORE_ID,
			  &USB_Host, &HID_cb, &USR_Callbacks);
	
	Delay_ms(20000); 
	for(int z=0; z<=199; z++)
	{
		MPU_Get_Gyroscope(imu_gyro_data);
		data[z]=imu_gyro_data[0];
		sumx = sumx + data[z];
	}
	for(int z=0; z<=199; z++)
	{
		MPU_Get_Gyroscope(imu_gyro_data);
		data[z]=imu_gyro_data[1];
		sumy = sumy + data[z];
	}
	for(int z=0; z<=199; z++)
	{
		MPU_Get_Gyroscope(imu_gyro_data);
		data[z]=imu_gyro_data[2];
		sumz = sumz + data[z];
	}
//	printf("sumx=%f, sumy=%f, sumz=%f",sumx/200.0,sumy/200.0,sumz/200.0);
	beep_on_times(3, 100);
 Task_Manage_List_Init();
    while (1)
    {
        Execute_Task_List_RUN();

#if ENABLE_MOTOR_FORCE_TEST
        MOTOR_A_SetSpeed(MOTOR_FORCE_TEST_SPEED);
        MOTOR_B_SetSpeed(MOTOR_FORCE_TEST_SPEED);
        MOTOR_C_SetSpeed(MOTOR_FORCE_TEST_SPEED);
        MOTOR_D_SetSpeed(MOTOR_FORCE_TEST_SPEED);
#endif
        
        Delay_ms(10);
    }
}

void soft_reset(void)
{
	printf("stm32 reset\r\n");
	
	__set_FAULTMASK(1);
	
	NVIC_SystemReset();
}
