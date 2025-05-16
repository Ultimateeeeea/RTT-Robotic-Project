//#include "task_excution.h"
//
//void task_exec_entry(void *parameter)
//{
//    /* 1. 配置启动按键为上拉输入（防止忘记在别处初始化） */
//    rt_pin_mode(START_KEY_PIN, PIN_MODE_INPUT_PULLUP);
//
//    /* 2. 上电后先等待“启动键”被按下（低电平） */
//    while (rt_pin_read(START_KEY_PIN) == PIN_HIGH)   /* 保持高电平 = 未按 */
//    {
//        rt_thread_mdelay(10);                        /* 10 ms 轮询 + 喂狗 */
//    }
//
//    /* 3. 简单防抖：再确认 30 ms 内一直是低 */
//    rt_thread_mdelay(30);
//    if (rt_pin_read(START_KEY_PIN) == PIN_HIGH)
//        goto wait_start;                             /* 抖了一下，回去重等 */
//
//    /* 4. 检测到“按下” → 进入 10 s 待机 */
//    rt_kprintf("[TASK] Start key pressed, standby 10 s …\n");
//    rt_thread_mdelay(10 * 1000);
//    rt_kprintf("[TASK] Start Process OK! …\n"); /* 实测10s略有0.002s误差 */
//
//    /************** 潜在问题：在10s等待前触发小车四周的开关依然会改变开关标志位！********************/
//
//    /* ========= 正式进入任务循环 ========= */
//    while (1)
//    {
//        switch (g_task_state)
//        {
//        case TASK_SHELF2_GRAB:
//            /* TODO: 在此根据 both_button_press 细分动作并执行 */
//            break;
//
//        default:
//            /* 空闲 / 待命 … */
//            break;
//        }
//
//        rt_thread_mdelay(10);        /* 必要的循环延时，避免占满 CPU */
//    }
//
//wait_start:
//    /* 如果防抖失败，跳回来继续等 */
//    while (rt_pin_read(START_KEY_PIN) == PIN_HIGH)
//        rt_thread_mdelay(10);
//    goto wait_start;                 /* 理论上不会再抖，但保险起见 */
//}
//
//static int task_exec_init(void)
//{
//    rt_thread_t tid = rt_thread_create("task_execution",
//                                       task_exec_entry,
//                                       RT_NULL,
//                                       1024,
//                                       12,      /* 优先级 */
//                                       10);     /* 时间片 */
//    if (tid) rt_thread_startup(tid);
//    return 0;
//}
//INIT_APP_EXPORT(task_exec_init);
