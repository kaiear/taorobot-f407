# SWD 烧录说明

## 推荐方式

优先使用 JLink / STLink / CMSIS-DAP 通过 SWD 下载和调试。

## 接线

| 仿真器 | 控制板 |
| --- | --- |
| VCC | VCC |
| GND | GND |
| DIO / SWDIO | SWDIO |
| CLK / SWCLK | SWCLK |

即文档中的：

```text
VCC、GND、DIO、CLK 四根线一一对应
```

## Keil Debug Adapter

打开：

```text
Options for Target -> Debug
```

按实际仿真器选择：

```text
J-LINK / J-TRACE Cortex
```

或：

```text
ST-Link Debugger
```

## 下载

点击：

```text
Flash -> Download
```

## 注意

源码中 `main.c` 存在约 20 秒 IMU 校准延时：

```c
Delay_ms(20000);
```

因此刚烧录后板子可能不会马上进入主任务循环。
