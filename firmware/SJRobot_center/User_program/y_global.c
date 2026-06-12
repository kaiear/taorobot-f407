#include "y_global.h"

u8 cmd_return[CMD_RETURN_SIZE];

volatile uint8_t servo_test_enable = 0;
volatile uint8_t servo_test_id = 0;
volatile uint16_t servo_test_pos = 1500;
volatile uint16_t servo_test_time = 1000;
volatile uint8_t servo_test_send = 0;

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

/* ȡ����ֵ���� */
float abs_float(float value)
{
    if (value > 0)
    {
        return value;
    }
    return (-value);
}

/* ���ƶ���ı��� */
int X0 = 0, X1 = 0, X2 = 0, X3 = 0, X4 = 0, X5 = 0;

/* ���ƶ���ĺ��� */
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

void servo_bus_test_send_once(void)
{
    uint8_t safe_id;
    uint16_t safe_pos;
    uint16_t safe_time;

    if (!servo_test_enable || !servo_test_send)
    {
        return;
    }

    servo_test_send = 0;

    safe_id = servo_test_id;
    if (safe_id > 31)
    {
        safe_id = 31;
        servo_test_id = safe_id;
    }

    safe_pos = servo_test_pos;
    if (safe_pos < 600)
    {
        safe_pos = 600;
    }
    else if (safe_pos > 2400)
    {
        safe_pos = 2400;
    }
    servo_test_pos = safe_pos;

    safe_time = servo_test_time;
    if (safe_time < 500)
    {
        safe_time = 500;
    }
    else if (safe_time > 5000)
    {
        safe_time = 5000;
    }
    servo_test_time = safe_time;

    sprintf((char *)cmd_return, "{#%03dP%04dT%04d!}", safe_id, safe_pos, safe_time);
    zx_uart_send_str(cmd_return);
}

// ���� #000P1500T1000! ���Ƶ��ַ���
void parse_action(u8 *uart_receive_buf)
{
    u16 index, time, i = 0;
    int len;
    float pwm;
    zx_uart_send_str(uart_receive_buf);

    len = strlen((char *)uart_receive_buf); // ��ȡ���ڽ������ݵĳ���
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
