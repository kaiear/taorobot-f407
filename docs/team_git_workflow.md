# 团队 Git 协作和拉取代码说明

本文档说明：如何把这个 STM32 固件仓库给组员使用、组员如何拉取代码、以后大家如何提交修改。

## 1. 当前仓库是什么

当前仓库是 OpenRF4 / STM32F407VET6 控制板的下位机固件仓库。

核心 Keil 工程文件：

```text
firmware/SJRobot_center/project/SJRobot_center.uvprojx
```

组员拿到代码后，用 Keil 打开这个 `.uvprojx` 文件即可编译和烧录。

## 2. 最推荐的团队共享方式：推到远程 Git 仓库

建议把本地仓库推到 GitHub、Gitee、GitLab 或学校/团队自己的 Git 服务器。

### 2.1 仓库命名建议

推荐远程仓库名：

```text
tao-stm32-firmware
```

如果你想放在总项目 `cpprobot` 下，也可以叫：

```text
cpprobot-f407-firmware
```

### 2.2 你作为仓库创建者要做的事

在 GitHub/Gitee/GitLab 上新建一个空仓库，不要勾选自动生成 README、.gitignore、LICENSE，因为本地已经有这些内容。

假设远程仓库地址是：

```text
https://github.com/你的用户名/tao-stm32-firmware.git
```

在本地仓库目录执行：

```powershell
git remote add origin https://github.com/你的用户名/tao-stm32-firmware.git
git branch -M main
git push -u origin main
```

如果使用 Gitee，地址类似：

```text
https://gitee.com/你的用户名/tao-stm32-firmware.git
```

对应命令：

```powershell
git remote add origin https://gitee.com/你的用户名/tao-stm32-firmware.git
git branch -M main
git push -u origin main
```

如果之前已经添加过 origin，要修改地址：

```powershell
git remote set-url origin https://github.com/你的用户名/tao-stm32-firmware.git
```

## 3. 组员如何拉取代码

组员电脑需要先安装：

- Git for Windows
- Keil MDK
- STM32F4 Device Pack
- JLink/STLink 驱动，按实际仿真器选择

然后在想保存代码的位置执行：

```powershell
git clone https://github.com/你的用户名/tao-stm32-firmware.git
```

进入仓库：

```powershell
cd tao-stm32-firmware
```

打开 Keil 工程：

```text
firmware/SJRobot_center/project/SJRobot_center.uvprojx
```

## 4. 组员每天开始改代码前应该做什么

进入仓库目录后，先拉最新代码：

```powershell
git pull
```

然后再打开 Keil 或 VS Code 修改代码。

## 5. 组员改完代码后如何提交

查看改了哪些文件：

```powershell
git status
```

添加改动：

```powershell
git add .
```

提交：

```powershell
git commit -m "描述这次改了什么"
```

推送到远程：

```powershell
git push
```

示例：

```powershell
git commit -m "add uart2 velocity debug log"
```

## 6. 分工建议

建议不要所有人直接同时改同一个文件。可以按模块分工：

| 人员 | 建议负责内容 | 主要文件 |
| --- | --- | --- |
| A | 底盘电机和运动学 | `Hardware_program/y_motor.c`, `Motorcontrol_program/kinematics.c`, `Motorcontrol_program/robot.c` |
| B | 树莓派/ROS 串口协议 | `Drive_program/uart2.c`, `Drive_include/uart2.h`, `docs/serial_protocol.md` |
| C | 舵机/机械臂 | `Hardware_program/y_servo.c`, `Motorcontrol_program/arm.c`, `docs/servo_mapping.md` |

## 7. 不要提交什么

`.gitignore` 已经排除了常见无用文件，包括：

- Keil 编译输出：`Objects/`, `Listings/`, `DebugConfig/`
- Keil 用户配置：`*.uvguix.*`, `*.uvoptx`
- 编译产物：`*.hex`, `*.axf`, `*.map`, `*.o`
- 安装包/压缩包：`*.exe`, `*.zip`, `*.rar`, `*.7z`
- VS Code 本地配置：`.vscode/`

注意：

```text
*.uvprojx 必须提交
```

因为它是 Keil 工程本体。

## 8. 如果组员不会 Git，可以先这样做

最低限度只需要三条命令：

第一次拿代码：

```powershell
git clone 远程仓库地址
```

每天开始前：

```powershell
git pull
```

改完提交：

```powershell
git add .
git commit -m "说明这次改动"
git push
```

## 9. 如果发生冲突怎么办

如果 `git pull` 或 `git push` 出现 conflict，先不要乱删文件。

处理原则：

1. 先截图或复制报错信息。
2. 看冲突文件是哪几个。
3. 和对应模块负责人确认保留哪部分。
4. 解决后执行：

```powershell
git add 冲突文件
git commit
git push
```

## 10. 本地路径建议

你希望把仓库放到：

```text
D:\cpprobot\f407\tao-stm32-firmware
```

组员也可以用类似结构：

```text
D:\cpprobot\f407\tao-stm32-firmware
```

这样大家文档里的路径更容易统一。
