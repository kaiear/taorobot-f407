#ifndef __ROBOT_H
#define __ROBOT_H

#define ADC_REVISE  99.6

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//C库函数的相关头文件
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

 
//外设相关头文件
#include "sys.h"      //系统设置
#include "y_beep.h"     //蜂鸣器控制
#include "y_switch.h"    //按键检测 

#include "y_servo.h"    //舵机控制
#include "y_motor.h"    //直流电机调速控制
#include "y_mpu6050.h"  //IMU加速度陀螺仪测量


//机器人轮子速度数据结构体
typedef struct  
{
	double  RT;       //车轮实时速度，单位m/s
	float  TG;       //车轮目标速度，单位m/s
	short  PWM;      //车轮PWM控制速度
	
}ROBOT_Wheel;

//机器人速度结构体
typedef struct  
{
	short  RT_IX;     //实时X轴速度（16位整数）
	short  RT_IY;     //实时Y轴速度（16位整数）
	short  RT_IW;     //实时Yaw旋转轴速度（16位整数）
	
	short  TG_IX;     //目标X轴速度（16位整数）
	short  TG_IY;     //目标Y轴速度（16位整数）
	short  TG_IW;     //目标Yaw旋转轴速度（16位整数）
	
	float  RT_FX;     //实时X轴速度（浮点）
	float  RT_FY;     //实时Y轴速度（浮点）
	float  RT_FW;     //实时Yaw旋转轴速度（浮点）
	
	float  TG_FX;     //目标X轴速度（浮点）
	float  TG_FY;     //目标Y轴速度（浮点）
	float  TG_FW;     //目标Yaw旋转轴速度（浮点）
	
}ROBOT_Velocity;


//机器人IMU数据
typedef struct  
{
	short  ACC_X;     //X轴
	short  ACC_Y;     //Y轴
	short  ACC_Z;     //Z轴
	
	short  GYRO_X;     //X轴
	short  GYRO_Y;     //Y轴
	short  GYRO_Z;     //Z轴
	
}ROBOT_Imu;


//RGB灯效数据
typedef struct  
{
	uint8_t  M;    //灯效主模式
	uint8_t  S;    //灯效从模式
	uint8_t  T;    //灯效时间参数
	
	uint8_t  R;     //灯效颜色 R
	uint8_t  G;     //灯效颜色 G
	uint8_t  B;     //灯效颜色 B
	
}ROBOT_Light;


//阿克曼转向结构体
typedef struct  
{
	float  Radius;     //转弯半径
	float  Angle;      //机器人转向角度
	float  RAngle;     //前右轮转向角度
	float  SAngle;     //舵机角度
	
}ROBOT_Steering;

//机械臂参数

//电池
#define  VBAT_40P    1065     //电池40%电压
#define  VBAT_20P    1012     //电池20%电压
#define  VBAT_10P    984      //电池10%电压

#define  PID_RATE        100        //PID频率

//机器人类型
#define  ROBOT_MEC  1
#define  ROBOT_FWD  2
#define  ROBOT_AKM  3
#define  ROBOT_TWD  4

//电机编码器分辨率
#define  WHEEL_RESOLUTION     1320.0f    //1024线磁编码器分辨率,1204x30x4=122880    开关霍尔：11*4*30（减速比）= 1320

//麦轮机器人参数
#define  MEC_WHEEL_BASE           0.195f	      //轮距，左右轮的距离
#define  MEC_ACLE_BASE            0.175f       //轴距，前后轮的距离
#define  MEC_WHEEL_DIAMETER	      0.08f		  //轮子直径
#define  MEC_WHEEL_SCALE          (PI*MEC_WHEEL_DIAMETER*PID_RATE/WHEEL_RESOLUTION) //轮子速度m/s与编码器转换系数

//四轮差速机器人参数
#define  FWD_WHEEL_BASE           0.200	     //轮距，左右轮的距离
#define  FWD_WB_SCALE             1.75       //轮距系数，轮距系数与机器人的总负载、轮胎与地面的相对摩擦系数、转弯半径及质心位置都是有关系是一个非常复杂的参数，所以常用的方法就是做实验
#define  FWD_WHEEL_DIAMETER	      0.100		 //轮子直径
#define  FWD_WHEEL_SCALE          (PI*FWD_WHEEL_DIAMETER*PID_RATE/WHEEL_RESOLUTION) //轮子速度m/s与编码器转换系数

//麦轮机器人参数
#define  AKM_WHEEL_BASE           0.165	     //轮距，左右轮的距离
#define  AKM_ACLE_BASE            0.175f     //轴距，前后轮的距离
#define  AKM_WHEEL_DIAMETER	      0.075		 //轮子直径
#define  AKM_TURN_R_MINI          0.35f      //最小转弯半径( L*cot30-W/2)
#define  AKM_WHEEL_SCALE          (PI*AKM_WHEEL_DIAMETER*PID_RATE/WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数

//两轮差速机器人参数 
#define  TWD_WHEEL_DIAMETER	      0.0724	 //轮子直径
#define  TWD_WHEEL_BASE           0.206	     //轮距，左右轮的距离
#define  TWD_WHEEL_SCALE          (PI*TWD_WHEEL_DIAMETER*PID_RATE/WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数

//机器人速度限制
#define R_VX_LIMIT  1500   //X轴速度限值 m/s*1000
#define R_VY_LIMIT  1200   //Y轴速度限值 m/s*1000
#define R_VW_LIMIT  6280   //W旋转角速度限值 rad/s*1000

//机器人通信帧头定义
#define  ID_STM2ROS_DATA    0x10    //下位计向ROS发送的综合数据
#define  ID_ROS2STM_VEL     0x50    //ROS向下位机发送的速度数据
#define  ID_ROS2STM_IMU     0x51    //ROS向下位机发送的IMU陀螺仪零偏校准指令
#define  ID_ROS2STM_LIGHT   0x52    //ROS向下位机发送的灯光控制指令
#define  ID_ROS2STM_AKM     0x5f    //ROS向下位机发送的AKM机器人舵机零偏补偿角，其它类型忽略
#define  ID_ROS2STM_HAND    0x60    //ROS向下位机发送的夹爪角度数据
#define  ID_ROS2STM_ARM     0x70    //ROS向下位机发送的机械臂角度数据
#define  ID_ROS2STM_KEY     0x80    //ROS向下位机发送的键盘控制机械臂数据
#define  ID_ROS2STM_IK      0x90    //ROS向下位机发送的逆运动学控制机械臂数据
#define  ID_ROS2STM_BEEP    0x66    //ROS向下位机发送的蜂鸣器数据
#define  ID_ROS2STM_RESET   0x6f    //ROS向下位机发送的机械臂底盘复位


//灯光模式
#define  LEFFECT1    0x01    //单色模式  
#define  LEFFECT2    0x02    //呼吸模式  
#define  LEFFECT3    0x03    //彩色模式  
#define  LEFFECT4    0x04    //警灯模式 
#define  LEFFECT5    0x05    //定义模式  
#define  LEFFECT6    0x06    //定义模式


typedef struct {
	float   gpitch;
	float   groll;
	float   gyaw;
	
	short   gGyro1;
	short   gGyro2;
	short   gGyro3;
	
	float   Angle_Old;
	
	short   gAx;
	short   gAy;
	short   gAz;
	float   Temp;	
	
 }blance_samp;

#define  blance_samp_DEFAULTS  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0,0,0,0.0}  

extern  blance_samp   blance_sampPara;


//机器人关键全局变量
extern  ROBOT_Velocity  Vel; //机器人速度数据
extern  ROBOT_Wheel  Wheel_A,Wheel_B,Wheel_C,Wheel_D; //机器人轮子数据
extern  ROBOT_Imu  Imu; //机器人IMU数据
extern  ROBOT_Light  Light; //机器人RGB数据
extern  uint16_t Bat_Vol;  //机器人电池电压数据


//全局变量
extern int16_t imu_acc_data[3];  
extern int16_t imu_gyro_data[3];
extern int16_t imu_gyro_offset[3];   

extern int16_t motor_kp;  
extern int16_t motor_ki;    
extern int16_t motor_kd; 

extern int8_t imu_calibrate_flag;
extern int8_t uart_flag;
extern int8_t model_flag;

//阿克曼机器人
extern int16_t servo_offset;

//阿克曼机器人
extern int16_t arm_angle[7];          //机械臂关节角度
extern int16_t arm_angle_mid[7];   //机械臂角度偏差

extern int16_t times;
extern int16_t on_time;
extern int16_t off_time;

//ros舵机数据
typedef struct
{
    uint16_t pwm[6];  // 执行目标
    uint16_t time[6]; // 执行时间
	  uint16_t angle[6];
} ros_servo_t;
extern ros_servo_t ros_servo;

void Robot_Task(void);
void Bat_Task(void);
void Key_Task(void);
void Imu_Task(void);
void ROBOT_ArmControl(void);
#endif

