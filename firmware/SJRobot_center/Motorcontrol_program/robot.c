#include "main.h"
#include "usbh_usr.h"

// 机器人速度数据
ROBOT_Velocity Vel;

// 机器人轮子数据
ROBOT_Wheel Wheel_A, Wheel_B, Wheel_C, Wheel_D;

// 机器人IMU数据
ROBOT_Imu Imu;

// 机器人RGB数据
ROBOT_Light Light;

// 机器人电池电压数据
uint16_t Bat_Vol;

// IMU数据
int16_t imu_acc_data[3];
int16_t imu_gyro_data[3];
int16_t imu_gyro_offset[3];

// 机械臂相关变量
int16_t arm_angle[7] = {0, 0, 0, 0, 0, 0}; // 机械臂关节角度
int16_t arm_angle_mid[7] = {0};			   // 总线舵机中值

// 机械臂相关变量
ros_servo_t ros_servo;

// 电机PID控制参数
int16_t motor_kp = 800;
int16_t motor_kd = 400;

// IMU校准标志位
int8_t imu_calibrate_flag = 0;

// 串口输出标志位, 0为串口2, 1为串口3
int8_t uart_flag = 0;

// 阿克曼机器人专用，转向数据
ROBOT_Steering RobotStr;

// 阿克曼机器人专用，转向数据
int16_t servo_offset = 0;

int16_t times = 0;
int16_t on_time = 0;
int16_t off_time = 0;

blance_samp blance_sampPara = blance_samp_DEFAULTS;

// 函数定义
void ROBOT_IMUHandle(void);		// IMU数据处理
void ROBOT_SendDataToRos(void); // 发送数据

/**
 * @简  述  机器人管理任务
 * @参  数  无
 * @返回值  无
 */
void Robot_Task(void)
{
	if (ros_servo_data==1)
	{ 
		ros_servo_data=0;
		// 机器人机械臂控制
		duoji_set(arm_angle[0], arm_angle[1], arm_angle[2], arm_angle[3], arm_angle[4], arm_angle[5]);
//		printf("J0%d J1%d J2%d J3%d J4%d J5%d \r\n ",ros_servo.angle[0],ros_servo.angle[1], ros_servo.angle[2],ros_servo.angle[3],ros_servo.angle[4],ros_servo.angle[5]  );
	
	}
	// 机器人运动学处理
	ROBOT_Kinematics();

	// 获取PMU6050加速度数据
	ROBOT_IMUHandle();

	// 数据发送
	ROBOT_SendDataToRos();
}

/**
 * @简  述  机器人IMU数据处理
 * @参  数  无
 * @返回值  无
 */
void ROBOT_IMUHandle(void)
{
	MPU_Get_Accelerometer(imu_acc_data); // 得到加速度传感器数据

	Imu.ACC_X = imu_acc_data[1];  // ROS坐标X轴对应IMU的Y轴
	Imu.ACC_Y = -imu_acc_data[0]; // ROS坐标Y轴对应IMU的X轴反向
	Imu.ACC_Z = imu_acc_data[2];  // ROS坐标Z轴对应IMU的Z轴

	MPU_Get_Gyroscope(imu_gyro_data); // 得到陀螺仪数据

	imu_gyro_data[0] += imu_gyro_offset[0]; // 陀螺仪加入零票校准数据
	imu_gyro_data[1] += imu_gyro_offset[1];
	imu_gyro_data[2] += imu_gyro_offset[2];
	//
	
	Imu.GYRO_X = imu_gyro_data[1];	// ROS坐标X轴对应IMU的Y轴
	Imu.GYRO_Y = -imu_gyro_data[0]; // ROS坐标Y轴对应IMU的X轴反向
	Imu.GYRO_Z = imu_gyro_data[2];	// ROS坐标Z轴对应IMU的Z轴
//	printf("%d\r\n",Imu.GYRO_Z);
	//	mpu_dmp_get_data(&blance_sampPara.gpitch, &blance_sampPara.groll, &blance_sampPara.gyaw,&blance_sampPara.gGyro1, &blance_sampPara.gGyro2,&blance_sampPara.gGyro3,&blance_sampPara.gAx, &blance_sampPara.gAy, &blance_sampPara.gAz);
}

/**
 * @简  述  机器人发送数据到ROS
 * @参  数  无
 * @返回值  无
 */
void ROBOT_SendDataToRos(void)
{
	// 串口发送数据
	static uint8_t comdata[32];

	// 加速度 = (ax_acc/32768) * 2G
	comdata[0] = (u8)(Imu.ACC_X >> 8);
	comdata[1] = (u8)(Imu.ACC_X);
	comdata[2] = (u8)(Imu.ACC_Y >> 8);
	comdata[3] = (u8)(Imu.ACC_Y);
	comdata[4] = (u8)(Imu.ACC_Z >> 8);
	comdata[5] = (u8)(Imu.ACC_Z);

	// 陀螺仪角速度 = (ax_gyro/32768) * 500
	comdata[6] = (u8)(Imu.GYRO_X >> 8);
	comdata[7] = (u8)(Imu.GYRO_X);
	comdata[8] = (u8)(Imu.GYRO_Y >> 8);
	comdata[9] = (u8)(Imu.GYRO_Y);
	comdata[10] = (u8)(Imu.GYRO_Z >> 8);
	comdata[11] = (u8)(Imu.GYRO_Z);
//	printf("aaa%d\r\n",Imu.GYRO_Z);
	// 机器人速度值 单位为m/s，放大1000倍
	comdata[12] = (u8)(Vel.RT_IX >> 8);
	comdata[13] = (u8)(Vel.RT_IX);
	comdata[14] = (u8)(Vel.RT_IY >> 8);
	comdata[15] = (u8)(Vel.RT_IY);
	comdata[16] = (u8)(Vel.RT_IW >> 8);
	comdata[17] = (u8)(Vel.RT_IW);
	
	comdata[18] = (u8)((1500-ros_servo.pwm[0]) >> 8);
	comdata[19] = (u8)((1500-ros_servo.pwm[0]));
	comdata[20] = (u8)((1500-ros_servo.pwm[1]) >> 8);
	comdata[21] = (u8)((1500-ros_servo.pwm[1]));
	comdata[22] = (u8)((ros_servo.pwm[2]-1500) >> 8);
	comdata[23] = (u8)((ros_servo.pwm[2]-1500));
	comdata[24] = (u8)((1500-ros_servo.pwm[3]) >> 8);
	comdata[25] = (u8)((1500-ros_servo.pwm[3]));
	comdata[26] = (u8)((1500-ros_servo.pwm[4]) >> 8);
	comdata[27] = (u8)((1500-ros_servo.pwm[4]));
	comdata[28] = (u8)((1500-ros_servo.pwm[5]) >> 8);
	comdata[29] = (u8)((1500-ros_servo.pwm[5]));
	
	// 电池电压
	comdata[30] = (u8)(Bat_Vol >> 8);
	comdata[31] = (u8)(Bat_Vol);

	UART2_SendPacket(comdata, 32, ID_STM2ROS_DATA);
}

/**
 * @简  述  电量管理任务
 * @参  数  无
 * @返回值  无
 */
void Bat_Task()
{
	// 计数变量
	static uint16_t bat_vol_cnt = 0;

	while (1)
	{
		// 采集电池电压
		Bat_Vol = VIN_GetVol_X100();

		// 调试输出电池电压数据
		// printf("@ %d  \r\n",R_Bat_Vol);

		// 电量低于40%
		if (Bat_Vol < VBAT_40P)
		{
			// 电量低于20%
			if (Bat_Vol < VBAT_20P)
			{

				// 电量低于10%，关闭系统进入保护状态
				if (Bat_Vol < VBAT_10P) // 990
				{
					// 低压时间计数
					bat_vol_cnt++;

					// 超过10次，进入关闭状态
					if (bat_vol_cnt > 10)
					{
						// 电机速度设置为0
						MOTOR_A_SetSpeed(0);
						MOTOR_B_SetSpeed(0);
						MOTOR_C_SetSpeed(0);
						MOTOR_D_SetSpeed(0);

						// 蜂鸣器鸣叫报警
						while (1)
						{
							BEEP_On();
							BEEP_Off();
						}
					}
				}
				else
				{
					bat_vol_cnt = 0;
				}
			}
		}
		else
		{
		}
	}
}

/**
 * @简  述  按键处理任务
 * @参  数  无
 * @返回值  无
 */
void Key_Task()
{
	while (1)
	{
		// 按键扫描
		if (KEY_Scan() == 0)
		{
			Delay_ms(5);
			// 确定按键按下
			if (KEY_Scan() == 0)
			{
				// 等待按键抬起
				while (KEY_Scan() == 0)
				{
					Delay_ms(5);
				}
			}
		}
	}
}

/**
 * @简  述  姿态校准任务
 * @参  数  无
 * @返回值  无
 */
void Imu_Task()
{
	uint8_t i;

	// 陀螺仪校准变量
	static int16_t gyro_data[3];

	while (1)
	{

		// 检测IMU校准标志位
		if (imu_calibrate_flag > 0)
		{

			// 蜂鸣器提示
			BEEP_On();
			Delay_ms(5);
			BEEP_Off();

			imu_gyro_offset[0] = 0;
			imu_gyro_offset[1] = 0;
			imu_gyro_offset[2] = 0;

			// 陀螺仪校准
			for (i = 0; i < 10; i++)
			{
				// 延时函数
				Delay_ms(5);

				// 获取PMU6050陀螺仪数据
				//				MPU_Get_Gyroscope(gyro_data);

				// 计算偏差和
				imu_gyro_offset[0] += gyro_data[0];
				imu_gyro_offset[1] += gyro_data[1];
				imu_gyro_offset[2] += gyro_data[2];
			}

			// 计算平均偏差值
			imu_gyro_offset[0] = -imu_gyro_offset[0] / 10;
			imu_gyro_offset[1] = -imu_gyro_offset[1] / 10;
			imu_gyro_offset[2] = -imu_gyro_offset[2] / 10;

			// 蜂鸣器提示
			BEEP_On();
			Delay_ms(5);
			BEEP_Off();

			// 复位IMU校准标志位
			imu_calibrate_flag = 0;
		}
	}
}
