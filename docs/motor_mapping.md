# 电机映射记录

## 底层接口

源码位置：

```text
firmware/SJRobot_center/Hardware_program/y_motor.c
```

已有 4 路电机控制函数：

```c
MOTOR_A_SetSpeed(int16_t speed);
MOTOR_B_SetSpeed(int16_t speed);
MOTOR_C_SetSpeed(int16_t speed);
MOTOR_D_SetSpeed(int16_t speed);
```

## 速度范围

源码速度范围约为：

```text
-4200 ~ 4200
```

第一阶段调试不要直接满速，建议从低速开始：

```text
300 -> 500 -> 800
```

## 麦克纳姆轮运动学

源码位置：

```text
firmware/SJRobot_center/Motorcontrol_program/kinematics.c
```

默认配置：

```c
#define ROBOT_TYPE ROBOT_MEC
```

麦轮公式：

```c
Wheel_A.TG = Vel.TG_FX - Vel.TG_FY - Vel.TG_FW * K;
Wheel_B.TG = Vel.TG_FX + Vel.TG_FY + Vel.TG_FW * K;
Wheel_C.TG = Vel.TG_FX + Vel.TG_FY - Vel.TG_FW * K;
Wheel_D.TG = Vel.TG_FX - Vel.TG_FY + Vel.TG_FW * K;
```

输出到电机时：

```c
MOTOR_A_SetSpeed(-Wheel_A.PWM);
MOTOR_B_SetSpeed( Wheel_B.PWM);
MOTOR_C_SetSpeed(-Wheel_C.PWM);
MOTOR_D_SetSpeed( Wheel_D.PWM);
```

A/C 做了反向，后续需要和实际电机方向表核对。
##a轮对应左前300是向后转b轮对应右前300是向前转下面同上