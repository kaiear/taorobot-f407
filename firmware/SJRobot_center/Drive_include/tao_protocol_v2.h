#ifndef __TAO_PROTOCOL_V2_H
#define __TAO_PROTOCOL_V2_H

#include "stm32f4xx.h"

#define TAO_V2_FRAME_HEADER       0xAA
#define TAO_V2_FRAME_TAIL         0xBB
#define TAO_V2_MAX_PAYLOAD_LEN    64
#define TAO_V2_JOINT_COUNT        6

#define TAO_V2_TYPE_STOP          0x00
#define TAO_V2_TYPE_SET_MODE      0x01
#define TAO_V2_TYPE_PING          0x02
#define TAO_V2_TYPE_BASE_VEL      0x10
#define TAO_V2_TYPE_ARM_JOINTS    0x20
#define TAO_V2_TYPE_GRIPPER       0x21
#define TAO_V2_TYPE_ARM_PRESET    0x22
#define TAO_V2_TYPE_BUZZER        0x30
#define TAO_V2_TYPE_HEARTBEAT     0x40

#define TAO_V2_TYPE_STATUS        0x80
#define TAO_V2_TYPE_ACK           0x81
#define TAO_V2_TYPE_ERROR         0x82
#define TAO_V2_TYPE_PONG          0x83

#define TAO_V2_MODE_MANUAL        0x00
#define TAO_V2_MODE_ROS_AUTO      0x01
#define TAO_V2_MODE_ESTOP         0x02
#define TAO_V2_MODE_SAFE_IDLE     0x03

#define TAO_V2_ACK_OK             0
#define TAO_V2_ACK_BUSY           1
#define TAO_V2_ACK_REJECTED       2
#define TAO_V2_ACK_BAD_MODE       3
#define TAO_V2_ACK_LIMIT_CLAMPED  4
#define TAO_V2_ACK_BAD_LENGTH     5

#define TAO_V2_ERR_OK             0x0000
#define TAO_V2_ERR_BAD_HEADER     0x0001
#define TAO_V2_ERR_BAD_LENGTH     0x0002
#define TAO_V2_ERR_BAD_CRC        0x0003
#define TAO_V2_ERR_BAD_TAIL       0x0004
#define TAO_V2_ERR_UNKNOWN_TYPE   0x0005
#define TAO_V2_ERR_BAD_MODE       0x0006
#define TAO_V2_ERR_BASE_TIMEOUT   0x0007
#define TAO_V2_ERR_ARM_LIMIT      0x0008
#define TAO_V2_ERR_ESTOP_ACTIVE   0x0009
#define TAO_V2_ERR_LOW_BATTERY    0x000A
#define TAO_V2_ERR_OVERFLOW       0x000B

void TaoV2_Init(void);
void TaoV2_OnByte(uint8_t data);
void TaoV2_SendStatus(void);

#endif
