// /*
//  * 比赛控制逻辑头文件
//  * 根据比赛规则设计的控制流程
//  */
//
// #ifndef __COMPETITION_CONTROL_H__
// #define __COMPETITION_CONTROL_H__
//
// #include <rtthread.h>
// #include <rtdevice.h>
// #include <board.h>
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
// /* 函数声明 */
// int competition_control_init(void);                /* 初始化比赛控制 */
// competition_state_t get_competition_state(void);   /* 获取当前比赛状态 */
// rt_bool_t can_use_restart(void);                   /* 检查是否可以使用重启权利 */
// rt_bool_t is_in_restart_phase(void);               /* 检查是否处于重启阶段 */
//
// #endif /* __COMPETITION_CONTROL_H__ */
