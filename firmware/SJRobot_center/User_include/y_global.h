#ifndef _Y_GLOBAL_H_
#define _Y_GLOBAL_H_

#include "main.h"

#define MODULE "YH-KSTM32"

#define DJ_NUM 8

#define CMD_RETURN_SIZE 1024
extern u8 cmd_return[CMD_RETURN_SIZE];

uint16_t str_contain_str(unsigned char *str, unsigned char *str2);
float abs_float(float value);
void parse_action(u8 *uart_receive_buf);
void duoji_set(int duoji0, int duoji1, int duoji2, int duoji3, int duoji4, int duoji5);
#endif
