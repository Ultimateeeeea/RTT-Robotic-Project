//#include <rtthread.h>
//#include "motor_control.h"
//
///* 1. 定义事件位 */
//#define EVT_MOTOR_START  (1 << 0)
//
///* 2. 声明并初始化事件集对象 */
//static struct rt_event evt;
//
///* 3. 任务 A：执行完某段业务代码后，发送启动电机事件 */
//static void task_a_entry(void *parameter)
//{
//    while (1)
//    {
//        /* —— 这里替换成你自己的业务逻辑 —— */
//        rt_kprintf("[Task A] do some work...\n");
//        rt_thread_mdelay(500);  /* 假装这段代码执行了 500ms */
//
//        /* 业务完成后，发出 “启动电机” 事件 */
//        rt_kprintf("[Task A] send EVT_MOTOR_START\n");
//        rt_event_send(&evt, EVT_MOTOR_START);
//
//        /* 再等一会儿，方便观察 */
//        rt_thread_mdelay(2000);
//    }
//}
//
///* 4. 任务 B：等待启动电机事件，收到后调用 motor_thread_init() */
//static void task_b_entry(void *parameter)
//{
//    rt_uint32_t recved;
//    while (1)
//    {
//        /* 等待 EVT_MOTOR_START 到来，收到后自动清除该位 */
//        if(rt_event_recv(&evt,
//                      EVT_MOTOR_START,
//                      RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
//                      RT_WAITING_FOREVER,
//                      &recved) == RT_EOK )
//        {
//            rt_kprintf("[Task B] EVT_MOTOR_START received → start motor thread\n");
//            motor_thread_init();
//        }
//
//
//    }
//}
//
///* 5. 初始化函数：创建事件集 + 启动两个任务 */
//static int event_motor_init(void)
//{
//    /* 初始化事件集，FIFO 模式 */
//    rt_event_init(&evt, "evt", RT_IPC_FLAG_FIFO);
//
//    /* 创建并启动任务 A */
//    rt_thread_t tid_a = rt_thread_create("task_a",
//                                         task_a_entry,
//                                         RT_NULL,
//                                         512,
//                                         10,   /* 优先级 */
//                                         10);  /* 时间片 */
//    if (tid_a) rt_thread_startup(tid_a);
//
//    /* 创建并启动任务 B */
//    rt_thread_t tid_b = rt_thread_create("task_b",
//                                         task_b_entry,
//                                         RT_NULL,
//                                         512,
//                                         11,   /* 稍低优先级 */
//                                         10);
//    if (tid_b) rt_thread_startup(tid_b);
//
//    return 0;
//}
///* 注册到系统初始化，也可以在 main() 里手动调用 */
//INIT_APP_EXPORT(event_motor_init);
