/*-----------------------------------------------------------------------
 * File    : Arm_thread.c
 * Purpose : RT-Thread 线程示例——驱动 16 路单总线舵机控制板
 * MCU     : STM32F103ZET6
 * UART    : USART3  (CubeMX 已配置 9600 8N1)
 * RTOS    : RT-Thread 4.x
 * Dep     : LobotServoController.c/h  (放在 arm/Src & arm/Inc)
 *---------------------------------------------------------------------*/
#include <rtthread.h>
#include <rtdevice.h>
#include <stdint.h>
#include "arm/Inc/LobotServoController.h"   /* moveServo() 等 API :contentReference[oaicite:0]{index=0} */
#include "arm/Inc/bool.h"



/*----------- 串口设备名（与 board/drv_usart.c 对应） ------------------*/
#define SERVO_UART_NAME   "uart3"

/*----------- 全局串口句柄 --------------------------------------------*/
static rt_device_t servo_uart = RT_NULL;

/*======================  Lobot 硬件抽象层  ===========================*/
/* ① 发送缓存 —— 覆盖库里默认的阻塞发送函数 */
void uartWriteBuf(uint8_t *buf, uint8_t len)
{
    if (servo_uart)
        rt_device_write(servo_uart, 0, buf, len);
}

/* ② 延时 —— 把“忙等”改成 RTOS 友好的线程延时 */
void delay_ms(uint16_t nms)
{
    rt_thread_mdelay(nms);
}
void delay_s(int s)
{
    rt_thread_mdelay(s * 1000);
}

///*======================  舵机演示线程  ==============================*/
//static void servo_thread_entry(void *parameter)
//{
//    /* 打开 UART3 */
////    servo_uart = rt_device_find(SERVO_UART_NAME);
////    if (!servo_uart)
////    {
////        rt_kprintf("[servo] cannot find %s!\n", SERVO_UART_NAME);
////        return;
////    }
////    rt_device_open(servo_uart, RT_DEVICE_OFLAG_RDWR);   /* 默认已 9600 8N1 */
////    rt_kprintf("[servo] use %s ok\n", SERVO_UART_NAME);
////
////    delay_s(2);     /* 等控制板蓝灯自检完毕 */
//
//    /* 演示：1 号舵机在 1200 ms 内 500↔2500 脉宽来回摆动 */
//    while (1)
//    {
////        moveServo(3,  500, 1200);  /* ≈ 0° */
////        delay_s(2);
//
//        moveServo(ARM_ROTATE_SERVO_ID, ROTATE_FRONT_POS, 1000);
//        rt_thread_mdelay(1200);
//        moveServo(ARM_ROTATE_SERVO_ID, ROTATE_BACK_POS, 1000);
//        rt_thread_mdelay(1200);
//
//        moveServo(ARM_EXTEND_SERVO_ID, ARM_EXTENDED_POS, 1000);
//        rt_thread_mdelay(1000);
//        moveServo(ARM_EXTEND_SERVO_ID, ARM_RETRACTED_POS, 1000);
//        rt_thread_mdelay(1000);
//
//        moveServo(ARM_GRAB_SERVO_ID, GRAB_OPEN_POS, 1000);
//        rt_thread_mdelay(1000);
//        moveServo(ARM_GRAB_SERVO_ID, GRAB_CLOSE_POS, 1000);
//    }
//}

/////*======================  线程自动创建  ==============================*/
//#define SERVO_THREAD_STACK   1024
//#define SERVO_THREAD_PRIO    20
//#define SERVO_THREAD_TICK    10
//
//static int servo_thread_init(void)
//{
//    rt_thread_t tid = rt_thread_create("servo",
//                                       servo_thread_entry,
//                                       RT_NULL,
//                                       SERVO_THREAD_STACK,
//                                       SERVO_THREAD_PRIO,
//                                       SERVO_THREAD_TICK);
//    if (tid)
//    {
//        rt_thread_startup(tid);
//        rt_kprintf("[servo] thread started.\n");
//        return 0;
//    }
//    return -1;
//}
///* RT-Thread 组件初始化阶段自动调用 */
//INIT_APP_EXPORT(servo_thread_init);
