// /*
//  * 比赛控制逻辑实现
//  * 根据比赛规则设计的控制流程
//  */
//
// #include <rtthread.h>
// #include <rtdevice.h>
// #include <board.h>
// #include "drv_common.h"
// #include "motor/Inc/motor_state.h"
//
// /* 比赛状态定义 */
// typedef enum {
//     COMP_IDLE = 0,            /* 空闲状态 */
//     COMP_WAITING_START,       /* 等待启动信号 */
//     COMP_START_WAITING,       /* 10秒启动等待状态 */
//     COMP_RUNNING,             /* 比赛运行中 */
//     COMP_RESTART_WAITING,     /* 重启等待状态 */
//     COMP_FINISHED             /* 比赛结束 */
// } competition_state_t;
//
// /* 全局变量 */
// static competition_state_t comp_state = COMP_IDLE;
// static rt_tick_t comp_start_time = 0;      /* 比赛开始时间 */
// static rt_tick_t comp_restart_time = 0;    /* 重启开始时间 */
// static rt_tick_t comp_wait_start_time = 0; /* 启动等待时间 */
// static rt_bool_t restart_used = RT_FALSE;  /* 是否已使用重启权利 */
// static rt_bool_t in_restart_phase = RT_FALSE; /* 是否处于重启阶段 */
//
// /* 比赛时间常量 (单位：tick) */
// #define COMPETITION_TIME  (6 * 60 * RT_TICK_PER_SECOND)  /* 6分钟比赛时间 */
// #define RESTART_WAIT_TIME (10 * RT_TICK_PER_SECOND)      /* 10秒重启等待时间 */
// #define START_WAIT_TIME   (10 * RT_TICK_PER_SECOND)      /* 10秒启动等待时间 */
// #define RESTART_TIMEOUT   (2 * 60 * RT_TICK_PER_SECOND)  /* 2分钟重启超时 */
//
// /* 按钮定义 */
// #define START_KEY_PIN    GET_PIN(C, 13)  /* 使用PC13作为启动按钮 */
// #define RESTART_BTN_PIN   GET_PIN(D, 0)  /* 假设使用PD0作为重启按钮 */
//
// /* 函数声明 */
// static void competition_thread_entry(void *parameter);
// static void restart_robot(void);
//
// /**
//  * 初始化比赛控制
//  */
// int competition_control_init(void)
// {
//     /* 初始化按钮 */
//     rt_pin_mode(START_KEY_PIN, PIN_MODE_INPUT_PULLUP);
//     rt_pin_mode(RESTART_BTN_PIN, PIN_MODE_INPUT_PULLUP);
//
//     /* 创建比赛控制线程 */
//     rt_thread_t tid = rt_thread_create("comp_ctrl",
//                                       competition_thread_entry,
//                                       RT_NULL,
//                                       1024,
//                                       10,      /* 优先级 */
//                                       10);     /* 时间片 */
//     if (tid) rt_thread_startup(tid);
//     return 0;
// }
// INIT_APP_EXPORT(competition_control_init);
//
// /**
//  * 重启机器人
//  */
// static void restart_robot(void)
// {
//     if (restart_used || comp_state != COMP_RUNNING) {
//         return;  /* 已使用过重启权利或不在比赛中 */
//     }
//
//     /* 记录重启时间并设置状态 */
//     comp_restart_time = rt_tick_get();
//     comp_state = COMP_RESTART_WAITING;
//     in_restart_phase = RT_TRUE;
//     restart_used = RT_TRUE;
//
//     /* 停止所有电机 */
//     motion_stop();
//
//     rt_kprintf("[COMP] 机器人重启中，进入10秒等待阶段...\n");
// }
//
// /**
//  * 比赛控制线程
//  */
// static void competition_thread_entry(void *parameter)
// {
//     rt_tick_t current_time;
//     rt_tick_t elapsed_time;
//
//     while (1) {
//         current_time = rt_tick_get();
//
//         switch (comp_state) {
//         case COMP_IDLE:
//             /* 等待启动信号进入待命状态 */
//             if (rt_pin_read(START_KEY_PIN) == PIN_LOW) {
//                 /* 简单防抖 */
//                 rt_thread_mdelay(30);
//                 if (rt_pin_read(START_KEY_PIN) == PIN_LOW) {
//                     comp_state = COMP_WAITING_START;
//                     rt_kprintf("[COMP] 进入待命状态，等待裁判启动信号\n");
//                 }
//             }
//             break;
//
//         case COMP_WAITING_START:
//             /* 等待裁判启动信号 */
//             if (rt_pin_read(START_KEY_PIN) == PIN_LOW) {
//                 /* 简单防抖 */
//                 rt_thread_mdelay(30);
//                 if (rt_pin_read(START_KEY_PIN) == PIN_LOW) {
//                     comp_state = COMP_START_WAITING;
//                     comp_wait_start_time = rt_tick_get();
//                     rt_kprintf("[COMP] 进入10秒启动等待状态\n");
//
//                     /* 此时机器人不能有任何动作 */
//                     motion_stop();  /* 确保所有电机停止 */
//                 }
//             }
//             break;
//
//         case COMP_START_WAITING:
//             /* 10秒启动等待时间 */
//             elapsed_time = current_time - comp_wait_start_time;
//
//             if (elapsed_time >= START_WAIT_TIME) {
//                 /* 10秒等待结束，开始比赛 */
//                 comp_state = COMP_RUNNING;
//                 comp_start_time = rt_tick_get();
//                 rt_kprintf("[COMP] 启动等待结束，比赛开始！\n");
//
//                 /* 开始执行任务 */
//                 motor_state_set(ST_FORWARD);  /* 示例：开始向前移动 */
//             }
//             break;
//
//         case COMP_RUNNING:
//             /* 检查比赛是否结束 */
//             elapsed_time = current_time - comp_start_time;
//             if (elapsed_time >= COMPETITION_TIME) {
//                 comp_state = COMP_FINISHED;
//                 motion_stop();  /* 停止所有电机 */
//                 rt_kprintf("[COMP] 比赛结束！\n");
//                 break;
//             }
//
//             /* 检查重启按钮 */
//             if (!restart_used && rt_pin_read(RESTART_BTN_PIN) == PIN_LOW) {
//                 /* 简单防抖 */
//                 rt_thread_mdelay(30);
//                 if (rt_pin_read(RESTART_BTN_PIN) == PIN_LOW) {
//                     restart_robot();
//                 }
//             }
//             break;
//
//         case COMP_RESTART_WAITING:
//             /* 重启等待阶段 */
//             elapsed_time = current_time - comp_restart_time;
//
//             if (elapsed_time >= RESTART_WAIT_TIME) {
//                 /* 10秒等待结束，恢复比赛 */
//                 comp_state = COMP_RUNNING;
//                 in_restart_phase = RT_FALSE;
//                 rt_kprintf("[COMP] 重启完成，继续比赛\n");
//
//                 /* 重新开始任务 */
//                 motor_state_set(ST_FORWARD);  /* 示例：重新开始向前移动 */
//             }
//             break;
//
//         case COMP_FINISHED:
//             /* 比赛已结束，等待重置 */
//             if (rt_pin_read(START_KEY_PIN) == PIN_LOW) {
//                 /* 简单防抖 */
//                 rt_thread_mdelay(30);
//                 if (rt_pin_read(START_KEY_PIN) == PIN_LOW) {
//                     /* 重置比赛状态 */
//                     comp_state = COMP_IDLE;
//                     restart_used = RT_FALSE;
//                     in_restart_phase = RT_FALSE;
//                     rt_kprintf("[COMP] 系统重置，等待下一场比赛\n");
//                 }
//             }
//             break;
//         }
//
//         rt_thread_mdelay(10);  /* 10ms轮询 */
//     }
// }
//
// /**
//  * 获取当前比赛状态
//  */
// competition_state_t get_competition_state(void)
// {
//     return comp_state;
// }
//
// /**
//  * 检查是否可以使用重启权利
//  */
// rt_bool_t can_use_restart(void)
// {
//     return (comp_state == COMP_RUNNING && !restart_used);
// }
//
// /**
//  * 检查是否处于重启阶段
//  */
// rt_bool_t is_in_restart_phase(void)
// {
//     return in_restart_phase;
// }
