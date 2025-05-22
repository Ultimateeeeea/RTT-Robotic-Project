 #include "motor/Inc/motor_state.h"
 #include "motor/Inc/motor_cmd.h"



 extern volatile int16_t Curent_Speed[4];

 /***************************** 运动状态扫描线程 ****************************/
 static void state_thread(void *p)
 {
   /* 初始化底层串口uart4 */
   motor_cmd_init("uart4");

   static uint16_t Just_one;    //测试点
   while (1)
   {
       if(Just_one == 0)     //测试点
           motor_state_set(ST_LEFT);
       motor_state_poll();           /* 每循环一次执行一次动作 */
       ++Just_one;           //测试点


       rt_thread_mdelay(50);         /* 20Hz 刷新 */
   }
 }
 /******************************************************************/

 /***************************** 运动状态线程扫描开启函数 ****************************/
 int state_thread_init(void)
 {

   rt_thread_t tid = rt_thread_create("motor_state",
                                      state_thread,
                                      RT_NULL,
                                      1024,
                                      16,
                                      10);
   if (tid) rt_thread_startup(tid);
   return 0;
 }
 INIT_APP_EXPORT(state_thread_init);
 ///**********************************************************************************/
 ////
 ////
 /////***************************** 测速线程 ****************************/
 //
 //static void motor_rpm_thread_entry(void *parameter)
 //{
 //
 //  /* 局部变量保存单次读取结果 */
 //  int16_t LF_RPM = 0;
 //  int16_t RF_RPM = 0;
 //  int16_t LB_RPM = 0;
 //  int16_t RB_RPM = 0;
 //
 //  while (1)
 //  {
 //      /* 依次读取四路电机转速 */
 //      if (motor_cmd_read_rpm_nb(LF, &LF_RPM) != RT_EOK) LF_RPM = 0;
 //      if (motor_cmd_read_rpm_nb(RF, &RF_RPM) != RT_EOK) RF_RPM = 0;
 //      if (motor_cmd_read_rpm_nb(LB, &LB_RPM) != RT_EOK) LB_RPM = 0;
 //      if (motor_cmd_read_rpm_nb(RB, &RB_RPM) != RT_EOK) RB_RPM = 0;
 //
 //      /* 将局部变量批量写入全局数组 */
 //      int16_t rpm_local[4] = { LF_RPM, RF_RPM, LB_RPM, RB_RPM };
 //      for (int i = 0; i < 4; i++)
 //      {
 //          Curent_Speed[i] = rpm_local[i];
 //      }
 //
 //       /* （可选）打印四路速度，便于调试 */
 //        rt_kprintf("Speed LF:%d  RF:%d  LB:%d  RB:%d\r\n",
 //                   Curent_Speed[0],
 //                   Curent_Speed[1],
 //                   Curent_Speed[2],
 //                   Curent_Speed[3]);
 //
 //      /* 20 ms 延时，50 Hz 刷新 */
 //      rt_thread_mdelay(2);
 //  }
 //}
 //
 /////******************************************************************/
 ////
 ////
 /////************************************ 测速线程开启函数 ********************************/
 //static int init_motor_rpm_thread(void)
 //{
 //  rt_thread_t tid = rt_thread_create("rpm_thread",
 //                                     motor_rpm_thread_entry,
 //                                     RT_NULL,
 //                                     2048,   /* 栈大小 */
 //                                     18,    /* 优先级调整为12，使其高于 get_yaw 和 motor_state */
 //                                     20);   /* tick，影响 mdelay 精度 */
 //  if (tid)
 //      rt_thread_startup(tid);
 //  return 0;
 //}
 //INIT_COMPONENT_EXPORT(init_motor_rpm_thread);
 /////**********************************************************************************/
 ////
 ////
 ////
