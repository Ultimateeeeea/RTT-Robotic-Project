#include "motor/Inc/motor_state.h"
#include "motor/Inc/motor_cmd.h"


//static struct rt_event evt;       // 创建事件集对象 /*注意这个事件集只有在该文件内能用*/
//#define EVT_MOVE_FORWARD  (1<<0)    // 创建事件集内容
//#define EVT_MOVE_STOP     (1<<1)
extern volatile int16_t Curent_Speed[4];

/***************************** 运动状态扫描线程 ****************************/
static void state_thread(void *p)
{
   /* 初始化底层串口uart4 */
   motor_cmd_init("uart4");
   /* 初始化航向 PID + 速度差 PID */

   while (1)
   {
//       rt_uint32_t set;
//       /***************** 事件集接收 ****************/
//       if (rt_event_recv(&evt,
//                         EVT_MOVE_FORWARD|EVT_MOVE_STOP,
//                         RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
//                         10, &set) == RT_EOK)
//       {
//           if (set & EVT_MOVE_FORWARD) motor_state_set(ST_FORWARD);
//           if (set & EVT_MOVE_STOP)    motor_state_set(ST_IDLE);
//       }
//       /********************************************/
       motor_state_set(ST_FORWARD);
       motor_state_poll();           /* 每循环一次执行一次动作 */

//       if (motor_cmd_read_rpm_nb(0x05, &current_rpm) == RT_EOK)
//       {
//           // 使用 current_rpm 进行后续操作
//       }
//
//       else
//       {
//           rt_kprintf("Failed to read RPM\n");
//       }

       rt_thread_mdelay(50);         /* 20Hz 刷新 */
   }
}
/******************************************************************/

/***************************** 运动状态线程扫描开启函数 ****************************/
int state_thread_init(void)
{
//   rt_event_init(&evt, "motor_state_evt", RT_IPC_FLAG_FIFO);
   rt_thread_t tid = rt_thread_create("motor_state",
                                      state_thread,
                                      RT_NULL,
                                      1024,
                                      13,
                                      10);
   if (tid) rt_thread_startup(tid);
   return 0;
}
INIT_APP_EXPORT(state_thread_init);
/**********************************************************************************/


/***************************** 测速线程 ****************************/

static void motor_rpm_thread_entry(void *parameter)
{
    /* 局部变量保存单次读取结果 */
    int16_t LF_RPM = 0;
    int16_t RF_RPM = 0;
    int16_t LB_RPM = 0;
    int16_t RB_RPM = 0;

    while (1)
    {
        /* 依次读取四路电机转速 */
//        if (motor_cmd_read_rpm_nb(LF, &LF_RPM) != RT_EOK) LF_RPM = 0;
//        if (motor_cmd_read_rpm_nb(RF, &RF_RPM) != RT_EOK) RF_RPM = 0;
//        if (motor_cmd_read_rpm_nb(LB, &LB_RPM) != RT_EOK) LB_RPM = 0;
//        if (motor_cmd_read_rpm_nb(RB, &RB_RPM) != RT_EOK) RB_RPM = 0;

        /* 将局部变量批量写入全局数组 */
        int16_t rpm_local[4] = { LF_RPM, RF_RPM, LB_RPM, RB_RPM };
        for (int i = 0; i < 4; i++)
        {
            Curent_Speed[i] = rpm_local[i];
        }

        /* （可选）打印四路速度，便于调试 */
//        rt_kprintf("Speed LF:%d  RF:%d  LB:%d  RB:%d\r\n",
//                   Curent_Speed[0],
//                   Curent_Speed[1],
//                   Curent_Speed[2],
//                   Curent_Speed[3]);

        /* 20 ms 延时，50 Hz 刷新 */
        rt_thread_mdelay(20);
    }
}

/******************************************************************/


/************************************ 测速线程开启函数 ********************************/
static int init_motor_rpm_thread(void)
{
    rt_thread_t tid = rt_thread_create("rpm_thread",
                                       motor_rpm_thread_entry,
                                       RT_NULL,
                                       2048,   /* 栈大小 */
                                       12,    /* 优先级，可依据项目调整 */
                                       20);   /* tick，影响 mdelay 精度 */
    if (tid)
        rt_thread_startup(tid);
    return 0;
}
INIT_COMPONENT_EXPORT(init_motor_rpm_thread);
/**********************************************************************************/

//
///******************************************************/
///* 作为测试用 */
//static void test_event_thread(void *p)
//{
//   while (1)
//   {
////         rt_event_send(&evt, EVT_MOVE_FORWARD);
//         rt_kprintf("[TEST] Sent FORWARD\n");
//         rt_thread_mdelay(2000);  // 前进 2 秒
////       rt_event_send(&evt, EVT_MOVE_FORWARD);
////       rt_kprintf("[TEST] Sent FORWARD\n");
////       rt_thread_mdelay(2000);  // 前进 2 秒
////
////       rt_event_send(&evt, EVT_MOVE_STOP);
//       rt_kprintf("[TEST] Sent STOP\n");
//       rt_thread_mdelay(2000);  // 停止 2 秒
//   }
//}
//
//int test_thread_init(void)
//{
//   rt_thread_t tid = rt_thread_create("test_evt",
//                                      test_event_thread,
//                                      RT_NULL,
//                                      1024,
//                                      20,
//                                      10);
//   if (tid) rt_thread_startup(tid);
//   return 0;
//}
//INIT_APP_EXPORT(test_thread_init);
