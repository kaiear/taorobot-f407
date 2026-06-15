# Keil 工程配置说明（不重要）

## 工程文件

使用 Keil 打开：

```text
firmware/SJRobot_center/project/SJRobot_center.uvprojx
```

## 推荐安装

- Keil MDK 5.34 或相近版本
- STM32F4 Device Pack
- JLink 驱动或 STLink 驱动
- CH340 串口驱动
- Git for Windows
- VS Code

## 检查芯片型号

Keil 中打开：

```text
Options for Target -> Device
```

确认选择 STM32F407VET6 或同系列 STM32F407VE。

## Include Paths

如果 Build 时提示头文件找不到，检查：

```text
Options for Target -> C/C++ -> Include Paths
```

应包含类似路径：

```text
CORE
FWLIB/inc
Drive_include
Hardware_include
Motorcontrol_include
User_include
USB/...
DSP_LIB/Include
```

## 第一次验证流程

1. 打开工程。
2. 不改代码，先 Build。
3. 目标是 0 Error。
4. Warning 可以先记录，第一阶段不急于清理。
