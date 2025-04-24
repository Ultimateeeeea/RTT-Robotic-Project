/* motor_motion.h */
#ifndef __MOTOR_MOTION_H__
#define __MOTOR_MOTION_H__

#include <rtthread.h>
#include <rtdevice.h>

rt_err_t motion_forward(uint16_t rpm);      /* 前进 */
rt_err_t motion_backward(uint16_t rpm);
rt_err_t motion_strafe_left(uint16_t rpm);  /* 左平移 */
rt_err_t motion_strafe_right(uint16_t rpm);
rt_err_t motion_rotate_cw(uint16_t rpm);    /* 顺时针原地旋转 */
rt_err_t motion_stop(void);

#endif
