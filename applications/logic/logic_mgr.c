//#include <rtthread.h>
//#include <rthw.h>
//
//#include "logic_mgr.h"
//#include "switch/switch_evt.h"      /* g_sw_event & EVT_SW_BOTH_PRESS */
//#include "motor/motor_state.h"     /* motor_state_set() */
//
//
///* -------------------------------------------------------------
// *               1. 线程安全的计数器（关中断临界区）//  防止中断产生的影响
// * -----------------------------------------------------------*/
//static volatile rt_int32_t both_cnt = 0;
//
///* 原子 +1，返回 ++ 后的值 */
//static rt_int32_t both_counter_inc(void)
//{
//    rt_base_t level = rt_hw_interrupt_disable(); /* 进入临界区 */
//    both_cnt++;
//    rt_int32_t val = both_cnt;
//    rt_hw_interrupt_enable(level);               /* 退出临界区 */
//    return val;
//}
//
///* 清零计数 */
//static void both_counter_reset(void)
//{
//    rt_base_t level = rt_hw_interrupt_disable();
//    both_cnt = 0;
//    rt_hw_interrupt_enable(level);
//}
//
/////* -------------------------------------------------------------
//// *               2. 动作决策映射
//// * -----------------------------------------------------------*/
////static action_t decide_action(rt_int32_t cnt)
////{
////    switch (cnt)
////    {
////    case 1:  return ACT_SEEK;      /* 第 1 次同时按下 → 寻物 */
////    case 2:  return ACT_GRAB;      /* 第 2 次 → 抓取 */
////    case 3:  return ACT_STRAFE_R;  /* 第 3 次 → 右移 */
////    default: return ACT_IDLE;      /* 其余 → 空闲 */
////    }
////}
////
////static void do_action(action_t act)
////{
////    switch (act)
////    {
////    case ACT_SEEK:      motor_state_set(ST_FORWARD);  break;
////    case ACT_GRAB:      motor_state_set(ST_ROTATE);   break;
////    case ACT_STRAFE_R:  motor_state_set(ST_RIGHT);    break;
////    default:            motor_state_set(ST_IDLE);     break;
////    }
////}
//
///* -------------------------------------------------------------
// *               3. 逻辑线程：等待事件 → 计数 → 动作
// * -----------------------------------------------------------*/
//static void logic_thread_entry(void *parameter)
//{
//    rt_uint32_t recvd;
//
//    while (1)
//    {
//        /* 阻塞等待 “两个微动开关首次同时按下” 事件 */
//        if (rt_event_recv(&g_sw_event,
//                          EVT_SW_BOTH_PRESS,
//                          RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
//                          RT_WAITING_FOREVER,
//                          &recvd) == RT_EOK)
//        {
//            /* 线程安全地自增计数器 */
//            button_press = both_counter_inc();
//
//            /* 根据计数决定动作，并下发给 motor_state */
//            action_t act = decide_action(button_press);
//            do_action(act);
//
////            /* 若计数 ≥3，循环归零 */
////            if (button_press >= 3)
////                both_counter_reset();
//        }
//    }
//}
//
///* -------------------------------------------------------------
// *               4. 初始化（自动执行）
// * -----------------------------------------------------------*/
//static int logic_mgr_init(void)
//{
//    rt_thread_t tid = rt_thread_create("logic",
//                                       logic_thread_entry,
//                                       RT_NULL,
//                                       1024,
//                                       16,      /* 优先级 */
//                                       10);     /* 时间片 */
//    if (tid) rt_thread_startup(tid);
//    return 0;
//}
//INIT_APP_EXPORT(logic_mgr_init);
