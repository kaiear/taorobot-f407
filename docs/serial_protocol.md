# USART2 串口协议记录

## 串口用途

USART2 接树莓派，后续 ROS 上位机优先通过该串口和 STM32 通信。

```text
USART2 = PD5/PD6
Baudrate = 115200
```

源码初始化：

```c
UART2_Init(115200);
```

## 源码位置

```text
firmware/SJRobot_center/Drive_program/uart2.c
```

## 帧头

接收协议帧头：

```text
0xAA 0x55
```

## 已有 ID 分类

```text
ID_ROS2STM_VEL    // 上位机发速度
ID_ROS2STM_IMU    // IMU 校准
ID_ROS2STM_HAND   // 夹爪/手部
ID_ROS2STM_ARM    // 机械臂
ID_ROS2STM_KEY    // 键盘机械臂控制
ID_ROS2STM_IK     // 机械臂逆运动学控制
ID_ROS2STM_BEEP   // 蜂鸣器
ID_ROS2STM_RESET  // 复位
```

## 底盘速度链路

上位机速度帧最终写入：

```c
Vel.TG_IX = ...;
Vel.TG_IY = ...;
Vel.TG_IW = ...;
```

然后由运动学计算电机 PWM：

```text
ROS/树莓派
  -> USART2
  -> uart2.c
  -> Vel.TG_IX / Vel.TG_IY / Vel.TG_IW
  -> ROBOT_Kinematics()
  -> MOTOR_A/B/C/D_SetSpeed()
```
