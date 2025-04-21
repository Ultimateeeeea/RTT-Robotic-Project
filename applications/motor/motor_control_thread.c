//#include <rtthread.h>
//#include <rtdevice.h>
//#include <board.h>
//#include "drv_common.h"
//
//#define MOTOR_UART_NAME  "uart4"
//static rt_device_t motor_uart = RT_NULL;
//
///*************************  步进电机的指令 *****************************/
///* 位置闭环命令 */
//
///**
// * Build and send a position command to the motor.
// * @param addr       Device address
// * @param rpm        Speed in RPM (0.1 resolution if S_Vel_IS enabled)
// * @param pulses     Number of microstep pulses
// * @param accel      Acceleration code (0 for disabled)
// * @param rel_abs    0x00: relative, 0x01: absolute
// * @param sync_flag  0x00: no sync, 0x01: enable sync
// */
//
//static void send_position_command(uint8_t addr,
//                                  uint16_t rpm,
//                                  uint32_t pulses,
//                                  uint8_t accel,
//                                  uint8_t rel_abs,
//                                  uint8_t sync_flag)
//{
//    uint8_t cmd[11];
//    cmd[0] = addr;                    // 地址
//    cmd[1] = 0xFD;                    // 功能码：位置模式
//    cmd[2] = (rpm >> 8) & 0xFF;       // 速度高字节
//    cmd[3] = rpm & 0xFF;              // 速度低字节
//    cmd[4] = accel;                   // 加速度档位
//    cmd[5] = (pulses >> 24) & 0xFF;   // 脉冲数字节3
//    cmd[6] = (pulses >> 16) & 0xFF;   // 脉冲数字节2
//    cmd[7] = (pulses >> 8) & 0xFF;    // 脉冲数字节1
//    cmd[8] = pulses & 0xFF;           // 脉冲数字节0
//    cmd[9] = rel_abs;                 // 相对/绝对
//    cmd[10] = sync_flag;              // 同步标志
//
//    // 默认校验 0x6B
//    // 如果需要其他校验方式，可在这里动态计算
//    cmd[10] = sync_flag;
//    uint8_t checksum = 0x6B;
//    // Final packet is 0--10 length = 11 bytes, checksum implicit or appended if required
//    rt_device_write(motor_uart, 0, cmd, sizeof(cmd));
//}
//
//
///* 立即停止命令 */
//
///**
// * Build and send a stop command to the motor.
// * @param addr       Device address (0x01–0xFF)
// * @param sync_flag  0x00: no sync, 0x01: enable sync
// */
//
//static void send_stop_command(uint8_t addr, uint8_t sync_flag)
//{
//    uint8_t cmd[5];
//    cmd[0] = addr;        /* 地址 */
//    cmd[1] = 0xFE;        /* 功能码：立即停止 */
//    cmd[2] = 0x98;        /* 停止子码 */
//    cmd[3] = sync_flag;   /* 同步标志 */
//    cmd[4] = 0x6B;        /* 校验 */
//    rt_device_write(motor_uart, 0, cmd, sizeof(cmd));
//}
//
///* 多机同步运动 */
//
///**
// * Build and send a multi-device sync move command.
// * This triggers all motors that have been preloaded with position commands.
// * @param addr       Broadcast address (0x00 for broadcast)
// */
//
//static void send_sync_move_command(uint8_t addr)
//{
//    uint8_t cmd[3];
//    cmd[0] = addr;       /* 地址: 使用广播地址如 0x00 */
//    cmd[1] = 0xFF;       /* 功能码：多机同步运动 */
//    cmd[2] = 0x66;       /* 子码: 同步启动 */
//    // 校验字节默认 0x6B，在此协议中当作最后一字节
//    rt_device_write(motor_uart, 0, cmd, sizeof(cmd));
//}
//
///************************************************************************/
//static void motor_thread_entry(void *parameter)
//{
//    /* 查找并打开 DMA 模式的 uart4 */
//    motor_uart = rt_device_find(MOTOR_UART_NAME);
//
//    if (!motor_uart)
//    {
//        rt_kprintf("Error: %s not found!\n", MOTOR_UART_NAME);
//        return;
//    }
//
//    if (rt_device_open(motor_uart,
//          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX) != RT_EOK)
//    {
//        rt_kprintf("Error: open %s failed!\n", MOTOR_UART_NAME);
//        return;
//    }
//
//    /* 示例：发送位置模式命令 */
//    send_position_command(
//        0x01,      /* 地址 */
//        500,       /* 500 RPM */
//        3200,      /* 3200 脉冲(16细分一圈) */
//        0x00,      /* 加速度 0 */
//        0x00,      /* 相对模式 */
//        0x00);     /* 不同步 */
//    rt_kprintf("Position command sent.\n");
//
//    rt_thread_mdelay(1000);
//
//    /* 示例：发送立即停止命令 */
//    send_stop_command(
//        0x01,      /* 地址 */
//        0x00);     /* 不同步 */
//    rt_kprintf("Stop command sent.\n");
//
//    /* 线程结束 */
//}
//
//int motor_thread_init(void)
//{
//    rt_thread_t tid = rt_thread_create("motor",
//                                      motor_thread_entry,
//                                      RT_NULL,
//                                      512,
//                                      20,
//                                      10);
//    if (tid)
//        rt_thread_startup(tid);
//    return 0;
//}
////INIT_APP_EXPORT(motor_thread_init);
