# 下位机 3 天跑通计划：电机、舵机、串口、遥控器

本文档用于安排两名组员在 3 天内完成 OpenRF4 / STM32F407VET6 控制板下位机基础跑通工作。

目标不是一开始做复杂功能，而是先把下位机最关键的硬件链路跑通，并为后续上位机代码开发交接出明确资料。

---

## 1. 总目标

3 天结束时，需要达到以下状态：

1. Keil 工程能正常编译、烧录、运行。
2. 四路电机 A/B/C/D 能单独 PWM 输出，方向和轮子位置记录清楚。
3. 麦克纳姆轮底盘能完成前进、后退、左右平移、原地旋转。
4. 6 路 PWM 舵机能单独控制，通道、方向、中位、安全范围记录清楚。
5. USART2 能接收上位机/电脑/树莓派发来的控制信号。
6. 能用遥控器/USB 手柄调试底盘。
7. 形成可以交给上位机开发使用的文档和协议说明。

---

## 2. 人员分工

假设两名组员分别为：

- 组员 A：底盘、电机、遥控器负责人
- 组员 B：舵机、串口协议、上位机信号负责人

### 2.1 组员 A 负责内容

组员 A 主要负责底盘相关工作：

1. Keil 编译和烧录验证。
2. 四路电机 A/B/C/D 单独测试。
3. 电机正反转方向记录。
4. 麦克纳姆轮运动方向验证。
5. USB 手柄/遥控器控制底盘验证。
6. 更新 `docs/motor_mapping.md`。
7. 为后续上位机 `/cmd_vel` 底盘控制做准备。

后续可转到上位机开发方向：

- 键盘控制节点。
- `/cmd_vel` 速度控制。
- ROS 底盘控制节点。
- 速度限幅、急停逻辑。
- 手柄输入到上位机控制逻辑。

### 2.2 组员 B 负责内容

组员 B 主要负责舵机和通信相关工作：

1. Keil 编译和烧录验证。
2. 6 路舵机 PWM 输出测试。
3. 每路舵机 index、方向、中位、安全范围记录。
4. USART2 串口收发测试。
5. 上位机/电脑/树莓派发包到 STM32 测试。
6. 更新 `docs/servo_mapping.md` 和 `docs/serial_protocol.md`。
7. 为后续上位机机械臂、夹爪、串口协议封装做准备。

后续可转到上位机开发方向：

- 串口通信 Python/C++ 节点。
- 机械臂控制节点。
- 夹爪控制节点。
- 上位机串口协议封装库。
- MoveIt 或自定义机械臂控制接口。

---

## 3. 代码仓库和工程入口

GitHub 仓库：

```text
https://github.com/kaiear/taorobot-f407.git
```

组员拉取：

```powershell
git clone https://github.com/kaiear/taorobot-f407.git
```

Keil 工程路径：

```text
taorobot-f407\firmware\SJRobot_center\project\SJRobot_center.uvprojx
```

当前控制板：

| 项目 | 内容 |
|---|---|
| 控制板 | OpenRF4 |
| 主控 | STM32F407VET6 |
| 内核 | Cortex-M4 |
| 主频 | 168 MHz |
| Flash | 512 KB |
| SRAM | 192 KB |
| Keil Device | STM32F407VE / STM32F407VETx |

---

## 4. 关键源码位置

| 文件 | 作用 |
|---|---|
| `User_program/main.c` | 程序入口，初始化外设并启动任务循环 |
| `User_program/Task_manager.c` | 周期任务调度 |
| `User_program/app_ps2.c` | USB 手柄/PS2 风格输入处理 |
| `Motorcontrol_program/kinematics.c` | 底盘运动学，当前默认麦克纳姆轮 |
| `Motorcontrol_program/robot.c` | 机器人控制核心逻辑 |
| `Hardware_program/y_motor.c` | 四路电机底层控制 |
| `Hardware_program/y_servo.c` | 六路 PWM 舵机控制 |
| `Drive_program/uart2.c` | USART2，树莓派/ROS 与 STM32 通信入口 |

---

## 5. 3 天压缩时间线

### Day 1：环境、烧录、电机/舵机单项输出

Day 1 的目标是：两个人都能独立拉代码、打开工程、编译、烧录；同时开始单独测试电机和舵机，不做复杂联调。

#### Day 1 上午：共同完成环境跑通

负责人：组员 A + 组员 B

任务：

1. clone GitHub 仓库。
2. Keil 打开 `.uvprojx` 工程。
3. 检查芯片型号。
4. 检查 Include Paths。
5. Build 工程。
6. 连接 JLink/STLink/CMSIS-DAP。
7. 通过 SWD Download。
8. 观察板子基础运行现象。

检查项：

| 项目 | 期望 |
|---|---|
| Git clone | 成功 |
| Keil 工程 | 能打开 |
| Build | 0 Error |
| SWD | 能识别芯片 |
| Download | 成功 |
| 板子运行 | LED、蜂鸣器、串口、手柄任意一种有反应 |

注意：

源码 `main.c` 中可能存在：

```c
Delay_ms(20000);
```

所以烧录后可能需要等待约 20 秒，不要刚烧进去就判断程序没有运行。

#### Day 1 下午：A 测电机，B 测舵机

##### 组员 A：单电机 PWM 测试

目标：

- 四个电机通道都能单独转。
- 记录 A/B/C/D 对应的实际轮子位置。
- 记录正数、负数对应的实际转向。

底层接口：

```c
MOTOR_A_SetSpeed(int16_t speed);
MOTOR_B_SetSpeed(int16_t speed);
MOTOR_C_SetSpeed(int16_t speed);
MOTOR_D_SetSpeed(int16_t speed);
```

速度范围：

```text
-4200 ~ 4200
```

但 Day 1 只允许低速测试：

```text
100 -> 300 -> 500
```

第一次测试必须让车轮悬空。

测试表：

| 电机接口 | 测试值 | 实际轮子位置 | 方向 | 是否正常 |
|---|---:|---|---|---|
| A | 300 | 待测 | 待测 | 待测 |
| A | -300 | 待测 | 待测 | 待测 |
| B | 300 | 待测 | 待测 | 待测 |
| B | -300 | 待测 | 待测 | 待测 |
| C | 300 | 待测 | 待测 | 待测 |
| C | -300 | 待测 | 待测 | 待测 |
| D | 300 | 待测 | 待测 | 待测 |
| D | -300 | 待测 | 待测 | 待测 |

输出文档：

```text
docs/motor_mapping.md
```

##### 组员 B：单舵机 PWM 测试

目标：

- 6 路舵机 PWM 都能输出。
- 记录 index 对应哪个舵机/关节。
- 记录中位、方向、安全范围。

底层接口：

```c
duoji_doing_set(index, aim, time);
```

源码限制：

```c
if (aim > 2490) aim = 2490;
else if (aim < 510) aim = 510;
```

PWM 范围大致：

```text
510 ~ 2490 us
```

但 Day 1 只允许小范围测试：

```text
1300 -> 1500 -> 1700
```

特殊注意：

```c
if(index == 3)
{
    aim = 3000 - aim;
}
```

说明 `index == 3` 在源码中做了反向。

测试表：

| index | 1500us 姿态 | 1300us 方向 | 1700us 方向 | 实际关节 | 是否安全 |
|---:|---|---|---|---|---|
| 0 | 待测 | 待测 | 待测 | 待测 | 待测 |
| 1 | 待测 | 待测 | 待测 | 待测 | 待测 |
| 2 | 待测 | 待测 | 待测 | 待测 | 待测 |
| 3 | 待测 | 待测 | 待测 | 待测，源码反向 | 待测 |
| 4 | 待测 | 待测 | 待测 | 待测 | 待测 |
| 5 | 待测 | 待测 | 待测 | 待测 | 待测 |

输出文档：

```text
docs/servo_mapping.md
```

#### Day 1 晚上提交要求

组员 A：

```bash
git checkout -b feature/motor-remote-test
git add docs/motor_mapping.md
git commit -m "update motor channel test notes"
git push -u origin feature/motor-remote-test
```

组员 B：

```bash
git checkout -b feature/servo-uart-test
git add docs/servo_mapping.md
git commit -m "update servo pwm test notes"
git push -u origin feature/servo-uart-test
```

Day 1 验收：

| 验收项 | 负责人 | 通过标准 |
|---|---|---|
| Keil Build | A/B | 0 Error |
| SWD Download | A/B | 下载成功 |
| 单电机输出 | A | A/B/C/D 至少能低速转动 |
| 单舵机输出 | B | 6 路至少能到 1500us |
| 文档更新 | A/B | 电机/舵机测试表有记录 |

---

### Day 2：底盘运动、遥控器、USART2 通信

Day 2 的目标是：A 把底盘运动和遥控器跑通；B 把 USART2 通信跑通。晚上两个人开始第一次联调。

#### Day 2 上午：A 测麦轮运动和遥控器

##### 组员 A：麦克纳姆轮运动验证

当前代码默认：

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

实际输出：

```c
MOTOR_A_SetSpeed(-Wheel_A.PWM);
MOTOR_B_SetSpeed( Wheel_B.PWM);
MOTOR_C_SetSpeed(-Wheel_C.PWM);
MOTOR_D_SetSpeed( Wheel_D.PWM);
```

注意：A/C 在代码中已经反向。

测试动作：

| 动作 | 期望结果 | 是否通过 | 备注 |
|---|---|---|---|
| 前进 | 车向前 | 待测 | 低速 |
| 后退 | 车向后 | 待测 | 低速 |
| 左平移 | 车向左横移 | 待测 | 低速 |
| 右平移 | 车向右横移 | 待测 | 低速 |
| 左旋转 | 原地逆时针 | 待测 | 低速 |
| 右旋转 | 原地顺时针 | 待测 | 低速 |
| 停止 | 电机停止 | 待测 | 必测 |

建议测试速度：

```text
300 -> 500 -> 800
```

Day 2 仍然不建议超过：

```text
1200
```

##### 组员 A：遥控器/USB 手柄测试

源码链路：

```text
USB 手柄
  ↓
app_ps2.c
  ↓
Vel.TG_IX / Vel.TG_IY / Vel.TG_IW
  ↓
ROBOT_Kinematics()
  ↓
MOTOR_A/B/C/D_SetSpeed()
```

重点变量：

```c
Vel.TG_IX  // 前后速度
Vel.TG_IY  // 左右平移速度
Vel.TG_IW  // 旋转速度
```

测试表：

| 手柄动作 | 期望结果 | 是否通过 | 备注 |
|---|---|---|---|
| 左摇杆上推 | 前进 | 待测 | 低速 |
| 左摇杆下拉 | 后退 | 待测 | 低速 |
| 左摇杆左推 | 左平移 | 待测 | 低速 |
| 左摇杆右推 | 右平移 | 待测 | 低速 |
| 右摇杆左推 | 左旋转 | 待测 | 低速 |
| 右摇杆右推 | 右旋转 | 待测 | 低速 |
| 摇杆松开 | 停止 | 待测 | 必测 |

输出文档：

```text
docs/motor_mapping.md
```

#### Day 2 上午到下午：B 测 USART2 裸串口

##### 组员 B：USART2 基础信息

| 项目 | 内容 |
|---|---|
| 串口 | USART2 |
| 引脚 | PD5/PD6 |
| 用途 | 接树莓派/上位机 |
| 波特率 | 115200 |
| 协议帧头 | `0xAA 0x55` |

关键源码：

```text
Drive_program/uart2.c
```

协议 ID 包括：

```c
ID_ROS2STM_VEL
ID_ROS2STM_IMU
ID_ROS2STM_HAND
ID_ROS2STM_ARM
ID_ROS2STM_KEY
ID_ROS2STM_IK
ID_ROS2STM_BEEP
ID_ROS2STM_RESET
```

##### 组员 B：裸串口收发测试

目标：

1. 电脑能打开 COM 口。
2. 串口助手能以 115200 发送数据。
3. STM32 能收到数据。
4. STM32 能通过 LED、蜂鸣器或回传串口证明收到数据。

建议先做最小 debug：

- 收到固定字符 `B`，蜂鸣器响一下。
- 收到固定字节 `0x55`，LED 翻转。
- 或 STM32 每秒发一次心跳。

测试表：

| 测试项 | 通过标准 | 是否通过 | 备注 |
|---|---|---|---|
| COM 口识别 | 电脑能看到串口 | 待测 | COMx |
| 115200 打开 | 串口助手能打开 | 待测 | 8N1 |
| STM32 接收 | 收到固定字节有反应 | 待测 | LED/蜂鸣器 |
| STM32 发送 | PC 能收到回传 | 待测 | 可选 |
| 稳定性 | 连续 1 分钟无异常 | 待测 | 可选 |

输出文档：

```text
docs/serial_protocol.md
```

#### Day 2 下午：B 测协议帧接收

目标：

- 发送以 `0xAA 0x55` 开头的数据包。
- STM32 能进入 `uart2.c` 对应解析逻辑。
- 至少验证一个简单 ID，例如蜂鸣器、复位、速度或舵机相关指令。

建议测试优先级：

1. 蜂鸣器/LED 指令：风险最低。
2. Stop 指令：用于安全验证。
3. 速度指令：低速控制底盘。
4. 舵机指令：小范围控制舵机。

测试表：

| 协议测试 | 期望结果 | 是否通过 | 备注 |
|---|---|---|---|
| 帧头识别 | `0xAA 0x55` 能被识别 | 待测 | 必测 |
| 蜂鸣器/LED | 板子有反应 | 待测 | 低风险 |
| Stop | 电机停止 | 待测 | 必测 |
| 速度包 | `Vel.TG_IX/IY/IW` 改变 | 待测 | 低速 |
| 舵机包 | 舵机小范围动作 | 待测 | 小范围 |

#### Day 2 晚上：第一次联调

组员 A + B 一起完成：

| 联调项 | 负责人 | 通过标准 |
|---|---|---|
| 上位机发 stop | A/B | 电机立即停止 |
| 上位机发低速前进 | A/B | 底盘低速前进 |
| 上位机发低速后退 | A/B | 底盘低速后退 |
| 上位机发舵机中位 | B | 指定舵机到安全中位 |
| 手柄控制底盘 | A | 能独立控制 |
| 串口和手柄切换 | A/B | 不互相造成危险动作 |

Day 2 验收：

| 验收项 | 负责人 | 通过标准 |
|---|---|---|
| 麦轮底盘运动 | A | 前后、横移、旋转基本正确 |
| 遥控器控制 | A | 能控制底盘，松手停止 |
| USART2 裸通信 | B | PC/树莓派能和 STM32 通信 |
| 协议帧识别 | B | `0xAA 0x55` 可解析 |
| 第一次联调 | A/B | 上位机至少能触发一种安全动作 |

---

### Day 3：完整联调、文档补齐、上位机交接

Day 3 的目标是：把下位机测试结果收口，避免留下不安全测试代码，并输出可以转上位机开发的交接资料。

#### Day 3 上午：完整功能联调

##### 联调 1：上位机速度控制底盘

目标链路：

```text
电脑/树莓派
  ↓ 串口
USART2
  ↓
uart2.c
  ↓
Vel.TG_IX / Vel.TG_IY / Vel.TG_IW
  ↓
ROBOT_Kinematics()
  ↓
MOTOR_A/B/C/D_SetSpeed()
```

测试表：

| 上位机命令 | 期望底盘动作 | 是否通过 | 备注 |
|---|---|---|---|
| stop | 停止 | 待测 | 必测 |
| forward low | 低速前进 | 待测 | 低速 |
| backward low | 低速后退 | 待测 | 低速 |
| left low | 左平移 | 待测 | 低速 |
| right low | 右平移 | 待测 | 低速 |
| rotate left low | 左旋转 | 待测 | 低速 |
| rotate right low | 右旋转 | 待测 | 低速 |

验收标准：

- 所有动作都能执行。
- 方向正确或已记录修正方案。
- stop 指令有效。
- 速度有上限，不会突然高速。

##### 联调 2：上位机控制舵机/夹爪

目标链路：

```text
电脑/树莓派
  ↓ 串口
USART2
  ↓
uart2.c
  ↓
舵机/机械臂控制分支
  ↓
duoji_doing_set(index, aim, time)
```

测试表：

| 上位机命令 | 期望动作 | 是否通过 | 备注 |
|---|---|---|---|
| servo 0 middle | Servo 0 到中位 | 待测 | 安全 |
| servo 1 middle | Servo 1 到中位 | 待测 | 安全 |
| servo 2 middle | Servo 2 到中位 | 待测 | 安全 |
| servo 3 middle | Servo 3 到中位 | 待测 | 注意反向 |
| servo 4 middle | Servo 4 到中位 | 待测 | 安全 |
| servo 5 middle | Servo 5 到中位 | 待测 | 安全 |
| gripper open | 夹爪张开 | 待测 | 小范围 |
| gripper close | 夹爪闭合 | 待测 | 小范围 |

验收标准：

- 每路舵机可控。
- 每路安全范围明确。
- index 3 反向逻辑记录清楚。
- 不允许出现顶死、卡死后仍持续输出的情况。

##### 联调 3：遥控器作为独立调试手段

测试表：

| 遥控器动作 | 期望 | 是否通过 |
|---|---|---|
| 前进 | 底盘前进 | 待测 |
| 后退 | 底盘后退 | 待测 |
| 左平移 | 底盘左移 | 待测 |
| 右平移 | 底盘右移 | 待测 |
| 左旋转 | 原地左转 | 待测 |
| 右旋转 | 原地右转 | 待测 |
| 松手 | 停止 | 待测 |

验收标准：

- 即使上位机还没写完，也能用遥控器独立调底盘。
- 遥控器控制和串口控制之间不会产生危险冲突。

#### Day 3 下午：收口、文档、合并准备

##### 1. 禁用危险测试代码

所有测试代码必须满足：

- 默认上电不能自动跑电机。
- 默认上电不能大幅转舵机。
- 测试功能必须用宏开关控制。
- 合并到 `main` 前，测试宏默认关闭。

建议宏：

```c
#define ENABLE_MOTOR_TEST      0
#define ENABLE_SERVO_TEST      0
#define ENABLE_UART_DEBUG      0
#define ENABLE_REMOTE_DEBUG    0
```

如果后续要重构，可以建立统一调试入口：

```text
User_program/debug_test.c
User_include/debug_test.h
```

测试模式：

```c
#define DEBUG_TEST_NONE        0
#define DEBUG_TEST_MOTOR       1
#define DEBUG_TEST_SERVO       2
#define DEBUG_TEST_UART        3
#define DEBUG_TEST_REMOTE      4

#define DEBUG_TEST_MODE DEBUG_TEST_NONE
```

##### 2. 文档补齐

组员 A 最终需要补齐：

```text
docs/motor_mapping.md
```

必须包含：

- A/B/C/D 对应的实际轮子位置。
- 正数/负数对应方向。
- 是否需要软件反向。
- 麦轮前后、横移、旋转测试结果。
- 手柄型号和摇杆功能。
- 安全速度建议。
- 已知异常。

组员 B 最终需要补齐：

```text
docs/servo_mapping.md
docs/serial_protocol.md
```

`docs/servo_mapping.md` 必须包含：

- 每路 index 对应关节。
- 每路中位 PWM。
- 每路安全最小/最大 PWM。
- 每路 1300/1700 对应方向。
- index 3 反向说明。
- 夹爪开合 PWM 范围。

`docs/serial_protocol.md` 必须包含：

- USART2 引脚和波特率。
- 帧头 `0xAA 0x55`。
- 已验证的协议 ID。
- 速度控制包测试结果。
- stop 指令测试结果。
- 舵机/夹爪指令测试结果。
- 上位机后续封装建议。

##### 3. Git 提交和合并

组员 A：

```bash
git checkout feature/motor-remote-test
git add docs/motor_mapping.md
git commit -m "complete motor and remote controller validation"
git push
```

组员 B：

```bash
git checkout feature/servo-uart-test
git add docs/servo_mapping.md docs/serial_protocol.md
git commit -m "complete servo and uart validation"
git push
```

合并到 `main` 前检查：

| 检查项 | 要求 |
|---|---|
| Keil Build | 0 Error |
| 默认上电 | 电机不自动转 |
| 默认上电 | 舵机不危险大幅动作 |
| 测试宏 | 默认关闭 |
| 文档 | 已补齐 |
| stop | 有效 |
| 分支 | 已 push 到 GitHub |

---

## 6. 三天下位机总验收表

### 6.1 工程和烧录

| 编号 | 项目 | 负责人 | 截止时间 | 通过标准 |
|---|---|---|---|---|
| 1 | clone 仓库 | A/B | Day 1 上午 | 能拉代码 |
| 2 | Keil 打开工程 | A/B | Day 1 上午 | 能打开 `.uvprojx` |
| 3 | Build | A/B | Day 1 上午 | 0 Error |
| 4 | SWD 连接 | A/B | Day 1 上午 | 能识别芯片 |
| 5 | Download | A/B | Day 1 上午 | 烧录成功 |
| 6 | 基础运行 | A/B | Day 1 上午 | 板子有反应 |

### 6.2 电机和底盘

| 编号 | 项目 | 负责人 | 截止时间 | 通过标准 |
|---|---|---|---|---|
| 7 | A 电机测试 | A | Day 1 下午 | 能正转、反转、停止 |
| 8 | B 电机测试 | A | Day 1 下午 | 能正转、反转、停止 |
| 9 | C 电机测试 | A | Day 1 下午 | 能正转、反转、停止 |
| 10 | D 电机测试 | A | Day 1 下午 | 能正转、反转、停止 |
| 11 | 电机位置映射 | A | Day 1 晚上 | A/B/C/D 对应轮子记录清楚 |
| 12 | 麦轮前后 | A | Day 2 上午 | 前进后退正确 |
| 13 | 麦轮横移 | A | Day 2 上午 | 左右平移正确 |
| 14 | 麦轮旋转 | A | Day 2 上午 | 左右旋转正确 |
| 15 | 安全停止 | A | Day 2 上午 | 停止指令立即停车 |
| 16 | 上位机速度控制 | A/B | Day 3 上午 | 串口发速度，底盘动作 |

### 6.3 舵机

| 编号 | 项目 | 负责人 | 截止时间 | 通过标准 |
|---|---|---|---|---|
| 17 | Servo 0 | B | Day 1 下午 | 1500us 有中位，1300/1700 可动 |
| 18 | Servo 1 | B | Day 1 下午 | 1500us 有中位，1300/1700 可动 |
| 19 | Servo 2 | B | Day 1 下午 | 1500us 有中位，1300/1700 可动 |
| 20 | Servo 3 | B | Day 1 下午 | 已记录源码反向 |
| 21 | Servo 4 | B | Day 1 下午 | 1500us 有中位，1300/1700 可动 |
| 22 | Servo 5 | B | Day 1 下午 | 1500us 有中位，1300/1700 可动 |
| 23 | 舵机安全范围 | B | Day 3 下午 | 每路最小/最大建议值记录清楚 |
| 24 | 上位机舵机控制 | B | Day 3 上午 | 串口发指令，舵机动作 |

### 6.4 串口和上位机信号

| 编号 | 项目 | 负责人 | 截止时间 | 通过标准 |
|---|---|---|---|---|
| 25 | USART2 连接 | B | Day 2 上午 | COM 口能打开 |
| 26 | 裸串口收发 | B | Day 2 下午 | 固定字节能触发反应 |
| 27 | 协议帧解析 | B | Day 2 下午 | `0xAA 0x55` 能被识别 |
| 28 | 速度指令 | A/B | Day 3 上午 | 上位机发速度，底盘动作 |
| 29 | 停止指令 | A/B | Day 3 上午 | 上位机发 stop，底盘停止 |
| 30 | 舵机指令 | B | Day 3 上午 | 上位机能控制舵机 |
| 31 | 蜂鸣器/LED 指令 | B | Day 2 下午 | 可作为通信验证 |

### 6.5 遥控器

| 编号 | 项目 | 负责人 | 截止时间 | 通过标准 |
|---|---|---|---|---|
| 32 | 手柄识别 | A | Day 2 上午 | 插入后有反应 |
| 33 | 摇杆前后 | A | Day 2 上午 | 能控制前后 |
| 34 | 摇杆左右 | A | Day 2 上午 | 能控制横移 |
| 35 | 旋转控制 | A | Day 2 上午 | 能原地旋转 |
| 36 | 松手停止 | A | Day 2 上午 | 安全停止 |

### 6.6 文档和交接

| 编号 | 文档/产物 | 负责人 | 截止时间 | 要求 |
|---|---|---|---|---|
| 37 | `docs/motor_mapping.md` | A | Day 3 下午 | 电机、底盘、遥控器记录完整 |
| 38 | `docs/servo_mapping.md` | B | Day 3 下午 | 舵机 index、中位、安全范围完整 |
| 39 | `docs/serial_protocol.md` | B | Day 3 下午 | 串口协议和测试结果完整 |
| 40 | 测试视频/照片 | A/B | Day 3 下午 | 关键测试有记录 |
| 41 | 已知问题列表 | A/B | Day 3 下午 | 未解决问题写清楚 |
| 42 | 上位机交接说明 | A/B | Day 3 下午 | 后续开发能直接接手 |

---

## 7. 下位机完成后转上位机开发

### 7.1 组员 A 转上位机底盘控制

组员 A 负责上位机底盘链路：

```text
/cmd_vel
  ↓
上位机串口协议封装
  ↓
USART2
  ↓
STM32
  ↓
ROBOT_Kinematics()
  ↓
电机 PWM
```

A 需要交接/使用的信息：

| 信息 | 来源 |
|---|---|
| 电机 A/B/C/D 实际轮子位置 | `docs/motor_mapping.md` |
| 电机正负方向 | `docs/motor_mapping.md` |
| 麦轮运动方向是否正确 | `docs/motor_mapping.md` |
| 安全速度范围 | `docs/motor_mapping.md` |
| stop 指令 | `docs/serial_protocol.md` |
| 速度协议格式 | `docs/serial_protocol.md` |

A 的上位机任务：

1. 写键盘控制脚本。
2. 写 `/cmd_vel` 到串口协议转换节点。
3. 做速度限幅。
4. 做急停逻辑。
5. 做底盘运动测试脚本。

### 7.2 组员 B 转上位机机械臂/夹爪/串口协议

组员 B 负责上位机机械臂和协议链路：

```text
机械臂/夹爪命令
  ↓
上位机串口协议封装
  ↓
USART2
  ↓
STM32
  ↓
duoji_doing_set(index, aim, time)
```

B 需要交接/使用的信息：

| 信息 | 来源 |
|---|---|
| 舵机 index 对应关节 | `docs/servo_mapping.md` |
| 每路中位 PWM | `docs/servo_mapping.md` |
| 每路安全范围 | `docs/servo_mapping.md` |
| index 3 反向 | `docs/servo_mapping.md` |
| 舵机协议格式 | `docs/serial_protocol.md` |
| 串口波特率和帧头 | `docs/serial_protocol.md` |

B 的上位机任务：

1. 写串口协议 Python/C++ 封装库。
2. 写单路舵机控制脚本。
3. 写夹爪 open/close 脚本。
4. 写机械臂预设姿态脚本。
5. 后续对接 MoveIt 或自定义机械臂控制节点。

---

## 8. 安全要求

### 8.1 电机安全

1. 第一次测试必须悬空。
2. 速度从 100/300 开始。
3. 不允许直接给 4200。
4. 每个测试必须有停止指令。
5. 默认上电不能自动跑车。
6. 车落地测试时，旁边必须有人能断电。
7. 上位机速度必须限幅。

### 8.2 舵机安全

1. 第一次用 1500us。
2. 小幅测试 1300/1700。
3. 不要直接打到 510/2490。
4. 发现卡死立刻断电。
5. 每个关节记录安全范围。
6. 机械臂附近不要放手指。
7. 合并到主分支前，默认不能自动大幅转动舵机。

### 8.3 串口安全

1. 上位机发速度必须限幅。
2. 串口断开时，下位机应停止或保持安全。
3. 收到异常包不要乱动。
4. stop 指令优先级最高。
5. 未确认协议前，先测试蜂鸣器/LED，再测试电机和舵机。

---

## 9. 每天收工前必须做的事

### Day 1 收工前

组员 A：

- 更新 `docs/motor_mapping.md`。
- 提交 `feature/motor-remote-test`。

组员 B：

- 更新 `docs/servo_mapping.md`。
- 提交 `feature/servo-uart-test`。

### Day 2 收工前

组员 A：

- 补充麦轮运动和遥控器测试记录。
- 确认底盘 stop 有效。

组员 B：

- 补充 USART2 裸串口和协议帧测试记录。
- 确认至少一种低风险协议指令可用。

A + B：

- 完成第一次上位机到 STM32 联调。
- 记录问题列表。

### Day 3 收工前

A + B：

- 完成下位机总验收表。
- 禁用危险测试代码。
- 确认 Keil Build 通过。
- 确认默认上电安全。
- 文档补齐。
- 分支 push 到 GitHub。
- 准备合并到 `main`。
- 明确各自转上位机后的任务。

---

## 10. 最终交付物

3 天结束后，必须交付：

| 交付物 | 负责人 |
|---|---|
| 可编译、可烧录的 Keil 工程 | A/B |
| 电机映射和底盘测试记录 | A |
| 遥控器测试记录 | A |
| 舵机映射和安全范围记录 | B |
| USART2 串口协议测试记录 | B |
| 上位机发速度控制底盘的验证结果 | A/B |
| 上位机发舵机指令的验证结果 | B |
| 已知问题列表 | A/B |
| 后续上位机任务分工 | A/B |

完成这些后，下位机阶段可以认为基本跑通，后续可以正式进入上位机代码编写阶段。
