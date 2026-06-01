# tao-stm32-firmware

OpenRF4 / STM32F407VET6 控制板下位机固件仓库。

## 仓库目标

本仓库用于保存原厂 STM32 下位机源码快照，并在此基础上逐步添加项目调试记录、串口协议说明、电机/舵机映射说明和后续定制代码。

## 控制板信息

- 控制板：OpenRF4 控制板
- MCU：STM32F407VET6
- 内核：Cortex-M4
- 主频：168 MHz
- Flash：512 KB
- SRAM：192 KB
- Keil Device 建议选择：STM32F407VE / STM32F407VETx

## Keil 工程

工程文件：

```text
firmware/SJRobot_center/project/SJRobot_center.uvprojx
```

## 关键目录

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

## 第一阶段原则

1. 第一次提交只保存原厂源码快照，不修改业务代码。
2. 文档和调试记录放在 `docs/`。
3. Keil 编译输出、用户本地配置和安装包不进 Git。
4. 后续每个功能独立提交，便于回退和对比。
