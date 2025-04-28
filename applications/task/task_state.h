#ifndef __TASK_STATE_H__
#define __TASK_STATE_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_common.h"
#include "switch/switch_evt.h"      /* g_sw_event & EVT_SW_BOTH_PRESS */

typedef enum
{
    TASK_SHELF2_GRAB = 0,         /* 抓取第二层货架物品 */
    TASK_GO_HOME,                 /* 回家吧孩子 */
    /* TASK_...               预留：后续不同货架、回家、自检等任务 */
} task_state_t;

extern volatile task_state_t g_task_state;      //全局变量，记录目前执行的任务

void task_state_update(rt_uint32_t both_cnt);   //扫描小车状态函数

#endif
