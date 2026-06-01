#include "y_global.h"

u8 cmd_return[CMD_RETURN_SIZE];

void zx_uart_send_str(u8 *str)
{
    uart3_send_str(str);
    uart1_send_str(str);
}

uint16_t str_contain_str(unsigned char *str, unsigned char *str2)
{
    unsigned char *str_temp, *str_temp2;
    str_temp = str;
    str_temp2 = str2;
    while (*str_temp)
    {
        if (*str_temp == *str_temp2)
        {
            while (*str_temp2)
            {
                if (*str_temp++ != *str_temp2++)
                {
                    str_temp = str_temp - (str_temp2 - str2) + 1;
                    str_temp2 = str2;
                    break;
                }
            }
            if (!*str_temp2)
            {
                return (str_temp - str);
            }
        }
        else
        {
            str_temp++;
        }
    }
    return 0;
}

/* 取绝对值函数 */
float abs_float(float value)
{
    if (value > 0)
    {
        return value;
    }
    return (-value);
}

/* 控制舵机的变量 */
int X0 = 0, X1 = 0, X2 = 0, X3 = 0, X4 = 0, X5 = 0;

/* 控制舵机的函数 */
void duoji_set(int duoji0, int duoji1, int duoji2, int duoji3, int duoji4, int duoji5)
{ 
    sprintf((char *)cmd_return, "{#000P%04dT%04d!#001P%04dT%04d!#002P%04dT%04d!#003P%04dT%04d!#004P%04dT%04d!#005P%04dT%04d!}", ros_servo.pwm[0], ros_servo.time[0],
																																 ros_servo.pwm[1], ros_servo.time[1],
																																 ros_servo.pwm[2],ros_servo.time[2],
																																 ros_servo.pwm[3],ros_servo.time[3],
																																 ros_servo.pwm[4],ros_servo.time[4],
																																 ros_servo.pwm[5],ros_servo.time[5]);

    parse_action(cmd_return);
}

// 处理 #000P1500T1000! 类似的字符串
void parse_action(u8 *uart_receive_buf)
{
    u16 index, time, i = 0;
    int len;
    float pwm;
    zx_uart_send_str(uart_receive_buf);

    len = strlen((char *)uart_receive_buf); // 获取串口接收数据的长度
    while (uart_receive_buf[i] && (len >= i))
    {
        if (uart_receive_buf[i] == '#')
        {
            index = 0;
            i++;
            while (uart_receive_buf[i] && uart_receive_buf[i] != 'P')
            {
                index = index * 10 + uart_receive_buf[i] - '0';
                i++;
            }
        }
        else if (uart_receive_buf[i] == 'P')
        {
            pwm = 0;
            i++;
            while (uart_receive_buf[i] && uart_receive_buf[i] != 'T')
            {
                pwm = pwm * 10 + uart_receive_buf[i] - '0';
                i++;
            }
        }
        else if (uart_receive_buf[i] == 'T')
        {
            time = 0;
            i++;
            while (uart_receive_buf[i] && uart_receive_buf[i] != '!')
            {
                time = time * 10 + uart_receive_buf[i] - '0';
                i++;
            }

            duoji_doing_set(index, pwm, time);
        }
        else
        {
            i++;
        }
    }
}
