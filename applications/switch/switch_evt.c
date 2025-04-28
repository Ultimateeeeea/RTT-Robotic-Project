#include "switch_evt.h"

/************************ 函数功能 ***********************/
//  实现开关去抖，并统计被同时按下的次数：button_press(全局变量)
//  创建了全局事件集对象：g_sw_event
//  全局事件：EVT_SW_BOTH_PRESS
//  用于统计两开关被按下的**全局**标志位：both_button_press
//  开关被同时按下，发送事件集
/*********************************************************/

/* 全局事件集对象 */
struct rt_event g_sw_event;

/* 去抖相关 */
#define SCAN_PERIOD_MS   10          /* 10ms 轮询一次 */
#define DEBOUNCE_CNT     3           /* 连续 3 次稳定才算按下 */

/* 内部状态 */
static uint8_t sw1_stable = 0, sw2_stable = 0;  //开关稳定时的计数
static uint8_t sw1_cnt = 0, sw2_cnt = 0;
static rt_bool_t both_pressed_flag = RT_FALSE;  //两开关同时被按下的标志位
static volatile rt_int32_t both_cnt = 0;        //用于统计开关被按下的原子计数(防止被中断干扰)
volatile rt_uint32_t  both_button_press = 0;             //全局变量，记录两开关被同时按下的次数

/* 开关去抖，当两开关被同时按下时发送事件: EVT_SW_BOTH_PRESS*/
static void sw_scan_entry(void *parameter)
{
    while (1)
    {
        /* 读取当前电平是否为按下 */
        rt_uint8_t sw1_now = (rt_pin_read(SW1_PIN) == SW_ACTIVE);
        rt_uint8_t sw2_now = (rt_pin_read(SW2_PIN) == SW_ACTIVE);

        /* --- SW1 去抖 --- */
        if (sw1_now) {
            if (sw1_cnt < DEBOUNCE_CNT) sw1_cnt++;
        } else {
            sw1_cnt = 0;
        }
        sw1_stable = (sw1_cnt >= DEBOUNCE_CNT);

        /* --- SW2 去抖 --- */
        if (sw2_now) {
            if (sw2_cnt < DEBOUNCE_CNT) sw2_cnt++;
        } else {
            sw2_cnt = 0;
        }
        sw2_stable = (sw2_cnt >= DEBOUNCE_CNT);

        /* 检测“两键首次同时按下” */
        if (sw1_stable && sw2_stable)
        {
            if (!both_pressed_flag)
            {
                both_pressed_flag = RT_TRUE;          /* 置位一次性标志 */

                /* 打印测试 */
                rt_kprintf("Both swithes pressed!\n");

                rt_event_send(&g_sw_event, EVT_SW_BOTH_PRESS);
            }
        }
        else
        {
            both_pressed_flag = RT_FALSE;             /* 其中一键松开后复位标志 */
        }

        rt_thread_mdelay(SCAN_PERIOD_MS);
    }
}

/* -------------------------------------------------------------
 *                   线程安全的计数器（关中断临界区）//  防止中断产生的影响
 * -----------------------------------------------------------*/
/* 原子 +1，返回 ++ 后的值 */
static rt_int32_t both_counter_inc(void)
{
    rt_base_t level = rt_hw_interrupt_disable(); /* 进入临界区 */
    both_cnt++;
    rt_int32_t val = both_cnt;
    rt_hw_interrupt_enable(level);               /* 退出临界区 */
    return val;
}

/* 清零计数 */
//static void both_counter_reset(void)
//{
//    rt_base_t level = rt_hw_interrupt_disable();
//    both_cnt = 0;
//    rt_hw_interrupt_enable(level);
//}

/* -------------------------------------------------------------
 *                          逻辑线程：等待事件 → 计数
 * -----------------------------------------------------------*/
static void logic_thread_entry(void *parameter)
{
    rt_uint32_t recvd;

    while (1)
    {
        /* 阻塞等待 “两个微动开关首次同时按下” 事件 */
        if (rt_event_recv(&g_sw_event,
                          EVT_SW_BOTH_PRESS,
                          RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_FOREVER,
                          &recvd) == RT_EOK)
        {
            /* 线程安全地自增计数器 */
            both_button_press = both_counter_inc();

            /* 打印测试 */
            rt_kprintf("[SWITCH STATE] switch to %d\n",both_button_press);
//            /* 若计数 ≥3，循环归零 */

            /* 刷新全局任务状态，供 task_execution 模块使用 */
            task_state_update(both_button_press);
            rt_kprintf("[CAR STATE] car state to %d\n",g_task_state);
//            if (both_button_press >= 3)
//                both_counter_reset();
        }
    }
}

/* 初始化函数，注册到 INIT_APP_EXPORT */
int switch_evt_init(void)
{
    /* 1. 初始化 GPIO */
    rt_pin_mode(SW1_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(SW2_PIN, PIN_MODE_INPUT_PULLUP);

    /* 2. 初始化事件集 */
    rt_event_init(&g_sw_event, "sw_evt", RT_IPC_FLAG_FIFO);

    /* 3. 创建扫描线程 */
    rt_thread_t tid = rt_thread_create("sw_scan",
                                       sw_scan_entry,
                                       RT_NULL,
                                       512,
                                       10,      /* 优先级 */
                                       10);     /* 时间片 */
    if (tid) rt_thread_startup(tid);
    return 0;
}

static int logic_mgr_init(void)
{
    rt_thread_t tid = rt_thread_create("logic",
                                       logic_thread_entry,
                                       RT_NULL,
                                       1024,
                                       11,      /* 优先级 */
                                       10);     /* 时间片 */
    if (tid) rt_thread_startup(tid);
    return 0;
}

/* 系统启动时自动调用 */
INIT_APP_EXPORT(logic_mgr_init);
INIT_APP_EXPORT(switch_evt_init);
