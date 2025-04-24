/* motor_cmd.h */
#ifndef __MOTOR_CMD_H__
#define __MOTOR_CMD_H__

#include <rtthread.h>
#include <rtdevice.h>

rt_err_t motor_cmd_init(const char *uart_name);

/* 速度模式 */
rt_err_t motor_cmd_speed(uint8_t addr,
                         uint8_t dir,
                         uint16_t rpm,
                         uint8_t accel,
                         uint8_t sync_flag);

/* 位置模式 */
rt_err_t motor_cmd_position(uint8_t addr,
                                  uint8_t dir,
                                  uint16_t rpm,
                                  uint8_t accel,
                                  uint32_t pulses,
                                  uint8_t rel_abs,
                                  uint8_t sync_flag);

/* 立即停止 */
rt_err_t motor_cmd_stop(uint8_t addr, uint8_t sync_flag);

/* 同步启动 */
rt_err_t motor_cmd_sync(uint8_t addr);   /* 广播 0x00 一般 */

#endif
