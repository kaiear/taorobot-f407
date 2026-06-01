#include "main.h"
//读取串口舵机的数据并且准备发送到上位机主控










//获得上位机的舵机指令，并且发送到串口舵机上
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//PWM 舵机
/**
  * @简  述  机器人机械臂控制
  * @参  数  无
  * @返回值  无
  */
void ROBOT_ArmControl(void)
{
	int16_t temp;
	
	//幅度限制保护
	if(arm_angle[0] > JOINTA_UP_LIMIT)	 arm_angle[0] = JOINTA_UP_LIMIT;
	if(arm_angle[0] < JOINTA_LOW_LIMIT)   arm_angle[0] = JOINTA_LOW_LIMIT;
	
	if(arm_angle[1] > JOINTB_UP_LIMIT)	 arm_angle[1] = JOINTB_UP_LIMIT;
	if(arm_angle[1] < JOINTB_LOW_LIMIT)   arm_angle[1] = JOINTB_LOW_LIMIT;
	
	if(arm_angle[2] > JOINTC_UP_LIMIT)	 arm_angle[2] = JOINTC_UP_LIMIT;
	if(arm_angle[2] < JOINTC_LOW_LIMIT)   arm_angle[2] = JOINTC_LOW_LIMIT;

	if(arm_angle[3] > JOINTD_UP_LIMIT)	 arm_angle[3] = JOINTD_UP_LIMIT;
	if(arm_angle[3] < JOINTD_LOW_LIMIT)   arm_angle[3] = JOINTD_LOW_LIMIT;

	if(arm_angle[4] > JOINTE_UP_LIMIT)	 arm_angle[4] = JOINTE_UP_LIMIT;
	if(arm_angle[4] < JOINTE_LOW_LIMIT)   arm_angle[4] = JOINTE_LOW_LIMIT;

	if(arm_angle[5] > JOINTF_UP_LIMIT)	 arm_angle[5] = JOINTF_UP_LIMIT;
	if(arm_angle[5] < JOINTF_LOW_LIMIT)   arm_angle[5] = JOINTF_LOW_LIMIT;	
	
	//printf("A%d  \r\n ", ax_arm_angle[0] );
	
	//设置舵机角度
	SERVO_S1_SetAngle((arm_angle[0] + arm_angle_offset[0])/(PI*1000)*1800);
	SERVO_S2_SetAngle((-(arm_angle[1] + arm_angle_offset[1]))/(PI*1000)*1800);
	SERVO_S3_SetAngle((arm_angle[2] + arm_angle_offset[2])/(PI*1000)*1800);
	SERVO_S4_SetAngle((arm_angle[3] + arm_angle_offset[3])/(PI*1000)*1800);	
	SERVO_S5_SetAngle((arm_angle[4] + arm_angle_offset[4])/(PI*1000)*1800);
	SERVO_S6_SetAngle((arm_angle[5] + arm_angle_offset[5])/(PI*1000)*1800);		
	
}

/**
  * @简  述  机器人机械臂底盘复位
  * @参  数  无
  * @返回值  无
  */
void ROBOT_Reset(void)
{
	int16_t temp;
	arm_angle[0] = 0;
	arm_angle[1] = -670;
	arm_angle[2] = -1500;
	arm_angle[3] = -1000;
	arm_angle[4] = 0;
	arm_angle[5] = 0;
	
	//幅度限制保护
	if(arm_angle[0] > JOINTA_UP_LIMIT)	 arm_angle[0] = JOINTA_UP_LIMIT;
	if(arm_angle[0] < JOINTA_LOW_LIMIT)   arm_angle[0] = JOINTA_LOW_LIMIT;
	
	if(arm_angle[1] > JOINTB_UP_LIMIT)	 arm_angle[1] = JOINTB_UP_LIMIT;
	if(arm_angle[1] < JOINTB_LOW_LIMIT)   arm_angle[1] = JOINTB_LOW_LIMIT;
	
	if(arm_angle[2] > JOINTC_UP_LIMIT)	 arm_angle[2] = JOINTC_UP_LIMIT;
	if(arm_angle[2] < JOINTC_LOW_LIMIT)   arm_angle[2] = JOINTC_LOW_LIMIT;

	if(arm_angle[3] > JOINTD_UP_LIMIT)	 arm_angle[3] = JOINTD_UP_LIMIT;
	if(arm_angle[3] < JOINTD_LOW_LIMIT)   arm_angle[3] = JOINTD_LOW_LIMIT;

	if(arm_angle[4] > JOINTE_UP_LIMIT)	 arm_angle[4] = JOINTE_UP_LIMIT;
	if(arm_angle[4] < JOINTE_LOW_LIMIT)   arm_angle[4] = JOINTE_LOW_LIMIT;

	if(arm_angle[5] > JOINTF_UP_LIMIT)	 arm_angle[5] = JOINTF_UP_LIMIT;
	if(arm_angle[5] < JOINTF_LOW_LIMIT)   arm_angle[5] = JOINTF_LOW_LIMIT;	
	
	//printf("A%d  \r\n ", ax_arm_angle[0] );
	//设置舵机角度
	SERVO_S1_SetAngle((0 + arm_angle_offset[0])/(PI*1000)*1800);
	SERVO_S2_SetAngle((-(-670 + arm_angle_offset[1]))/(PI*1000)*1800);
	SERVO_S3_SetAngle((-1500 + arm_angle_offset[2])/(PI*1000)*1800);
	SERVO_S4_SetAngle((-1000 + arm_angle_offset[3])/(PI*1000)*1800);	
	SERVO_S5_SetAngle((0 + arm_angle_offset[4])/(PI*1000)*1800);
	SERVO_S6_SetAngle((0 + arm_angle_offset[5])/(PI*1000)*1800);		
	
}

