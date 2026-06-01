# 舵机映射记录

## 底层接口

源码位置：

```text
firmware/SJRobot_center/Hardware_program/y_servo.c
```

核心接口：

```c
duoji_doing_set(index, aim, time);
```

## PWM 范围

源码限制：

```c
if (aim > 2490) aim = 2490;
else if (aim < 510) aim = 510;
```

因此舵机 PWM 范围大致为：

```text
510 ~ 2490 us
```

## 特殊反向

源码中 `index == 3` 时做了反向：

```c
if(index == 3)
{
    aim = 3000 - aim;
}
```

机械臂调试时必须记录每一路舵机的实际方向、零位和安全范围。
