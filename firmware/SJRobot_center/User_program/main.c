#include "main.h"

#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;
extern HID_Machine_TypeDef HID_Machine;

#define ENABLE_MOTOR_FORCE_TEST 0
#define MOTOR_FORCE_TEST_SPEED 600
#define STARTUP_STABILIZE_DELAY_MS 1000
#define ENABLE_STARTUP_GYRO_AVG 0
#define STARTUP_GYRO_SAMPLE_COUNT 20
#define MAIN_ALIVE_DEBUG 0

int main(void)
	
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

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
	printf("uart1 debug online\r\n");

	UART2_Init(115200);
	Delay_ms(5);
	TaoV2_Init();

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
	ros_servo.pwm[0]=servo_home_pwm(0);
	ros_servo.pwm[1]=servo_home_pwm(1);
	ros_servo.pwm[2]=servo_home_pwm(2);
	ros_servo.pwm[3]=servo_home_pwm(3);
	ros_servo.pwm[4]=servo_home_pwm(4);
	ros_servo.pwm[5]=servo_home_pwm(5);
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
#if ENABLE_STARTUP_GYRO_AVG
	float data[STARTUP_GYRO_SAMPLE_COUNT];
	float sumx = 0;
	float sumy = 0;
	float sumz = 0;
#endif
	
	for (int i = 0; i < 10; i++)
	{
		
		MPU_Get_Gyroscope(imu_gyro_data);
		imu_gyro_offset[0] += imu_gyro_data[0];
		imu_gyro_offset[1] += imu_gyro_data[1];
		imu_gyro_offset[2] += imu_gyro_data[2];
	}

	
	imu_gyro_offset[0] = -imu_gyro_offset[0] / 10;
	imu_gyro_offset[1] = -imu_gyro_offset[1] / 10;
	imu_gyro_offset[2] = -imu_gyro_offset[2] / 10;
	
	USBH_Init(&USB_OTG_Core_dev,
			  USB_OTG_FS_CORE_ID,
			  &USB_Host, &HID_cb, &USR_Callbacks);
	printf("boot after usbh init\r\n");
	
	Delay_ms(STARTUP_STABILIZE_DELAY_MS);
#if ENABLE_STARTUP_GYRO_AVG
	for(int z=0; z<STARTUP_GYRO_SAMPLE_COUNT; z++)
	{
		MPU_Get_Gyroscope(imu_gyro_data);
		data[z]=imu_gyro_data[0];
		sumx = sumx + data[z];
	}
	for(int z=0; z<STARTUP_GYRO_SAMPLE_COUNT; z++)
	{
		MPU_Get_Gyroscope(imu_gyro_data);
		data[z]=imu_gyro_data[1];
		sumy = sumy + data[z];
	}
	for(int z=0; z<STARTUP_GYRO_SAMPLE_COUNT; z++)
	{
		MPU_Get_Gyroscope(imu_gyro_data);
		data[z]=imu_gyro_data[2];
		sumz = sumz + data[z];
	}
//	printf("sumx=%f, sumy=%f, sumz=%f",sumx/STARTUP_GYRO_SAMPLE_COUNT,sumy/STARTUP_GYRO_SAMPLE_COUNT,sumz/STARTUP_GYRO_SAMPLE_COUNT);
#endif
	beep_on_times(3, 100);
 Task_Manage_List_Init();
	printf("main loop enter\r\n");
    while (1)
    {
#if MAIN_ALIVE_DEBUG
        static uint16_t alive_count = 0;
#endif

        Execute_Task_List_RUN();

#if MAIN_ALIVE_DEBUG
		alive_count++;
		if (alive_count >= 100)
		{
			alive_count = 0;
			printf("alive tick\r\n");
		}
#endif

        servo_bus_test_send_once();

#if ENABLE_MOTOR_FORCE_TEST
        MOTOR_A_SetSpeed(0);
        MOTOR_B_SetSpeed(0);
        MOTOR_C_SetSpeed(0);
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
