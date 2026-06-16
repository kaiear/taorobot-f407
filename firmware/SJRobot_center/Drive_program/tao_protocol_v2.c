#include "main.h"

typedef enum
{
    TAO_V2_WAIT_AA = 0,
    TAO_V2_READ_LEN,
    TAO_V2_READ_TYPE,
    TAO_V2_READ_PAYLOAD,
    TAO_V2_READ_CRC,
    TAO_V2_READ_BB
} TaoV2RxState;

static TaoV2RxState tao_v2_state = TAO_V2_WAIT_AA;
static uint8_t tao_v2_len = 0;
static uint8_t tao_v2_type = 0;
static uint8_t tao_v2_payload[TAO_V2_MAX_PAYLOAD_LEN];
static uint8_t tao_v2_index = 0;
static uint8_t tao_v2_crc = 0;
static uint8_t tao_v2_rx_crc = 0;
static uint8_t tao_v2_mode = TAO_V2_MODE_SAFE_IDLE;
static uint8_t tao_v2_base_state = 0;
static uint8_t tao_v2_arm_state = 0;
static uint8_t tao_v2_buzzer_state = 0;
static uint8_t tao_v2_last_arm_seq = 0;
static uint16_t tao_v2_error_code = TAO_V2_ERR_OK;
static uint32_t tao_v2_last_heartbeat_ms = 0;

#define TAO_V2_HEARTBEAT_TIMEOUT_MS 500

static uint8_t TaoV2_CrcUpdate(uint8_t crc, uint8_t data)
{
    uint8_t i;

    crc ^= data;
    for(i = 0; i < 8; i++)
    {
        if(crc & 0x01)
        {
            crc = (crc >> 1) ^ 0x8C;
        }
        else
        {
            crc >>= 1;
        }
    }
    return crc;
}

static void TaoV2_ResetRx(void)
{
    tao_v2_state = TAO_V2_WAIT_AA;
    tao_v2_len = 0;
    tao_v2_type = 0;
    tao_v2_index = 0;
    tao_v2_crc = 0;
    tao_v2_rx_crc = 0;
}

static int16_t TaoV2_ReadI16(const uint8_t *data)
{
    return (int16_t)((uint16_t)data[0] | ((uint16_t)data[1] << 8));
}

static uint16_t TaoV2_ReadU16(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static uint32_t TaoV2_ReadU32(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) | ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

static uint16_t TaoV2_ArmAngleToPwm(uint8_t index, int16_t angle, uint8_t *clamped)
{
    int32_t pwm;
    uint16_t limited_pwm;
    int32_t home_pwm;

    home_pwm = servo_home_pwm(index);

    if(index == 2)
    {
        pwm = home_pwm + ((int32_t)angle * 1000) / 2356;
    }
    else
    {
        pwm = home_pwm - ((int32_t)angle * 1000) / 2356;
    }

    if(pwm < 0)
    {
        pwm = 0;
    }
    else if(pwm > 3000)
    {
        pwm = 3000;
    }

    limited_pwm = servo_pwm_limit(index, (uint16_t)pwm);
    if(clamped != 0 && limited_pwm != (uint16_t)pwm)
    {
        *clamped = 1;
    }
    return limited_pwm;
}

static void TaoV2_WriteI16(uint8_t *data, int16_t value)
{
    data[0] = (uint8_t)(value & 0xFF);
    data[1] = (uint8_t)((value >> 8) & 0xFF);
}

static void TaoV2_WriteU16(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)(value & 0xFF);
    data[1] = (uint8_t)((value >> 8) & 0xFF);
}

static void TaoV2_WriteU32(uint8_t *data, uint32_t value)
{
    data[0] = (uint8_t)(value & 0xFF);
    data[1] = (uint8_t)((value >> 8) & 0xFF);
    data[2] = (uint8_t)((value >> 16) & 0xFF);
    data[3] = (uint8_t)((value >> 24) & 0xFF);
}

static int16_t TaoV2_ClampI16(int16_t value, int16_t min_value, int16_t max_value)
{
    if(value < min_value)
    {
        return min_value;
    }
    if(value > max_value)
    {
        return max_value;
    }
    return value;
}

static void TaoV2_SendFrame(uint8_t type, const uint8_t *payload, uint8_t len)
{
    uint8_t crc;
    uint8_t i;

    if(len > TAO_V2_MAX_PAYLOAD_LEN)
    {
        return;
    }

    crc = 0;
    crc = TaoV2_CrcUpdate(crc, len);
    crc = TaoV2_CrcUpdate(crc, type);

    USART_SendData(USART2, TAO_V2_FRAME_HEADER);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
    USART_SendData(USART2, len);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
    USART_SendData(USART2, type);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);

    for(i = 0; i < len; i++)
    {
        crc = TaoV2_CrcUpdate(crc, payload[i]);
        USART_SendData(USART2, payload[i]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
    }

    USART_SendData(USART2, crc);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
    USART_SendData(USART2, TAO_V2_FRAME_TAIL);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}

static void TaoV2_SendAck(uint8_t ack_type, uint8_t result)
{
    uint8_t payload[2];

    payload[0] = ack_type;
    payload[1] = result;
    TaoV2_SendFrame(TAO_V2_TYPE_ACK, payload, 2);
}

static void TaoV2_SendError(uint16_t error_code, uint8_t detail)
{
    uint8_t payload[3];

    tao_v2_error_code = error_code;
    TaoV2_WriteU16(payload, error_code);
    payload[2] = detail;
    TaoV2_SendFrame(TAO_V2_TYPE_ERROR, payload, 3);
}

static uint8_t TaoV2_RequireLen(uint8_t type, uint8_t expected_len)
{
    if(tao_v2_len != expected_len)
    {
        TaoV2_SendAck(type, TAO_V2_ACK_BAD_LENGTH);
        TaoV2_SendError(TAO_V2_ERR_BAD_LENGTH, type);
        return 0;
    }
    return 1;
}

static uint8_t TaoV2_RequireRosAuto(uint8_t type)
{
    if(tao_v2_mode != TAO_V2_MODE_ROS_AUTO)
    {
        TaoV2_SendError(TAO_V2_ERR_BAD_MODE, type);
        return 0;
    }
    return 1;
}

static uint8_t TaoV2_IsValidMode(uint8_t mode)
{
    return (mode == TAO_V2_MODE_MANUAL ||
            mode == TAO_V2_MODE_ROS_AUTO ||
            mode == TAO_V2_MODE_ESTOP ||
            mode == TAO_V2_MODE_SAFE_IDLE);
}

static void TaoV2_StopRobot(void)
{
    Vel.TG_IX = 0;
    Vel.TG_IY = 0;
    Vel.TG_IW = 0;
    tao_v2_base_state = 0;
}

uint8_t TaoV2_GetMode(void)
{
    return tao_v2_mode;
}

uint8_t TaoV2_SetMode(uint8_t mode)
{
    if(!TaoV2_IsValidMode(mode))
    {
        return 0;
    }

    TaoV2_StopRobot();
    tao_v2_mode = mode;
    if(mode == TAO_V2_MODE_ROS_AUTO)
    {
        tao_v2_last_heartbeat_ms = millis();
        tao_v2_error_code = TAO_V2_ERR_OK;
    }
    return 1;
}

uint8_t TaoV2_IsRosAutoActive(void)
{
    uint32_t now = millis();

    return (tao_v2_mode == TAO_V2_MODE_ROS_AUTO &&
            (uint32_t)(now - tao_v2_last_heartbeat_ms) <= TAO_V2_HEARTBEAT_TIMEOUT_MS);
}

static void TaoV2_HandleBaseVel(void)
{
    int16_t vx;
    int16_t vy;
    int16_t wz;
    uint8_t clamped = 0;

    if(!TaoV2_RequireLen(TAO_V2_TYPE_BASE_VEL, 6) || !TaoV2_RequireRosAuto(TAO_V2_TYPE_BASE_VEL))
    {
        return;
    }

    vx = TaoV2_ReadI16(&tao_v2_payload[0]);
    vy = TaoV2_ReadI16(&tao_v2_payload[2]);
    wz = TaoV2_ReadI16(&tao_v2_payload[4]);

    if(vx != TaoV2_ClampI16(vx, -R_VX_LIMIT, R_VX_LIMIT)) clamped = 1;
    if(vy != TaoV2_ClampI16(vy, -R_VY_LIMIT, R_VY_LIMIT)) clamped = 1;
    if(wz != TaoV2_ClampI16(wz, -R_VW_LIMIT, R_VW_LIMIT)) clamped = 1;

    Vel.TG_IX = TaoV2_ClampI16(vx, -R_VX_LIMIT, R_VX_LIMIT);
    Vel.TG_IY = TaoV2_ClampI16(vy, -R_VY_LIMIT, R_VY_LIMIT);
    Vel.TG_IW = TaoV2_ClampI16(wz, -R_VW_LIMIT, R_VW_LIMIT);
    tao_v2_base_state = clamped ? TAO_V2_ACK_LIMIT_CLAMPED : TAO_V2_ACK_OK;
}

static void TaoV2_HandleArmJoints(void)
{
    uint8_t i;
    uint16_t duration_ms;
    uint8_t clamped = 0;

    if(!TaoV2_RequireLen(TAO_V2_TYPE_ARM_JOINTS, 15) || !TaoV2_RequireRosAuto(TAO_V2_TYPE_ARM_JOINTS))
    {
        return;
    }

    tao_v2_last_arm_seq = tao_v2_payload[0];
    for(i = 0; i < TAO_V2_JOINT_COUNT; i++)
    {
        arm_angle[i] = TaoV2_ReadI16(&tao_v2_payload[1 + i * 2]);
        ros_servo.pwm[i] = TaoV2_ArmAngleToPwm(i, arm_angle[i], &clamped);
        ros_servo.time[i] = 50;
    }

    duration_ms = TaoV2_ReadU16(&tao_v2_payload[13]);
    if(duration_ms > 0)
    {
        for(i = 0; i < TAO_V2_JOINT_COUNT; i++)
        {
            ros_servo.time[i] = duration_ms;
        }
    }

    ros_servo_data = 1;
    tao_v2_arm_state = clamped ? TAO_V2_ACK_LIMIT_CLAMPED : TAO_V2_ACK_OK;
}

static void TaoV2_HandleGripper(void)
{
    uint8_t percent;
    uint8_t clamped = 0;
    uint8_t result;

    if(!TaoV2_RequireLen(TAO_V2_TYPE_GRIPPER, 1) || !TaoV2_RequireRosAuto(TAO_V2_TYPE_GRIPPER))
    {
        return;
    }

    percent = tao_v2_payload[0];
    if(percent > 100)
    {
        percent = 100;
        clamped = 1;
    }

    arm_angle[5] = (int16_t)(-((int32_t)percent * 1000 / 100));
    ros_servo.pwm[5] = TaoV2_ArmAngleToPwm(5, arm_angle[5], &clamped);
    ros_servo.time[5] = 50;
    ros_servo_data = 1;

    result = clamped ? TAO_V2_ACK_LIMIT_CLAMPED : TAO_V2_ACK_OK;
    tao_v2_arm_state = result;
    TaoV2_SendAck(TAO_V2_TYPE_GRIPPER, result);
}

static void TaoV2_HandleArmPreset(void)
{
    uint8_t preset;
    uint8_t i;

    if(!TaoV2_RequireLen(TAO_V2_TYPE_ARM_PRESET, 1) || !TaoV2_RequireRosAuto(TAO_V2_TYPE_ARM_PRESET))
    {
        return;
    }

    preset = tao_v2_payload[0];
    if(preset == 0)
    {
        for(i = 0; i < TAO_V2_JOINT_COUNT; i++)
        {
            arm_angle[i] = 0;
            ros_servo.pwm[i] = servo_home_pwm(i);
            ros_servo.time[i] = 1500;
        }
        ros_servo_data = 1;
        tao_v2_arm_state = TAO_V2_ACK_OK;
        TaoV2_SendAck(TAO_V2_TYPE_ARM_PRESET, TAO_V2_ACK_OK);
    }
    else
    {
        TaoV2_SendAck(TAO_V2_TYPE_ARM_PRESET, TAO_V2_ACK_REJECTED);
    }
}

static void TaoV2_Dispatch(void)
{
    uint32_t time_ms;
    uint8_t pong_payload[4];

    switch(tao_v2_type)
    {
        case TAO_V2_TYPE_STOP:
            if(TaoV2_RequireLen(TAO_V2_TYPE_STOP, 0))
            {
                TaoV2_StopRobot();
                tao_v2_mode = TAO_V2_MODE_ESTOP;
                TaoV2_SendAck(TAO_V2_TYPE_STOP, TAO_V2_ACK_OK);
            }
            break;

        case TAO_V2_TYPE_SET_MODE:
            if(TaoV2_RequireLen(TAO_V2_TYPE_SET_MODE, 1))
            {
                if(TaoV2_SetMode(tao_v2_payload[0]))
                {
                    TaoV2_SendAck(TAO_V2_TYPE_SET_MODE, TAO_V2_ACK_OK);
                }
                else
                {
                    TaoV2_SendAck(TAO_V2_TYPE_SET_MODE, TAO_V2_ACK_REJECTED);
                    TaoV2_SendError(TAO_V2_ERR_BAD_MODE, tao_v2_payload[0]);
                }
            }
            break;

        case TAO_V2_TYPE_PING:
            if(TaoV2_RequireLen(TAO_V2_TYPE_PING, 4))
            {
                time_ms = TaoV2_ReadU32(tao_v2_payload);
                TaoV2_WriteU32(pong_payload, time_ms);
                TaoV2_SendFrame(TAO_V2_TYPE_PONG, pong_payload, 4);
            }
            break;

        case TAO_V2_TYPE_BASE_VEL:
            TaoV2_HandleBaseVel();
            break;

        case TAO_V2_TYPE_ARM_JOINTS:
            TaoV2_HandleArmJoints();
            break;

        case TAO_V2_TYPE_GRIPPER:
            TaoV2_HandleGripper();
            break;

        case TAO_V2_TYPE_ARM_PRESET:
            TaoV2_HandleArmPreset();
            break;

        case TAO_V2_TYPE_BUZZER:
            if(TaoV2_RequireLen(TAO_V2_TYPE_BUZZER, 2))
            {
                if(tao_v2_payload[0] == 2)
                {
                    ROBOT_BeepFaceSuccess();
                }
                else
                {
                    times = tao_v2_payload[1];
                    on_time = 100;
                    off_time = 100;
                }
                tao_v2_buzzer_state = tao_v2_payload[0];
                TaoV2_SendAck(TAO_V2_TYPE_BUZZER, TAO_V2_ACK_OK);
            }
            break;

        case TAO_V2_TYPE_HEARTBEAT:
            if(TaoV2_RequireLen(TAO_V2_TYPE_HEARTBEAT, 1))
            {
                tao_v2_last_heartbeat_ms = millis();
                tao_v2_error_code = TAO_V2_ERR_OK;
            }
            break;

        default:
            TaoV2_SendError(TAO_V2_ERR_UNKNOWN_TYPE, tao_v2_type);
            break;
    }
}

void TaoV2_Init(void)
{
    TaoV2_ResetRx();
    tao_v2_mode = TAO_V2_MODE_SAFE_IDLE;
    tao_v2_error_code = TAO_V2_ERR_OK;
    tao_v2_last_heartbeat_ms = millis();
}

void TaoV2_SafetyTick(void)
{
    uint32_t now = millis();

    if(tao_v2_mode == TAO_V2_MODE_ESTOP || tao_v2_mode == TAO_V2_MODE_SAFE_IDLE)
    {
        TaoV2_StopRobot();
        return;
    }

    if(tao_v2_mode == TAO_V2_MODE_ROS_AUTO &&
       (uint32_t)(now - tao_v2_last_heartbeat_ms) > TAO_V2_HEARTBEAT_TIMEOUT_MS)
    {
        TaoV2_StopRobot();
        tao_v2_mode = TAO_V2_MODE_SAFE_IDLE;
        tao_v2_error_code = TAO_V2_ERR_BASE_TIMEOUT;
        TaoV2_SendError(TAO_V2_ERR_BASE_TIMEOUT, TAO_V2_TYPE_HEARTBEAT);
    }
}

void TaoV2_OnByte(uint8_t data)
{
    switch(tao_v2_state)
    {
        case TAO_V2_WAIT_AA:
            if(data == TAO_V2_FRAME_HEADER)
            {
                tao_v2_state = TAO_V2_READ_LEN;
            }
            break;

        case TAO_V2_READ_LEN:
            if(data == 0x55)
            {
                TaoV2_ResetRx();
                break;
            }
            if(data > TAO_V2_MAX_PAYLOAD_LEN)
            {
                TaoV2_SendError(TAO_V2_ERR_BAD_LENGTH, data);
                TaoV2_ResetRx();
                if(data == TAO_V2_FRAME_HEADER)
                {
                    tao_v2_state = TAO_V2_READ_LEN;
                }
                break;
            }
            tao_v2_len = data;
            tao_v2_crc = TaoV2_CrcUpdate(0, data);
            tao_v2_state = TAO_V2_READ_TYPE;
            break;

        case TAO_V2_READ_TYPE:
            tao_v2_type = data;
            tao_v2_crc = TaoV2_CrcUpdate(tao_v2_crc, data);
            tao_v2_index = 0;
            tao_v2_state = (tao_v2_len == 0) ? TAO_V2_READ_CRC : TAO_V2_READ_PAYLOAD;
            break;

        case TAO_V2_READ_PAYLOAD:
            tao_v2_payload[tao_v2_index++] = data;
            tao_v2_crc = TaoV2_CrcUpdate(tao_v2_crc, data);
            if(tao_v2_index >= tao_v2_len)
            {
                tao_v2_state = TAO_V2_READ_CRC;
            }
            break;

        case TAO_V2_READ_CRC:
            tao_v2_rx_crc = data;
            tao_v2_state = TAO_V2_READ_BB;
            break;

        case TAO_V2_READ_BB:
            if(data != TAO_V2_FRAME_TAIL)
            {
                TaoV2_SendError(TAO_V2_ERR_BAD_TAIL, data);
            }
            else if(tao_v2_rx_crc != tao_v2_crc)
            {
                TaoV2_SendError(TAO_V2_ERR_BAD_CRC, tao_v2_type);
            }
            else
            {
                TaoV2_Dispatch();
            }
            TaoV2_ResetRx();
            if(data == TAO_V2_FRAME_HEADER)
            {
                tao_v2_state = TAO_V2_READ_LEN;
            }
            break;

        default:
            TaoV2_ResetRx();
            break;
    }
}

void TaoV2_SendStatus(void)
{
    uint8_t payload[22];
    uint8_t i;

    payload[0] = tao_v2_mode;
    payload[1] = tao_v2_base_state;
    payload[2] = tao_v2_arm_state;
    payload[3] = tao_v2_buzzer_state;
    TaoV2_WriteU16(&payload[4], tao_v2_error_code);
    TaoV2_WriteU16(&payload[6], Bat_Vol);
    payload[8] = TAO_V2_JOINT_COUNT;

    for(i = 0; i < TAO_V2_JOINT_COUNT; i++)
    {
        TaoV2_WriteI16(&payload[9 + i * 2], arm_angle[i]);
    }

    payload[21] = tao_v2_last_arm_seq;
    TaoV2_SendFrame(TAO_V2_TYPE_STATUS, payload, 22);
}
