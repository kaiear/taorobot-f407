# tao-stm32-firmware

OpenRF4 / STM32F407VET6 控制板下位机固件仓库。

## 1. 仓库目标

本仓库用于保存原厂 STM32 下位机源码快照，并在此基础上逐步添加项目调试记录、串口协议说明、电机/舵机映射说明和后续定制代码。

这个仓库可以理解为 CppRobot 项目里专门负责 F407 控制板的下位机代码库。

## 2. 当前本地推荐位置

本仓库建议放到：

```text
D:\cpprobot\f407\tao-stm32-firmware
```

## 3. 控制板信息

- 控制板：OpenRF4 控制板
- MCU：STM32F407VET6
- 内核：Cortex-M4
- 主频：168 MHz
- Flash：512 KB
- SRAM：192 KB
- Keil Device 建议选择：STM32F407VE / STM32F407VETx

## 4. Keil 工程

工程文件：

```text
firmware/SJRobot_center/project/SJRobot_center.uvprojx
```

用 Keil 打开这个文件即可编译、下载和调试 STM32 程序。

## 5. 目录作用说明

```text
tao-stm32-firmware/
├── README.md
├── .gitignore
├── docs/
├── firmware/
│   └── SJRobot_center/
└── tools/
```

### 5.1 根目录文件

| 文件 | 作用 |
| --- | --- |
| `README.md` | 仓库总说明，告诉大家这个库是什么、怎么打开、目录做什么 |
| `.gitignore` | Git 忽略规则，防止 Keil 编译输出、本机配置、安装包等垃圾文件进入仓库 |

### 5.2 docs/

`docs/` 放项目说明和调试记录，不放源码。

| 文件 | 作用 |
| --- | --- |
| `docs/board_openrf4_notes.md` | OpenRF4 控制板资源记录：芯片、串口、电机、舵机、供电、SWD 等 |
| `docs/keil_setup.md` | Keil 打开工程、芯片型号、Include Paths、Build 检查说明 |
| `docs/flashing_swd.md` | JLink/STLink/CMSIS-DAP 通过 SWD 下载程序的接线和 Keil 设置 |
| `docs/motor_mapping.md` | 电机 A/B/C/D、底层接口、速度范围、麦轮运动学记录 |
| `docs/servo_mapping.md` | 6 路舵机接口、PWM 范围、特殊反向通道记录 |
| `docs/serial_protocol.md` | USART2 和树莓派/ROS 通信协议记录 |
| `docs/team_git_workflow.md` | 组员如何拉取、提交、协作、避免冲突的说明 |

### 5.3 firmware/SJRobot_center/

这是原厂 STM32 下位机源码主体。

```text
firmware/SJRobot_center/
├── CORE/
├── Drive_include/
├── Drive_program/
├── Hardware_include/
├── Hardware_program/
├── Motorcontrol_include/
├── Motorcontrol_program/
├── User_include/
├── User_program/
├── USB/
├── FWLIB/
├── DSP_LIB/
└── project/
```

各目录作用：

| 目录 | 作用 |
| --- | --- |
| `CORE/` | Cortex-M4 / STM32 启动文件、中断文件、系统配置 |
| `FWLIB/` | STM32F4 标准外设库 |
| `DSP_LIB/` | ARM CMSIS DSP 数学库 |
| `Drive_include/` | 驱动层头文件，如串口、GPIO、PWM、CAN |
| `Drive_program/` | 驱动层 C 文件，如 `uart2.c`、`GPIO_int.c` |
| `Hardware_include/` | 硬件模块头文件，如电机、舵机、蜂鸣器、MPU6050 |
| `Hardware_program/` | 硬件模块实现文件，如 `y_motor.c`、`y_servo.c` |
| `Motorcontrol_include/` | 运动控制相关头文件 |
| `Motorcontrol_program/` | 运动学、机器人控制、机械臂控制等 |
| `User_include/` | 用户层头文件 |
| `User_program/` | 主程序、任务调度、手柄处理等 |
| `USB/` | USB Host / HID 手柄相关代码 |
| `project/` | Keil 工程文件目录，核心文件是 `SJRobot_center.uvprojx` |

### 5.4 tools/

`tools/` 用于放工具链说明或小脚本。

不要把 Keil 安装包、JLink 安装包、镜像、zip/rar/exe 等大文件放进 Git。

## 6. 关键源码入口

| 文件 | 作用 |
| --- | --- |
| `User_program/main.c` | 程序入口，初始化外设并启动任务循环 |
| `User_program/Task_manager.c` | 周期任务调度 |
| `User_program/app_ps2.c` | USB 手柄/PS2 风格输入处理 |
| `Motorcontrol_program/kinematics.c` | 底盘运动学，当前默认麦克纳姆轮 |
| `Motorcontrol_program/robot.c` | 机器人控制核心逻辑 |
| `Hardware_program/y_motor.c` | 四路电机底层控制 |
| `Hardware_program/y_servo.c` | 六路 PWM 舵机控制 |
| `Drive_program/uart2.c` | USART2，树莓派/ROS 与 STM32 通信入口 |

## 7. 组员如何拉取代码

最推荐方式是：把本仓库推送到 GitHub / Gitee / GitLab，然后组员用 `git clone` 拉取。

详细说明见：

```text
docs/team_git_workflow.md
```

简化版流程：

### 7.1 你先创建远程仓库并推送

假设远程仓库地址是：

```text
https://github.com/你的用户名/tao-stm32-firmware.git
```

执行：

```powershell
git remote add origin https://github.com/你的用户名/tao-stm32-firmware.git
git branch -M main
git push -u origin main
```

### 7.2 组员拉取

组员执行：

```powershell
git clone https://github.com/你的用户名/tao-stm32-firmware.git
```

然后用 Keil 打开：

```text
firmware/SJRobot_center/project/SJRobot_center.uvprojx
```

### 7.3 组员每天开始前

```powershell
git pull
```

### 7.4 组员改完后

```powershell
git add .
git commit -m "说明这次改动"
git push
```

## 8. 第一阶段原则

1. 第一次提交只保存原厂源码快照，不修改业务代码。
2. 文档和调试记录放在 `docs/`。
3. Keil 编译输出、用户本地配置和安装包不进 Git。
4. 后续每个功能独立提交，便于回退和对比。
5. 不要多人同时改同一个核心文件，避免冲突。
