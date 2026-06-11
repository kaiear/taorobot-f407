#include "app_ps2.h"
#include "usbh_hid_gamepad.h"

#define PSX_BUTTON_NUM 16 // 手柄按键数目
#define ENABLE_PS2_CHASSIS_CONTROL 1
#define PS2_DEBUG_PRINT 1
#define PS2_DEADZONE 20
#define PS2_MOVE_SCALE 2
#define PS2_TURN_SCALE 6
#define PS2_MAX_MOVE_SPEED 300
#define PS2_MAX_TURN_SPEED 600
#define PS2_TIMEOUT_COUNT 100
#define PS2_CAPTURE_THRESHOLD 60

u8 uart_receive_buf[128];

/* PS2手柄接收数据
   ps2_buf[3]<<8 + ps2_buf[4] = (二进制，1释放，0按下)
   x  x  x  x  x  x  x  x   x  x  x  x  x  x  x  x
   LL LD LR LU ST AR AL SE  RL RD RR RU R1 L1 R2 L2

   L=左边 R=右边 D=下 U=上 A=遥感 1=前上 2=前下
   ST=START SE=SELECT
*/

/* 初始指令，如果没有从上位机下载指令的话会执行下面的指令 */
const char *pre_cmd_set_grn[PSX_BUTTON_NUM] = {
    // 绿灯模式下按键的配置
    "<G_L2:#005P0600T2000!^#005PDST!>", // L2  左上500
    "<G_R2:#005P2400T2000!^#005PDST!>", // R2	右上500
    "<G_L1:#004P0600T2000!^#004PDST!>", // L1	左上1000
    "<G_R1:#004P2400T2000!^#004PDST!>", // R1	右上1000
    "<G_RU:#002P2400T2000!^#002PDST!>", // RU	前进1000
    "<G_RR:#003P2400T2000!^#003PDST!>", // RR	右平移1000
    "<G_RD:#002P0600T2000!^#002PDST!>", // RD	后退1000
    "<G_RL:#003P0600T2000!^#003PDST!>", // RL	左平移1000
    "<G_SE:$DJR!>",                     // SE
    "<G_AL:>",                          // AL
    "<G_AR:>",                          // AR
    "<G_ST:#255P1500T2000!>",                     // ST
    "<G_LU:#001P0600T2000!^#001PDST!>", // LU	前进500
    "<G_LR:#000P0600T2000!^#000PDST!>", // LR	右转500
    "<G_LD:#001P2400T2000!^#001PDST!>", // LD	后退500
    "<G_LL:#000P2400T2000!^#000PDST!>", // LL	左转500
};

static u16 ps2_cmd = 0;
static u16 ps2_cmd_last = 0;
static u16 ps2_status_flag = 0xffff;

static short ps2_limit_speed(short value, short limit)
{
    if (value > limit)
        return limit;
    if (value < -limit)
        return -limit;
    return value;
}

static short ps2_axis_to_speed(u8 raw_value, short scale, short limit)
{
    short value;

    if (abs(128 - raw_value) <= PS2_DEADZONE)
        return 0;

    value = (128 - raw_value) * scale;
    return ps2_limit_speed(value, limit);
}

static const char *ps2_capture_label(void)
{
    short lx = 128 - ps2_buf[0];
    short ly = 128 - ps2_buf[1];
    short rx = 128 - ps2_buf[2];

    if (ly > PS2_CAPTURE_THRESHOLD)
        return "LEFT_STICK_FORWARD";
    if (ly < -PS2_CAPTURE_THRESHOLD)
        return "LEFT_STICK_BACKWARD";
    if (lx > PS2_CAPTURE_THRESHOLD)
        return "LEFT_STICK_LEFT";
    if (lx < -PS2_CAPTURE_THRESHOLD)
        return "LEFT_STICK_RIGHT";
    if (rx > PS2_CAPTURE_THRESHOLD)
        return "RIGHT_STICK_LEFT";
    if (rx < -PS2_CAPTURE_THRESHOLD)
        return "RIGHT_STICK_RIGHT";

    return "CENTER";
}

static void ps2_stop_chassis(void)
{
    Vel.TG_IX = 0;
    Vel.TG_IY = 0;
    Vel.TG_IW = 0;
}

void app_ps2(void)
{
    uint16_t pos;
    static u16 ps2_lost_count = 0;
    static u16 ps2_debug_count = 0;

    // 或者ps2没有读取数据，直接返回
    if (!ps2_do_ok)
    {
        if (ps2_lost_count < PS2_TIMEOUT_COUNT)
            ps2_lost_count++;
        else
            ps2_stop_chassis();
        return;
    }
    ps2_lost_count = 0;
    ps2_do_ok = 0;

    ps2_cmd_last = ps2_cmd;
    /* 判断手柄数据，更改为匹配以前代码的格式 */
    if (ps2_buf[6] & 0x01) /* L2 */
        ps2_cmd &= ~0X0001;
    else
        ps2_cmd |= 0X0001;

    if (ps2_buf[6] & 0x02) /* R2 */
        ps2_cmd &= ~0X0002;
    else
        ps2_cmd |= 0X0002;

    if ((ps2_buf[5] & 0x40)) /* L1 */
        ps2_cmd &= ~0X0004;
    else
        ps2_cmd |= 0X0004;

    if ((ps2_buf[5] & 0x80)) /* R1 */
        ps2_cmd &= ~0X0008;
    else
        ps2_cmd |= 0X0008;

    if ((ps2_buf[5] & 0x10)) /* RU */
        ps2_cmd &= ~0X0010;
    else
        ps2_cmd |= 0X0010;

    if ((ps2_buf[5] & 0x02)) /* RR */
        ps2_cmd &= ~0X0020;
    else
        ps2_cmd |= 0X0020;

    if ((ps2_buf[5] & 0x01)) /* RD */
        ps2_cmd &= ~0X0040;
    else
        ps2_cmd |= 0X0040;

    if ((ps2_buf[5] & 0x08)) /* RL */
        ps2_cmd &= ~0X0080;
    else
        ps2_cmd |= 0X0080;

    if ((ps2_buf[6] & 0x20)) /* 04 */
        ps2_cmd &= ~0X0100;
    else
        ps2_cmd |= 0X0100;

    if ((ps2_buf[6] & 0x20)) /* AL */
        ps2_cmd &= ~0X0200;
    else
        ps2_cmd |= 0X0200;

    if ((ps2_buf[6] & 0x40)) /* AR */
        ps2_cmd &= ~0X0400;
    else
        ps2_cmd |= 0X0400;

    if ((ps2_buf[6] & 0x08)) /* ST */
        ps2_cmd &= ~0X0800;
    else
        ps2_cmd |= 0X0800;

    if (ps2_buf[4] == 0x00) /* LU */
        ps2_cmd &= ~0X1000;
    else
        ps2_cmd |= 0X1000;

    if (ps2_buf[4] == 0x02) /* LR */
        ps2_cmd &= ~0X2000;
    else
        ps2_cmd |= 0X2000;

    if (ps2_buf[4] == 0x04) /* LD */
        ps2_cmd &= ~0X4000;
    else
        ps2_cmd |= 0X4000;

    if (ps2_buf[4] == 0x06) /* LL */
        ps2_cmd &= ~0X8000;
    else
        ps2_cmd |= 0X8000;

    if (ps2_cmd != ps2_cmd_last)
    {
        for (u8 i = 0; i < 16; i++)
        {
            if (!(ps2_cmd & (1 << i))) /* 当前手柄按下 */
            {
                if ((ps2_status_flag & (1 << i))) /* 上一次手柄未按下 */
                {
                    memset(uart_receive_buf, 0, sizeof(uart_receive_buf));
                    memcpy((char *)uart_receive_buf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));

                    pos = str_contain_str(uart_receive_buf, (u8 *)"^");
                    if (pos)
                        uart_receive_buf[pos - 1] = '\0';

                    strcpy((char *)cmd_return, (char *)uart_receive_buf + 6);
                    /* 判断指令格式 */
                    parse_action(cmd_return);
                }
            }
            else
            {
                if (!(ps2_status_flag & (1 << i)))
                {
                    memset(uart_receive_buf, 0, sizeof(uart_receive_buf));
                    /* 执行一次释放事件 */
                    memcpy((char *)uart_receive_buf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));

                    pos = str_contain_str(uart_receive_buf, (u8 *)"^");
                    if (pos)
                    {
                        strcpy((char *)cmd_return, (char *)uart_receive_buf + pos);
                        /* 判断指令格式 */
                        parse_action(cmd_return);
                    }
                }
            }
        }
        ps2_status_flag = ps2_cmd;
    }

#if ENABLE_PS2_CHASSIS_CONTROL
    Vel.TG_IX = ps2_axis_to_speed(ps2_buf[1], PS2_MOVE_SCALE, PS2_MAX_MOVE_SPEED);
    Vel.TG_IY = ps2_axis_to_speed(ps2_buf[0], PS2_MOVE_SCALE, PS2_MAX_MOVE_SPEED);
    Vel.TG_IW = ps2_axis_to_speed(ps2_buf[2], PS2_TURN_SCALE, PS2_MAX_TURN_SPEED);
#endif

#if PS2_DEBUG_PRINT
    ps2_debug_count++;
    if (ps2_debug_count >= 50)
    {
        ps2_debug_count = 0;
        printf("ps2 action=%s lx=%d ly=%d rx=%d ry=%d vel=%d,%d,%d\r\n",
               ps2_capture_label(),
               ps2_buf[0], ps2_buf[1], ps2_buf[2], ps2_buf[3],
               Vel.TG_IX, Vel.TG_IY, Vel.TG_IW);
    }
#endif
}
