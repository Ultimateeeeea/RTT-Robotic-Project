#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#ifdef Error_Handler
#  undef Error_Handler
#endif
#include "tim.h"
#include "gpio.h"

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_common.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_COUNT   4

/* 用 GET_PIN(G, n) 宏直接生成引脚 ID，避免 PG0 冲突 */
static const rt_base_t trig_pins[SENSOR_COUNT] = {
    GET_PIN(G, 0),
    GET_PIN(G, 1),
    GET_PIN(G, 2),
    GET_PIN(G, 3),
};
static const rt_base_t echo_pins[SENSOR_COUNT] = {
    GET_PIN(G, 4),
    GET_PIN(G, 5),
    GET_PIN(G, 6),
    GET_PIN(G, 7),
};

/* 存放四路测得的距离，单位 cm */
extern volatile float UltrasonicDistance_cm[SENSOR_COUNT];

/**
 * @brief   启动四路超声波并行测距线程
 * @return  RT_EOK 成功；其他 失败
 */
int ultrasonic_thread_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __ULTRASONIC_H__ */
