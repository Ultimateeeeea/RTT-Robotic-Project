/* motor_state.h */
#ifndef __MOTOR_STATE_H__
#define __MOTOR_STATE_H__

#include "motor/Inc/motor_cmd.h"

/* 假设四个轮地址固定：LF=0x01, RF=0x02, LB=0x03, RB=0x04 */
#define LF 0x01
#define RF 0x02
#define LB 0x03
#define RB 0x04

/* 57步进电机 */
#define Elevator 0x05

/* CW为顺时针 CCW为逆时针 */
#define CW 0x00
#define CCW 0x01

/* Relative为相对位置 Absolute为绝对位置*/
#define Relative 0x00
#define Absolute 0x01

/* 前后左右四路速度 */
extern volatile int16_t Curent_Speed[4];

/* 状态 */
typedef enum
{
   ST_IDLE = 0,     // 空闲状态
   ST_FORWARD,      // 向前直行
   ST_BACKWARD,     // 向后直退
   ST_LEFT,        // 向左平移（左平行滑动，供扫描物品时的移动）
   ST_RIGHT,        // 向右平移（右平行滑动）
   ST_PIVOT_RIGHT180,   // 新增：原地右转180°
   ST_PIVOT_RIGHT90,   // 新增：以右侧为轴原地右转 90°
   ST_UP,           // 57升一层
   ST_DOWN,         // 57降低一层
   ST_Elevator_Stop // 57停止
} motor_state_t;     // 枚举体变量名称：motor_state_t

/* 动作 */
rt_err_t motion_forward(uint16_t rpm);      /* 前进 */
rt_err_t motion_backward(uint16_t rpm);
rt_err_t motion_stop(void);                 /* 停止 */

rt_err_t motion_strafe_left(uint16_t rpm_base); /* 左平移 */
rt_err_t motion_strafe_right(uint16_t rpm);     /* 右平移 */


rt_err_t elevator_up(uint16_t rpm);         /* 57升 */
rt_err_t elevator_down(uint16_t rpm);
rt_err_t elevator_stop(void);

void motion_pivot_right_90(void);    /* 小车右转90度 */
void motion_pivot_right_180(void);
void motor_state_set(motor_state_t st); /* 当前状态设置 */
void motor_state_poll(void);            /* 当前状态对应的 **运动** 状态 */
motor_state_t motor_get_current_state(void); // motor_state_t 是您电机状态的枚举类型
#endif
