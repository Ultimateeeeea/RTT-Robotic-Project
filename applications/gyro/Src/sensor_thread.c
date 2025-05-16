#include "gyro/Inc/sensor_port.h"

static void ShowHelp(void)//上电提示并且打印下面内容
{
    rt_kprintf("\r\n************************     WIT_SDK_DEMO   ************************");
    rt_kprintf("\r\n************************          HELP           ************************\r\n");
    rt_kprintf("UART SEND:K\\r\\n   Z-axis zero setting.\r\n");  //只对101Z轴置零起作用
    //101获取零偏过程中，不要动设备，然后等待20秒左右，再发送退出校准指令P这个
    rt_kprintf("UART SEND:A\\r\\n   AUTO setting.\r\n");  //只对101自动获取零偏起作用
    rt_kprintf("UART SEND:P\\r\\n   AUTO setting.\r\n");  //只对101结束自动获取零偏起作用

    rt_kprintf("UART SEND:h\\r\\n   help.\r\n");
    rt_kprintf("******************************************************************************\r\n");
}

static void CmdProcess(void)//UART1接收到的数据会代入到这里进行检测
{
    switch(s_cCmd)//判断输入的是什么进而来判断进行什么操作
    {

         case 'K':  if (WitStartIYAWCali() != WIT_HAL_OK)      //只对101z轴置零起作用
            rt_kprintf("\r\nSet IYAWCali Error\r\n");
              break;
         case 'A':
            if(WitStartRKMODECali() != WIT_HAL_OK) //自动获取零偏
                rt_kprintf("\r\nSet MagCali Error\r\n");
            break;
        case 'P':
            if(WitStopRKMODECali() != WIT_HAL_OK) //
                rt_kprintf("\r\nSet MagCali Error\r\n");
            break;
        case 'h':
            ShowHelp();//帮助
            break;
    }

    s_cCmd = 0xff;
}

void gyro_entry_thread(void *parameter)
{
    sensor_port_init();
    while (1)//循环语句
     {
         CmdProcess();//如果发送数据修改失败会发送错误提示会一直检测，成功的话不会有提示，如果需要成功有提示也是可以进行修改的

         if (s_cDataUpdate & ANGLE_UPDATE)
         {
             // 1) 计算出 Yaw，单位度
             float yaw = sReg[Yaw] / 32768.0f * 180.0f;
             g_yaw_angle = yaw; //得到全局Z轴角度
             // 2) 放大 100 倍取整
             int32_t v = (int32_t)(g_yaw_angle * 100);
             if (v < 0)  // 处理负数
             {
                 rt_kprintf("Yaw: -%d.%02d°\r\n",
                     -v / 100,
                     -v % 100);
             }
             else
             {
                 rt_kprintf("Yaw: %d.%02d°\r\n",
                     v / 100,
                     v % 100);
             }
             // 3) 清标志
             s_cDataUpdate &= ~ANGLE_UPDATE;
         }

//         gyro_cmd_zero();
         rt_thread_mdelay(10);
     }
}

//void gyrozero_entry_thread(void *parameter)
//{
//    while (1)//循环语句
//     {
//         gyro_cmd_zero();
//         rt_thread_mdelay(100);
//     }
//}

int gyro_thread_init(void)
{
    rt_thread_t tid = rt_thread_create("gyro_entry_thread",
                                        gyro_entry_thread,
                                       RT_NULL,
                                       2048,   /* 2 KB 栈，足够浮点输出 */
                                       12,     /* 优先级，可根据系统调整 */
                                       20);    /* 时间片 */
    if (tid)
        rt_thread_startup(tid);
    return tid ? RT_EOK : -RT_ERROR;
}

//int gyrozero_thread_init(void)
//{
//    rt_thread_t tid = rt_thread_create("gyrozero_entry_thread",
//                                        gyrozero_entry_thread,
//                                       RT_NULL,
//                                       2048,   /* 2 KB 栈，足够浮点输出 */
//                                       13,     /* 优先级，可根据系统调整 */
//                                       20);    /* 时间片 */
//    if (tid)
//        rt_thread_startup(tid);
//    return tid ? RT_EOK : -RT_ERROR;
//}

INIT_APP_EXPORT(gyro_thread_init);
//INIT_APP_EXPORT(gyrozero_thread_init);
