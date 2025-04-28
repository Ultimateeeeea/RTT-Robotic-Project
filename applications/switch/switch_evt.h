#ifndef __SWITCH_EVT_H__
#define __SWITCH_EVT_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_common.h"
#include "task/task_state.h"

/* ------------ GPIO 宏，根据实际板子修改 ------------- */
#define SW1_PIN    GET_PIN(E,0)   /* 例：PE0 前轮 */
#define SW2_PIN    GET_PIN(E,1)   /* 例：PE1 后轮 */
#define SW_ACTIVE  PIN_LOW         /* 低电平表示按下 */

/* ------------ 事件位定义 ---------------------------- */
#define EVT_SW_BOTH_PRESS    (1 << 0)   /* 两个开关首次共同触发 */

extern struct rt_event g_sw_event; /* 全局事件集对象 */
int switch_evt_init(void);   /* 在 INIT_APP_EXPORT 中自动调用 */

extern volatile rt_uint32_t  both_button_press;   /* 全局变量：统计按钮首次按下次数 */

#endif /* __SWITCH_EVT_H__ */
