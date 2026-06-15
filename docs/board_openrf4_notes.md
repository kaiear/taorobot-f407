# OpenRF4 控制板记录（不重要）

## 主控

- 控制板：OpenRF4
- MCU：STM32F407VET6
- 内核：Cortex-M4
- 主频：168 MHz
- Flash：512 KB
- SRAM：192 KB

Keil 设备型号建议选择：

```text
STM32F407VE / STM32F407VETx
```

## 板载资源

| 功能 | 资源 |
| --- | --- |
| 编码器电机 | 4 路：A/B/C/D |
| PWM 舵机 | 6 路 |
| 主控通信串口 | USART2，PD5/PD6，接树莓派 |
| 下载调试 | SWD：VCC/GND/DIO/CLK |
| USB 手柄 | USB Host，源码中已有 `USBH_Process()` |
| 电源输入 | 推荐 6~16V，电流 5A 以上 |
| 舵机电源 | 板载约 8V，最大 5A |
| 树莓派供电 | 板载 5V，最大 5A |

## 重要结论

USART2 是后续 ROS 上位机与 STM32 通信的优先通道：

```text
USART2 = PD5/PD6 = 用户自定义串口 = 接树莓派
```
