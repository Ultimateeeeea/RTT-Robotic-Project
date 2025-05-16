/* motor_state.h */
#ifndef __MOTOR_STATE_H__
#define __MOTOR_STATE_H__

#include "motor/Inc/motor_cmd.h"

/* 状态 */
typedef enum
{
    ST_IDLE = 0,     // 空闲状态
    ST_FORWARD,      // 向前直行
    ST_BACKWARD,     // 向后直退
    ST_LEFT,         // 向左平移（左平行滑动）
    ST_RIGHT,        // 向右平移（右平行滑动）
    ST_ROTATE        // 原地顺时针旋转
} motor_state_t;     // 枚举体变量名称：motor_state_t

/* 动作 */
rt_err_t motion_forward(uint16_t rpm);      /* 前进 */
rt_err_t motion_backward(uint16_t rpm);
rt_err_t motion_strafe_left(uint16_t rpm);  /* 左平移 */
rt_err_t motion_strafe_right(uint16_t rpm);
rt_err_t motion_rotate_cw(uint16_t rpm);    /* 顺时针原地旋转 */
rt_err_t motion_stop(void);

void motor_state_set(motor_state_t st); /* 当前状态设置 */
void motor_state_poll(void);            /* 当前状态对应的 **运动** 状态 */

#endif
