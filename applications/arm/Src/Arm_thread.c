//#include "arm/Inc/LobotServoController.h"
//
//#include "arm/Inc/bool.h"
//#include "drv_common.h"
//#include <rtthread.h>
//#include <rtdevice.h>
//#include <board.h>
//#include <rtdevice.h>
//
///* 这里的接收方式是非阻塞 */
//void arm_entry_thread(void *parameter)
//{
//    while (1)
//    {
//        moveServo(1, 1000, 1200); //1号舵机至1000位置
//        rt_thread_delay(1000);
//    }
//}
//
//int arm_entry_thread_init(void)
//{
//    rt_thread_t tid = rt_thread_create("arm_entry_thread",
//                                       arm_entry_thread,
//                                       RT_NULL,
//                                       1024,
//                                       16,
//                                       10);
//    if (tid) rt_thread_startup(tid);
//    return tid ? RT_EOK : -RT_ERROR;
//}
//
//INIT_APP_EXPORT(arm_entry_thread_init);
