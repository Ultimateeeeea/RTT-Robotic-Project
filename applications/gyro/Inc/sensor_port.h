#ifndef __SENSOR_PORT_H__
#define __SENSOR_PORT_H__

#ifdef Error_Handler
#  undef Error_Handler
#endif
#include "usart.h"
#include "main.h"
#include "tim.h"
#include "gpio.h"
/* CubeMX 生成的 usart.c 会导出 huart2 */

#include "gyro/Inc/wit_c_sdk.h"            /* 传感器官方 SDK */
#include "gyro/Inc/REG.h"
#include <stdint.h>
#include "drv_common.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rtdevice.h>

#define ACC_UPDATE      0x01
#define GYRO_UPDATE     0x02
#define ANGLE_UPDATE    0x04
#define MAG_UPDATE      0x08
#define READ_UPDATE     0x80

/* Wit_SDK初始化 */
int sensor_port_init(void);



/* 手动归零 */
rt_err_t gyro_cmd_zero(void);

/* 提供给内部数据读取用 */
extern volatile char s_cDataUpdate;
extern volatile char s_cCmd;

/* 全局变量，Yaw角度 */
extern volatile float g_yaw_angle;

/* 得到的全局设备 gyro_uart */
extern volatile rt_device_t gyro_uart;



#endif
