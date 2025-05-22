///*
// * vision_pick_task.c -- 根据 YOLO 视觉结果自动进行抓/放测试
// *
// * 依赖：
// *   1) yolo_serial_com 模块 (UART5) 已经初始化，并持续更新全局变量
// *   2) moveServo() API 可正常驱动舵机
// *
// * 使用方法：系统启动后自动创建线程 "vision_pick"；
// * 当检测到 cls 0/1/2/3/8 且 dx 偏差在阈值内时，立即执行一套抓取动作。
// */
//
//#include <rtthread.h>
//#include "yolo_comm/Inc/yolo_serial_com.h"
//#include "arm/Inc/LobotServoController.h" /* moveServo 原型在此头文件中 */
//
///* ---------------- 线程配置 ---------------- */
//#define VPICK_STACK_SIZE   1024
//#define VPICK_THREAD_PRIO  26
//#define VPICK_THREAD_TICK  10
//
///* ---------------- 参数宏 ---------------- */
//#define DX_THRESHOLD_CM    5      /* |dx|≤50mm 认为对准 */
//#define ACTION_DELAY(ms)   rt_thread_mdelay(ms)
//
//static void vision_pick_entry(void *parameter)
//{
//    while (1)
//    {
//        if (g_new_yolo_data_flag)
//        {
//            g_new_yolo_data_flag = RT_FALSE;    /* 立刻清标志 */
//
//            if (g_latest_yolo_result.detected)
//            {
//                int dx_mm = (int)(g_latest_yolo_result.x_offset_cm * 10);
//
//                if (g_latest_yolo_result.x_offset_cm <= DX_THRESHOLD_CM)
//                {
//                    rt_kprintf("[VPICK] Target OK  cls:%d  dx:%dmm\n",
//                               g_latest_yolo_result.item_class, dx_mm);
//
//                    /* ------- 抓取动作（恢复到用户上次确认的顺序） ------- */
//
//                    /* 1. 打开爪子 */
//                    moveServo(ARM_GRAB_SERVO_ID, GRAB_OPEN_POS, 500);     // 时间改为 800ms
//                    ACTION_DELAY(1000);                                 // 延时改为 1000ms
//
//                    /* 2. 伸出机械臂 */
//                    moveServo(ARM_EXTEND_SERVO_ID, ARM_EXTENDED_POS, 1000);
//                    ACTION_DELAY(1000);                                 // 延时改为 1000ms
//
//                    /* 3. 闭合爪子抓取 */
//                    moveServo(ARM_GRAB_SERVO_ID, GRAB_CLOSE_POS, 500);   // 时间改为 1500ms
//                    ACTION_DELAY(1000);                                 // 延时改为 1000ms
//
//                    /* 4. 收回机械臂 */
//                    moveServo(ARM_EXTEND_SERVO_ID, ARM_RETRACTED_POS, 1500); // 时间改为 1500ms
//                    ACTION_DELAY(900);
//
//                    /* 5. 转台逆时针 180° 对准置物篮 */
//                    moveServo(ARM_ROTATE_SERVO_ID, ROTATE_BACK_POS, 1500); // 时间改为 1500ms
//                    ACTION_DELAY(2000);                                 // 延时改为 1000ms
//
//                    /* 6. 打开爪子放置物品 */
//                    moveServo(ARM_GRAB_SERVO_ID, GRAB_OPEN_POS, 300);
//                    ACTION_DELAY(500);
//
//                    /* 7. 再次闭合爪子 */
//                    moveServo(ARM_GRAB_SERVO_ID, GRAB_CLOSE_POS, 300);
//                    ACTION_DELAY(400);
//
//                    /* 8. 转台顺时针 180° 回到车前 */
//                    moveServo(ARM_ROTATE_SERVO_ID, ROTATE_FRONT_POS, 1500);
//                    ACTION_DELAY(600);
//                }
//                else
//                {
//                    rt_kprintf("[VPICK] dx %dmm out of range, skip.\n", dx_mm);
//                }
//            }
//        }
//        rt_thread_mdelay(20);
//    }
//}
//
///* ------------ 自动创建线程 ------------- */
//int vision_pick_task_init(void)
//{
//    rt_thread_t tid = rt_thread_create("vision_pick",
//                                       vision_pick_entry,
//                                       RT_NULL,
//                                       VPICK_STACK_SIZE,
//                                       VPICK_THREAD_PRIO,
//                                       VPICK_THREAD_TICK);
//    if (tid)
//        rt_thread_startup(tid);
//    else
//        rt_kprintf("[VPICK] create thread failed!\n");
//    return 0;
//}
//INIT_APP_EXPORT(vision_pick_task_init);
