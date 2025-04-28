#include "motor_cmd.h"

static rt_device_t motor_uart = RT_NULL;

/* 打开步进电机串口 */

rt_err_t motor_cmd_init(const char *uart_name)
{
    motor_uart = rt_device_find(uart_name);

    if (motor_uart)
    {
        rt_kprintf("%s is opened successfully\n", uart_name);
    }

    if (!motor_uart) return -RT_ERROR;    //找不到返回-1
    return rt_device_open(motor_uart, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);   //开启读写与DMA的收
}

/* 串口发送数据 */
static void send_frame(const uint8_t *buf, size_t len)
{
    rt_device_write(motor_uart, 0, buf, len);
    rt_thread_mdelay(2);    /* 等 2ms发送数据 */
}

/*************************  步进电机的指令 *****************************/
/* 速度模式控制 */
rt_err_t motor_cmd_speed(uint8_t addr,
                         uint8_t dir,
                         uint16_t rpm,
                         uint8_t accel,
                         uint8_t sync_flag)
{
    uint8_t cmd[8];

    cmd[0] = addr;                    // 设备地址
    cmd[1] = 0xF6;                    // 功能码：速度模式
    cmd[2] = dir;                     // 方向
    cmd[3] = (rpm >> 8) & 0xFF;       // 速度高字节
    cmd[4] =  rpm        & 0xFF;      // 速度低字节
    cmd[5] = accel;                   // 加速度档位
    cmd[6] = sync_flag;               // 同步标志：1 同步，0 不同步
    cmd[7] = 0x6B;                    // 校验字节

    send_frame(cmd, sizeof(cmd));     // 发送整帧
    return RT_EOK;                    // 返回成功
}


/* 位置闭环命令 */
rt_err_t motor_cmd_position(uint8_t addr,
                                  uint8_t dir,
                                  uint16_t rpm,
                                  uint8_t accel,
                                  uint32_t pulses,
                                  uint8_t rel_abs,
                                  uint8_t sync_flag)
{
    uint8_t cmd[13];
    cmd[0] = addr;                    // 地址
    cmd[1] = 0xFD;                    // 功能码：位置模式
    cmd[2] = dir;                     // 方向
    cmd[3] = (rpm >> 8) & 0xFF;       // 速度高字节
    cmd[4] = rpm & 0xFF;              // 速度低字节
    cmd[5] = accel;                   // 加速度档位
    cmd[6] = (pulses >> 24) & 0xFF;   // 脉冲数字节3
    cmd[7] = (pulses >> 16) & 0xFF;   // 脉冲数字节2
    cmd[8] = (pulses >> 8) & 0xFF;    // 脉冲数字节1
    cmd[9] = pulses & 0xFF;           // 脉冲数字节0
    cmd[10] = rel_abs;                // 相对/绝对
    cmd[11] = sync_flag;              // 同步标志
    cmd[12] = 0x6B;                   // 校验字节

    send_frame(cmd, sizeof(cmd));
    return RT_EOK;
}


/* 立即停止命令 */
rt_err_t motor_cmd_stop(uint8_t addr, uint8_t sync_flag)
{
    uint8_t cmd[5];
    cmd[0] = addr;        /* 地址 */
    cmd[1] = 0xFE;        /* 功能码：立即停止 */
    cmd[2] = 0x98;        /* 停止子码 */
    cmd[3] = sync_flag;   /* 同步标志 */
    cmd[4] = 0x6B;        /* 校验 */

    send_frame(cmd, sizeof(cmd));
    rt_thread_mdelay(5);
    return RT_EOK;
}

/* 多机同步运动 */
rt_err_t motor_cmd_sync(uint8_t addr)
{
    uint8_t cmd[4];
    cmd[0] = addr;       /* 地址: 使用广播地址如 0x00 */
    cmd[1] = 0xFF;       /* 功能码：多机同步运动 */
    cmd[2] = 0x66;       /* 子码: 同步启动 */
    cmd[3] = 0x6B;       /* 校验字节*/
    // 校验字节默认 0x6B，在此协议中当作最后一字节

    send_frame(cmd, sizeof(cmd));
    return RT_EOK;
}

/************************************************************************/
