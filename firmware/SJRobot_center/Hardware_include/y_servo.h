

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Y_SERVO_H
#define __Y_SERVO_H

#include "main.h"
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* QSA */
#define SERVO0_PIN GPIO_Pin_4
#define SERVO0_GPIO_PORT GPIOE               /* GPIO端口 */
#define SERVO0_GPIO_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define SERVO1_PIN GPIO_Pin_3
#define SERVO1_GPIO_PORT GPIOE               /* GPIO端口 */
#define SERVO1_GPIO_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define SERVO2_PIN GPIO_Pin_2
#define SERVO2_GPIO_PORT GPIOE               /* GPIO端口 */
#define SERVO2_GPIO_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define SERVO3_PIN GPIO_Pin_1
#define SERVO3_GPIO_PORT GPIOE               /* GPIO端口 */
#define SERVO3_GPIO_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define SERVO4_PIN GPIO_Pin_0
#define SERVO4_GPIO_PORT GPIOE               /* GPIO端口 */
#define SERVO4_GPIO_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define SERVO5_PIN GPIO_Pin_7
#define SERVO5_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO5_GPIO_CLK RCC_AHB1Periph_GPIOB /* GPIO端口时钟 */

/* 控制舵机引脚输出的宏 */
#define SERVO0_PIN_SET(level) GPIO_WriteBit(SERVO0_GPIO_PORT, SERVO0_PIN, level)
#define SERVO1_PIN_SET(level) GPIO_WriteBit(SERVO1_GPIO_PORT, SERVO1_PIN, level)
#define SERVO2_PIN_SET(level) GPIO_WriteBit(SERVO2_GPIO_PORT, SERVO2_PIN, level)
#define SERVO3_PIN_SET(level) GPIO_WriteBit(SERVO3_GPIO_PORT, SERVO3_PIN, level)
#define SERVO4_PIN_SET(level) GPIO_WriteBit(SERVO4_GPIO_PORT, SERVO4_PIN, level)
#define SERVO5_PIN_SET(level) GPIO_WriteBit(SERVO5_GPIO_PORT, SERVO5_PIN, level)

#define DJ_NUM 8 /* 舵机数量，为8是因为定时器中断计算pwm周期需要 */

typedef struct
{
    // uint8_t valid; // 有效 TODO
    uint16_t aim;  // 执行目标
    uint16_t time; // 执行时间
    float cur;     // 当前值
    float inc;     // 增量
} servo_t;

extern servo_t duoji_doing[DJ_NUM];

// X-SOFT 接口函数
void SERVO_Init(void); // 舵机接口初始化
void servo_pin_set(u8 index, BitAction level);
void duoji_doing_set(u8 index, int aim, int time);

void TIM14_Int_Init(u16 arr, u16 psc);

#endif

/******************* (C) 版权 2022 XTARK **************************************/
