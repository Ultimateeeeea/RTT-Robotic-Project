/* motor_motion.c */
#include "motor_motion.h"
#include "motor_cmd.h"

/* 假设四个轮地址固定：LF=0x01, RF=0x02, LB=0x03, RB=0x04 */
#define LF 0x01
#define RF 0x02
#define LB 0x03
#define RB 0x04
/* CW为顺时针 CCW为逆时针 */
#define CW 0x00
#define CCW 0x01

//前进指令
rt_err_t motion_forward(uint16_t rpm)
{
    motor_cmd_speed(LF, CW, rpm, 0, RT_FALSE);
    motor_cmd_speed(RF, CW, rpm, 0, RT_FALSE);
    motor_cmd_speed(LB, CW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RB, CW, rpm, 0, RT_TRUE);
    motor_cmd_sync(0x00);  // 广播 使得电机同步运转
    return RT_EOK;
}

//后退指令
rt_err_t motion_backward(uint16_t rpm)
{
    motor_cmd_speed(LF, CCW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RF, CCW, rpm, 0, RT_TRUE);
    motor_cmd_speed(LB, CCW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RB, CCW, rpm, 0, RT_TRUE);
    motor_cmd_sync(0x00);  // 广播 使得电机同步运转
    return RT_EOK;
}

//左平移指令
rt_err_t motion_strafe_left(uint16_t rpm)
{
    motor_cmd_speed(LF, CCW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RF, CW, rpm, 0, RT_TRUE);
    motor_cmd_speed(LB, CW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RB, CCW, rpm, 0, RT_TRUE);
    motor_cmd_sync(0x00);  // 广播 使得电机同步运转
    return RT_EOK;
}

//右平移指令
rt_err_t motion_strafe_right(uint16_t rpm)
{
    motor_cmd_speed(LF, CW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RF, CCW, rpm, 0, RT_TRUE);
    motor_cmd_speed(LB, CCW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RB, CW, rpm, 0, RT_TRUE);
    motor_cmd_sync(0x00);  // 广播 使得电机同步运转
    return RT_EOK;
}

//停止指令
rt_err_t motion_stop(void)
{
    motor_cmd_stop(LF, RT_TRUE);
    motor_cmd_stop(RF, RT_TRUE);
    motor_cmd_stop(LB, RT_TRUE);
    motor_cmd_stop(RB, RT_TRUE);
    motor_cmd_sync(0x00);  // 广播 使得电机同步运转
    return RT_EOK;
}

//顺时针原地旋转
rt_err_t motion_rotate_cw(uint16_t rpm)
{
    motor_cmd_speed(LF, CW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RF, CCW, rpm, 0, RT_TRUE);
    motor_cmd_speed(LB, CW, rpm, 0, RT_TRUE);
    motor_cmd_speed(RB, CCW, rpm, 0, RT_TRUE);
    motor_cmd_sync(0x00);  // 广播 使得电机同步运转
    return RT_EOK;
}
