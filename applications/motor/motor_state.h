/* motor_state.h */
#ifndef __MOTOR_STATE_H__
#define __MOTOR_STATE_H__

#include <rtthread.h>
#include <rtdevice.h>

typedef enum
{
    ST_IDLE = 0,     // 空闲状态
    ST_FORWARD,      // 向前直行
    ST_BACKWARD,     // 向后直退
    ST_LEFT,         // 向左平移（左平行滑动）
    ST_RIGHT,        // 向右平移（右平行滑动）
    ST_ROTATE        // 原地顺时针旋转
} motor_state_t;     // 枚举体变量名称：motor_state_t

void motor_state_set(motor_state_t st); /* 当前状态设置 */
void motor_state_poll(void);            /* 当前状态对应的 **运动** 状态 */

#endif
