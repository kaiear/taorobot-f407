#include "main.h"

//根据机器人类型，选择编译机器人运动学处理函数
#define ROBOT_TYPE   ROBOT_MEC


#if (ROBOT_TYPE == ROBOT_MEC)
/**
  * @简  述  机器人运动学处理-麦克纳姆轮
  * @参  数  无
  * @返回值  无
  */
void ROBOT_Kinematics(void)
{

	//通过编码器获取车轮实时转速m/s
	Wheel_A.RT = (float)-((int16_t)ENCODER_A_GetCounter()*MEC_WHEEL_SCALE);
	ENCODER_A_SetCounter(0);
	Wheel_B.RT = (float)((int16_t)ENCODER_B_GetCounter()*MEC_WHEEL_SCALE);
	ENCODER_B_SetCounter(0);		
	Wheel_C.RT = (float)-((int16_t)ENCODER_C_GetCounter()*MEC_WHEEL_SCALE);
	ENCODER_C_SetCounter(0);
	Wheel_D.RT = (float)((int16_t)ENCODER_D_GetCounter()*MEC_WHEEL_SCALE);
	ENCODER_D_SetCounter(0);
	
	//调试输出轮子转速
	//printf("@%f  %f   \r\n",Wheel_A.RT,Wheel_B.RT);
	
	//运动学正解析，由机器人轮子速度计算机器人速度
	Vel.RT_IX = (( Wheel_A.RT + Wheel_B.RT + Wheel_C.RT + Wheel_D.RT)/4)*1000;
	Vel.RT_IY = ((-Wheel_A.RT + Wheel_B.RT + Wheel_C.RT - Wheel_D.RT)/4)*1000;
	Vel.RT_IW = ((-Wheel_A.RT + Wheel_B.RT - Wheel_C.RT + Wheel_D.RT)/4/(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2))*1000;	
	
	//机器人目标速度限制
	if( Vel.TG_IX > R_VX_LIMIT )    Vel.TG_IX = R_VX_LIMIT;
	if( Vel.TG_IX < (-R_VX_LIMIT))  Vel.TG_IX = (-R_VX_LIMIT);
	if( Vel.TG_IY > R_VY_LIMIT)     Vel.TG_IY = R_VY_LIMIT;
	if( Vel.TG_IY < (-R_VY_LIMIT))  Vel.TG_IY = (-R_VY_LIMIT);
	if( Vel.TG_IW > R_VW_LIMIT)     Vel.TG_IW = R_VW_LIMIT;
	if( Vel.TG_IW < (-R_VW_LIMIT))  Vel.TG_IW = (-R_VW_LIMIT);
	
	//目标速度转化为浮点类型
	Vel.TG_FX = (Vel.TG_IX/1000)+(Vel.TG_IX%1000)*0.001;
	Vel.TG_FY = (Vel.TG_IY/1000)+(Vel.TG_IY%1000)*0.001;
	Vel.TG_FW = (Vel.TG_IW/1000)+(Vel.TG_IW%1000)*0.001;
	
	//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
	Wheel_A.TG = Vel.TG_FX - Vel.TG_FY - Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	Wheel_B.TG = Vel.TG_FX + Vel.TG_FY + Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	Wheel_C.TG = Vel.TG_FX + Vel.TG_FY - Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	Wheel_D.TG = Vel.TG_FX - Vel.TG_FY + Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	
	//利用PID算法计算电机PWM值
	Wheel_A.PWM = SPEED_PidCtlA(Wheel_A.TG, Wheel_A.RT);   
	Wheel_B.PWM = SPEED_PidCtlB(Wheel_B.TG, Wheel_B.RT);  
	Wheel_C.PWM = SPEED_PidCtlC(Wheel_C.TG, Wheel_C.RT);  
	Wheel_D.PWM = SPEED_PidCtlD(Wheel_D.TG, Wheel_D.RT);  
	
	//设置电机PWM值
	//MOTOR_A_SetSpeed( -Wheel_A.PWM);
	//MOTOR_B_SetSpeed( Wheel_B.PWM);  
	//MOTOR_C_SetSpeed(-Wheel_C.PWM);
	//MOTOR_D_SetSpeed(Wheel_D.PWM); 
	
//	printf("A%f B%f C%f D%f \r\n ",Wheel_A.RT,Wheel_B.RT, Wheel_C.RT, Wheel_D.RT  );
//	printf("A%f B%f C%f D%f \r\n ",Wheel_A.TG,Wheel_B.TG, Wheel_C.TG, Wheel_D.TG  );
	//printf("A%d B%d C%d  \r\n ",Vel.I_X, Vel.I_Y, Vel.I_W );
}
#endif


#if (ROBOT_TYPE == ROBOT_FWD)
/**
  * @简  述  机器人运动学处理-四轮差速
  * @参  数  无
  * @返回值  无
  */
void ROBOT_Kinematics(void)
{

	//通过编码器获取车轮实时转速m/s
	Wheel_A.RT = (float)-((int16_t)ENCODER_A_GetCounter()*FWD_WHEEL_SCALE);
	ENCODER_A_SetCounter(0);
	Wheel_B.RT = (float) ((int16_t)ENCODER_B_GetCounter()*FWD_WHEEL_SCALE);
	ENCODER_B_SetCounter(0);		
	Wheel_C.RT = (float)-((int16_t)ENCODER_C_GetCounter()*FWD_WHEEL_SCALE);
	ENCODER_C_SetCounter(0);
	Wheel_D.RT = (float) ((int16_t)ENCODER_D_GetCounter()*FWD_WHEEL_SCALE);
	ENCODER_D_SetCounter(0);
	
	//调试输出轮子转速
	//printf("@%f  %f   \r\n",Wheel_A.RT,Wheel_B.RT);
	
	//运动学正解析，由机器人轮子速度计算机器人速度
	Vel.RT_IX = (( Wheel_A.RT + Wheel_B.RT + Wheel_C.RT + Wheel_D.RT)/4)*1000;
	Vel.RT_IY = 0;
	Vel.RT_IW = ((-Wheel_A.RT + Wheel_B.RT - Wheel_C.RT + Wheel_D.RT)/(2*FWD_WB_SCALE*FWD_WHEEL_BASE))*1000;	
	
	//机器人目标速度限制
	if( Vel.TG_IX > R_VX_LIMIT )    Vel.TG_IX = R_VX_LIMIT;
	if( Vel.TG_IX < (-R_VX_LIMIT))  Vel.TG_IX = (-R_VX_LIMIT);
	if( Vel.TG_IY > R_VY_LIMIT)     Vel.TG_IY = R_VY_LIMIT;
	if( Vel.TG_IY < (-R_VY_LIMIT))  Vel.TG_IY = (-R_VY_LIMIT);
	if( Vel.TG_IW > R_VW_LIMIT)     Vel.TG_IW = R_VW_LIMIT;
	if( Vel.TG_IW < (-R_VW_LIMIT))  Vel.TG_IW = (-R_VW_LIMIT);
	
	//目标速度转化为浮点类型
	Vel.TG_FX = Vel.TG_IX/1000.0;
	Vel.TG_FY = 0;
	Vel.TG_FW = Vel.TG_IW/1000.0;
	
	//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
	Wheel_A.TG = Vel.TG_FX - Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	Wheel_B.TG = Vel.TG_FX + Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	Wheel_C.TG = Vel.TG_FX - Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	Wheel_D.TG = Vel.TG_FX + Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	
	
	//利用PID算法计算电机PWM值
	Wheel_A.PWM = SPEED_PidCtlA(Wheel_A.TG, Wheel_A.RT);   
	Wheel_B.PWM = SPEED_PidCtlB(Wheel_B.TG, Wheel_B.RT);  
	Wheel_C.PWM = SPEED_PidCtlC(Wheel_C.TG, Wheel_C.RT);  
	Wheel_D.PWM = SPEED_PidCtlD(Wheel_D.TG, Wheel_D.RT);  
	
	if(Vel.TG_IX ==0 && Vel.TG_IW ==0)
	{
		Wheel_A.PWM = 0;   
		Wheel_B.PWM = 0;  
		Wheel_C.PWM = 0;  
		Wheel_D.PWM = 0;  	
	}
	
	//设置电机PWM值
	MOTOR_A_SetSpeed(  Wheel_A.PWM);
	MOTOR_B_SetSpeed(  Wheel_B.PWM);  
	MOTOR_C_SetSpeed( -Wheel_C.PWM);
	MOTOR_D_SetSpeed( -Wheel_D.PWM); 
	
	//输出调试信息
	//printf("@%f  %f  %f %f  \r\n",Wheel_A.RT,Wheel_B.RT, Wheel_C.RT ,Wheel_D.RT);
	//printf("A%d B%d C%d  \r\n ",Vel.I_X, Vel.I_Y, Vel.I_W );
}
#endif



#if (ROBOT_TYPE == ROBOT_AKM)
/**
  * @简  述  机器人运动学处理-阿克曼
  * @参  数  无
  * @返回值  无
  */
void ROBOT_Kinematics(void)
{

	//舵机角度
	int16_t servo_angle;
	
	//通过编码器获取车轮实时转速m/s
	Wheel_A.RT = (float)-((int16_t)ENCODER_A_GetCounter()*AKM_WHEEL_SCALE);
	ENCODER_A_SetCounter(0);
	Wheel_B.RT = (float) ((int16_t)ENCODER_B_GetCounter()*AKM_WHEEL_SCALE);
	ENCODER_B_SetCounter(0);	
	
	//调试输出轮子转速
	//printf("@%f  %f   \r\n",Wheel_A.RT,Wheel_B.RT);
	
	//运动学正解析，由机器人轮子速度计算机器人速度
	Vel.RT_IX = ((Wheel_A.RT + Wheel_B.RT)/2)*1000;
	Vel.RT_IY = 0;
	Vel.RT_IW = ((-Wheel_A.RT + Wheel_B.RT)/AKM_WHEEL_BASE)*1000;	

	//机器人目标速度限制
	if( Vel.TG_IX > R_VX_LIMIT )    Vel.TG_IX = R_VX_LIMIT;
	if( Vel.TG_IX < (-R_VX_LIMIT))  Vel.TG_IX = (-R_VX_LIMIT);
	if( Vel.TG_IY > R_VY_LIMIT)     Vel.TG_IY = R_VY_LIMIT;
	if( Vel.TG_IY < (-R_VY_LIMIT))  Vel.TG_IY = (-R_VY_LIMIT);
	if( Vel.TG_IW > R_VW_LIMIT)     Vel.TG_IW = R_VW_LIMIT;
	if( Vel.TG_IW < (-R_VW_LIMIT))  Vel.TG_IW = (-R_VW_LIMIT);
	
	//目标速度转化为浮点类型
	Vel.TG_FX = Vel.TG_IX/1000.0;
	Vel.TG_FY = Vel.TG_IY/1000.0;
	Vel.TG_FW = Vel.TG_IW/1000.0;
	
	if(Vel.TG_IW != 0)
	{
		//判断机器人前进速度是否为0
		if( Vel.TG_IX != 0)
		{
			//计算转弯半径
			RobotStr.Radius =  Vel.TG_FX/Vel.TG_FW;
			
			//阿克曼机器人需要设置最小转弯半径
	        //如果目标速度要求的转弯半径小于最小转弯半径，
	        //会导致机器人运动摩擦力大大提高，严重影响控制效果

			//转弯半径小于最小转弯
			if(RobotStr.Radius>0 && RobotStr.Radius<AKM_TURN_R_MINI)
			{
				RobotStr.Radius = AKM_TURN_R_MINI; 
				
			}
				
			else if(RobotStr.Radius<0 && RobotStr.Radius>(-AKM_TURN_R_MINI))
			{
				RobotStr.Radius = -AKM_TURN_R_MINI;
			}
			
			//计算机器人前轮转向角度,单位弧度
			RobotStr.Angle = atan(AKM_ACLE_BASE/(RobotStr.Radius));				
				
			//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
			Wheel_A.TG = Vel.TG_FX*(RobotStr.Radius-0.5*AKM_WHEEL_BASE)/RobotStr.Radius;
			Wheel_B.TG = Vel.TG_FX*(RobotStr.Radius+0.5*AKM_WHEEL_BASE)/RobotStr.Radius;				
		}
		else
		{
			Wheel_A.TG = 0;
			Wheel_B.TG = 0;
			RobotStr.Radius = 0;
			RobotStr.Angle = 0; 
		}
	}
	else
	{
		Wheel_A.TG = Vel.TG_FX;
		Wheel_B.TG = Vel.TG_FX;
		RobotStr.Radius = 0;
		RobotStr.Angle = 0;
	}
	
	//根据前轮角度计算右前轮角度
	if(RobotStr.Angle !=0 )
	{
		RobotStr.RAngle = (atan(AKM_ACLE_BASE/((AKM_ACLE_BASE/tan(RobotStr.Angle*0.01745))+0.5*AKM_WHEEL_BASE)))*(180.0/PI);
	}
	else
	{
		RobotStr.RAngle = 0;
	}
	
    //根据右前轮角度，计算舵机转向角度
	RobotStr.SAngle = 	-(0.0041*RobotStr.RAngle*RobotStr.RAngle + 1.2053*RobotStr.RAngle)*180/PI;
	
	//根据舵机转向角度，计算舵机PWM控制量
	servo_angle = (RobotStr.SAngle*10 + 900 + ax_servo_offset);  
	
	//利用PID算法计算电机PWM值
	Wheel_A.PWM = SPEED_PidCtlA(Wheel_A.TG, Wheel_A.RT);   
	Wheel_B.PWM = SPEED_PidCtlB(Wheel_B.TG, Wheel_B.RT);   
		
	//设置电机PWM值
	MOTOR_A_SetSpeed(Wheel_A.PWM);
	MOTOR_B_SetSpeed(Wheel_B.PWM);  
	
	//设置舵机角度，S1,S2任意一个都可以
	SERVO_S1_SetAngle(servo_angle);
	SERVO_S2_SetAngle(servo_angle);
}
#endif


#if (ROBOT_TYPE == ROBOT_TWD)
/**
  * @简  述  机器人运动学处理-两轮差速
  * @参  数  无
  * @返回值  无
  */
void ROBOT_Kinematics(void)
{
	
	//通过编码器获取车轮实时转速m/s
	Wheel_A.RT = (float)-((int16_t)ENCODER_A_GetCounter()*MEC_WHEEL_SCALE);
	ENCODER_A_SetCounter(0);
	Wheel_B.RT = (float) ((int16_t)ENCODER_B_GetCounter()*MEC_WHEEL_SCALE);
	ENCODER_B_SetCounter(0);			
	
	//调试输出轮子转速
	//printf("@%f  %f   \r\n",Wheel_A.RT,Wheel_B.RT);
	
	//运动学正解析，由机器人轮子速度计算机器人速度
	Vel.RT_IX = ((Wheel_A.RT + Wheel_B.RT)/2)*1000;
	Vel.RT_IY = 0;
	Vel.RT_IW = ((-Wheel_A.RT + Wheel_B.RT)/TWD_WHEEL_BASE)*1000;		
	
	//机器人目标速度限制
	if( Vel.TG_IX > R_VX_LIMIT )    Vel.TG_IX = R_VX_LIMIT;
	if( Vel.TG_IX < (-R_VX_LIMIT))  Vel.TG_IX = (-R_VX_LIMIT);
	if( Vel.TG_IY > R_VY_LIMIT)     Vel.TG_IY = R_VY_LIMIT;
	if( Vel.TG_IY < (-R_VY_LIMIT))  Vel.TG_IY = (-R_VY_LIMIT);
	if( Vel.TG_IW > R_VW_LIMIT)     Vel.TG_IW = R_VW_LIMIT;
	if( Vel.TG_IW < (-R_VW_LIMIT))  Vel.TG_IW = (-R_VW_LIMIT);
	
	//目标速度转化为浮点类型
	Vel.TG_FX = Vel.TG_IX/1000.0;
	Vel.TG_FY = 0;
	Vel.TG_FW = Vel.TG_IW/1000.0;
	
	//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
	Wheel_A.TG = Vel.TG_FX - Vel.TG_FW*(TWD_WHEEL_BASE/2);
	Wheel_B.TG = Vel.TG_FX + Vel.TG_FW*(TWD_WHEEL_BASE/2);	
	

	//利用PID算法计算电机PWM值
	Wheel_A.PWM = SPEED_PidCtlA(Wheel_A.TG, Wheel_A.RT);   
	Wheel_B.PWM = SPEED_PidCtlB(Wheel_B.TG, Wheel_B.RT);  
 
	//设置电机PWM值
	MOTOR_A_SetSpeed( Wheel_A.PWM);
	MOTOR_B_SetSpeed( Wheel_B.PWM);  
 
	
	//printf("A%f B%f  \r\n ",MOTOR_A.Wheel_RT, MOTOR_B.Wheel_RT  );
	//printf("A%d B%d C%d  \r\n ",Vel.I_X, Vel.I_Y, Vel.I_W );	
	
	
}
#endif

/**
  * @简  述  机器人停止运动状态
  * @参  数  无
  * @返回值  无
  */
void ROBOT_Stop(void)
{
	
	//设置电机PWM值
	MOTOR_A_SetSpeed(0);
	MOTOR_B_SetSpeed(0);  
	MOTOR_C_SetSpeed(0);
	MOTOR_D_SetSpeed(0);  
	
}

