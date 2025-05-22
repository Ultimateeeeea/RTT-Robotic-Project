//————— motor_cmd 配置 —————//
#ifndef MOTOR_CMD_CONFIG_H
#define MOTOR_CMD_CONFIG_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_common.h"
#include <string.h>

rt_err_t motor_cmd_init(const char *uart_name); //串口4初始化

/* 速度模式 */
rt_err_t motor_cmd_speed(uint8_t addr,
                        uint8_t dir,
                        uint16_t rpm,
                        uint8_t accel,
                        uint8_t sync_flag);

/* 相对位置模式 */
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

/* 读取速度 */
rt_err_t motor_cmd_read_rpm_nb(uint8_t addr, int16_t *rpm);

#endif
