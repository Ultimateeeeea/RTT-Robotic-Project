/* motor_state.c */
#include "motor_state.h"
#include "motor_motion.h"
#include "switch/switch_evt.h"      /* g_sw_event & EVT_SW_BOTH_PRESS */

static motor_state_t cur_state = ST_IDLE;

/* 当前状态设置 */
void motor_state_set(motor_state_t st)
{
    cur_state = st;
}

/* 当前状态对应的运动状态 */
void motor_state_poll(void)
{
    switch (cur_state)
    {
    case ST_IDLE:      motion_stop();     break;
    case ST_FORWARD:   motion_forward(10);   break;
    case ST_BACKWARD:  motion_backward(500);  break;
    case ST_LEFT:      motion_strafe_left(500); break;
    case ST_RIGHT:     motion_strafe_right(500); break;
    case ST_ROTATE:    motion_rotate_cw(400);   break;
    default:           break;
    }
}
