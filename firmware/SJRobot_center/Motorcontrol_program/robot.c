#include "main.h"
#include "usbh_usr.h"
#include "y_beep.h"

// �������ٶ�����
ROBOT_Velocity Vel;

// ��������������
ROBOT_Wheel Wheel_A, Wheel_B, Wheel_C, Wheel_D;

// ������IMU����
ROBOT_Imu Imu;

// ������RGB����
ROBOT_Light Light;

// �����˵�ص�ѹ����
uint16_t Bat_Vol;

// IMU����
int16_t imu_acc_data[3];
int16_t imu_gyro_data[3];
int16_t imu_gyro_offset[3];

// ��е����ر���
int16_t arm_angle[7] = {0, 0, 0, 0, 0, 0}; // ��е�۹ؽڽǶ�
int16_t arm_angle_mid[7] = {0};			   // ���߶����ֵ

// ��е����ر���
ros_servo_t ros_servo;

// ���PID���Ʋ���
int16_t motor_kp = 800;
int16_t motor_kd = 400;

// IMUУ׼��־λ
int8_t imu_calibrate_flag = 0;

// ���������־λ, 0Ϊ����2, 1Ϊ����3
int8_t uart_flag = 0;

// ������������ר�ã�ת������
ROBOT_Steering RobotStr;

// ������������ר�ã�ת������
int16_t servo_offset = 0;

int16_t times = 0;
int16_t on_time = 0;
int16_t off_time = 0;

blance_samp blance_sampPara = blance_samp_DEFAULTS;

// ��������
void ROBOT_IMUHandle(void);		// IMU���ݴ���
void ROBOT_SendDataToRos(void); // ��������
void ROBOT_BeepHandle(void);

void ROBOT_BeepHandle(void)
{
	static uint8_t beep_active = 0;
	static uint8_t beep_is_on = 0;
	static int16_t beep_remaining = 0;
	static u32 beep_next_ms = 0;
	static int16_t beep_on_time = 100;
	static int16_t beep_off_time = 100;
	u32 now = millis();

	if (times > 0)
	{
		beep_remaining = times;
		beep_on_time = (on_time > 0) ? on_time : 100;
		beep_off_time = (off_time > 0) ? off_time : beep_on_time;
		beep_active = 1;
		beep_is_on = 0;
		beep_next_ms = now;

		times = 0;
		on_time = 0;
		off_time = 0;
		BEEP_Off();
	}

	if (!beep_active || ((int32_t)(now - beep_next_ms) < 0))
	{
		return;
	}

	if (!beep_is_on)
	{
		if (beep_remaining <= 0)
		{
			beep_active = 0;
			BEEP_Off();
			return;
		}

		BEEP_On();
		beep_is_on = 1;
		beep_next_ms = now + beep_on_time;
	}
	else
	{
		BEEP_Off();
		beep_is_on = 0;
		beep_remaining--;
		if (beep_remaining <= 0)
		{
			beep_active = 0;
		}
		else
		{
			beep_next_ms = now + beep_off_time;
		}
	}
}

/**
 * @��  ��  �����˹�������
 * @��  ��  ��
 * @����ֵ  ��
 */
void Robot_Task(void)
{
	if (ros_servo_data==1)
	{ 
		ros_servo_data=0;
		// �����˻�е�ۿ���
		duoji_set(arm_angle[0], arm_angle[1], arm_angle[2], arm_angle[3], arm_angle[4], arm_angle[5]);
//		printf("J0%d J1%d J2%d J3%d J4%d J5%d \r\n ",ros_servo.angle[0],ros_servo.angle[1], ros_servo.angle[2],ros_servo.angle[3],ros_servo.angle[4],ros_servo.angle[5]  );
	
	}
	ROBOT_BeepHandle();
	// �������˶�ѧ����
	ROBOT_Kinematics();

	// ��ȡPMU6050���ٶ�����
	ROBOT_IMUHandle();

	// ���ݷ���
	ROBOT_SendDataToRos();
}

/**
 * @��  ��  ������IMU���ݴ���
 * @��  ��  ��
 * @����ֵ  ��
 */
void ROBOT_IMUHandle(void)
{
	MPU_Get_Accelerometer(imu_acc_data); // �õ����ٶȴ���������

	Imu.ACC_X = imu_acc_data[1];  // ROS����X���ӦIMU��Y��
	Imu.ACC_Y = -imu_acc_data[0]; // ROS����Y���ӦIMU��X�ᷴ��
	Imu.ACC_Z = imu_acc_data[2];  // ROS����Z���ӦIMU��Z��

	MPU_Get_Gyroscope(imu_gyro_data); // �õ�����������

	imu_gyro_data[0] += imu_gyro_offset[0]; // �����Ǽ�����ƱУ׼����
	imu_gyro_data[1] += imu_gyro_offset[1];
	imu_gyro_data[2] += imu_gyro_offset[2];
	//
	
	Imu.GYRO_X = imu_gyro_data[1];	// ROS����X���ӦIMU��Y��
	Imu.GYRO_Y = -imu_gyro_data[0]; // ROS����Y���ӦIMU��X�ᷴ��
	Imu.GYRO_Z = imu_gyro_data[2];	// ROS����Z���ӦIMU��Z��
//	printf("%d\r\n",Imu.GYRO_Z);
	//	mpu_dmp_get_data(&blance_sampPara.gpitch, &blance_sampPara.groll, &blance_sampPara.gyaw,&blance_sampPara.gGyro1, &blance_sampPara.gGyro2,&blance_sampPara.gGyro3,&blance_sampPara.gAx, &blance_sampPara.gAy, &blance_sampPara.gAz);
}

/**
 * @��  ��  �����˷������ݵ�ROS
 * @��  ��  ��
 * @����ֵ  ��
 */
void ROBOT_SendDataToRos(void)
{
	// ���ڷ�������
	static uint8_t comdata[32];

	// ���ٶ� = (ax_acc/32768) * 2G
	comdata[0] = (u8)(Imu.ACC_X >> 8);
	comdata[1] = (u8)(Imu.ACC_X);
	comdata[2] = (u8)(Imu.ACC_Y >> 8);
	comdata[3] = (u8)(Imu.ACC_Y);
	comdata[4] = (u8)(Imu.ACC_Z >> 8);
	comdata[5] = (u8)(Imu.ACC_Z);

	// �����ǽ��ٶ� = (ax_gyro/32768) * 500
	comdata[6] = (u8)(Imu.GYRO_X >> 8);
	comdata[7] = (u8)(Imu.GYRO_X);
	comdata[8] = (u8)(Imu.GYRO_Y >> 8);
	comdata[9] = (u8)(Imu.GYRO_Y);
	comdata[10] = (u8)(Imu.GYRO_Z >> 8);
	comdata[11] = (u8)(Imu.GYRO_Z);
//	printf("aaa%d\r\n",Imu.GYRO_Z);
	// �������ٶ�ֵ ��λΪm/s���Ŵ�1000��
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
	
	// ��ص�ѹ
	comdata[30] = (u8)(Bat_Vol >> 8);
	comdata[31] = (u8)(Bat_Vol);

	UART2_SendPacket(comdata, 32, ID_STM2ROS_DATA);
}

/**
 * @��  ��  ������������
 * @��  ��  ��
 * @����ֵ  ��
 */
void Bat_Task()
{
	// ��������
	static uint16_t bat_vol_cnt = 0;

	while (1)
	{
		// �ɼ���ص�ѹ
		Bat_Vol = VIN_GetVol_X100();

		// ���������ص�ѹ����
		// printf("@ %d  \r\n",R_Bat_Vol);

		// ��������40%
		if (Bat_Vol < VBAT_40P)
		{
			// ��������20%
			if (Bat_Vol < VBAT_20P)
			{

				// ��������10%���ر�ϵͳ���뱣��״̬
				if (Bat_Vol < VBAT_10P) // 990
				{
					// ��ѹʱ�����
					bat_vol_cnt++;

					// ����10�Σ�����ر�״̬
					if (bat_vol_cnt > 10)
					{
						// ����ٶ�����Ϊ0
						MOTOR_A_SetSpeed(0);
						MOTOR_B_SetSpeed(0);
						MOTOR_C_SetSpeed(0);
						MOTOR_D_SetSpeed(0);

						// ���������б���
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
 * @��  ��  ������������
 * @��  ��  ��
 * @����ֵ  ��
 */
void Key_Task()
{
	while (1)
	{
		// ����ɨ��
		if (KEY_Scan() == 0)
		{
			Delay_ms(5);
			// ȷ����������
			if (KEY_Scan() == 0)
			{
				// �ȴ�����̧��
				while (KEY_Scan() == 0)
				{
					Delay_ms(5);
				}
			}
		}
	}
}

/**
 * @��  ��  ��̬У׼����
 * @��  ��  ��
 * @����ֵ  ��
 */
void Imu_Task()
{
	uint8_t i;

	// ������У׼����
	static int16_t gyro_data[3];

	while (1)
	{

		// ���IMUУ׼��־λ
		if (imu_calibrate_flag > 0)
		{

			// ��������ʾ
			BEEP_On();
			Delay_ms(5);
			BEEP_Off();

			imu_gyro_offset[0] = 0;
			imu_gyro_offset[1] = 0;
			imu_gyro_offset[2] = 0;

			// ������У׼
			for (i = 0; i < 10; i++)
			{
				// ��ʱ����
				Delay_ms(5);

				// ��ȡPMU6050����������
				//				MPU_Get_Gyroscope(gyro_data);

				// ����ƫ���
				imu_gyro_offset[0] += gyro_data[0];
				imu_gyro_offset[1] += gyro_data[1];
				imu_gyro_offset[2] += gyro_data[2];
			}

			// ����ƽ��ƫ��ֵ
			imu_gyro_offset[0] = -imu_gyro_offset[0] / 10;
			imu_gyro_offset[1] = -imu_gyro_offset[1] / 10;
			imu_gyro_offset[2] = -imu_gyro_offset[2] / 10;

			// ��������ʾ
			BEEP_On();
			Delay_ms(5);
			BEEP_Off();

			// ��λIMUУ׼��־λ
			imu_calibrate_flag = 0;
		}
	}
}
