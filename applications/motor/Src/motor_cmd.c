#include "motor/Inc/motor_cmd.h"

static rt_device_t motor_uart = RT_NULL;
#define READ_TIMEOUT_MS  20   // 最长等待 20ms，可根据波特率/场景调
#define MS_TO_TICKS(ms)  ((ms) * RT_TICK_PER_SECOND / 1000)

/* 打开步进电机串口uart4 */
rt_err_t motor_cmd_init(const char *uart_name)
{
   motor_uart = rt_device_find(uart_name);

   if (motor_uart != RT_NULL)
   {
       rt_kprintf("Device %s found.\n", uart_name);
       rt_err_t open_result = rt_device_open(motor_uart, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX); // 使用中断接收通常更好
       if (open_result == RT_EOK)
       {
           rt_kprintf("Device %s opened successfully.\n", uart_name);
           return RT_EOK;
       }
       else
       {
           rt_kprintf("Failed to open device %s, error code: %d\n", uart_name, open_result);
           motor_uart = RT_NULL; // 打开失败，将其重置为NULL
           return open_result;
       }
   }
   else
   {
       rt_kprintf("Device %s not found.\n", uart_name);
       return -RT_ERROR;
   }
}

/* 串口发送数据 */
static void send_frame(const uint8_t *buf, size_t len)
{
   rt_device_write(motor_uart, 0, buf, len);
   rt_thread_mdelay(2);    /* 等 2ms发送数据 */
}

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

///* 读取实时转速命令(非阻塞式) */
//rt_err_t motor_cmd_read_rpm_nb(uint8_t addr, int16_t *rpm)
//{
//  uint8_t cmd[3] = { addr, 0x35, 0x6B };
//  uint8_t buf[16];
//  uint8_t rx;
//  int     idx = 0;
//
//  rt_tick_t start = rt_tick_get();
//  rt_tick_t timeout = MS_TO_TICKS(READ_TIMEOUT_MS);
//
//  // 1) 丢掉旧数据
//  while (rt_device_read(motor_uart, 0, &rx, 1) == 1) { }
//
//  // 2) 发命令
//  send_frame(cmd, sizeof(cmd));
//
//  // 3) 逐字节非阻塞读，直到超时或找到一帧
//  while ((rt_tick_get() - start) < timeout)
//  {
//      if (rt_device_read(motor_uart, 0, &rx, 1) == 1)
//      {
//          rt_kprintf("RX: %02X | ", rx);
//          // 把新字节推入滑动窗口缓冲
//          if (idx < sizeof(buf)) buf[idx++] = rx;
//          else                   // 缓冲满就保留最新 16 字节
//          {
//              memmove(buf, buf + 1, sizeof(buf) - 1);
//              buf[sizeof(buf) - 1] = rx;
//          }
//
//          // 只要缓冲 >=6，就检查最后一段是否为一帧
//          if (idx >= 6)
//          {
//              int base = idx - 6;
//              if (buf[base]   == addr    &&
//                  buf[base+1] == 0x35    &&
//                  buf[base+5] == 0x6B)
//              {
//                  // 解析
//                  uint16_t mag = (buf[base+3] << 8) | buf[base+4];
//                  int16_t  v   = (int16_t)mag;
//                  // 按方向打负号
//                  if (buf[base+2] & 0x01) v = -v;
//                  *rpm = v;
////                   rt_thread_mdelay(50);
////                    rt_kprintf("%d\n",v);
//                  return RT_EOK;
//              }
//          }
//      }
//      else
//      {
//          // 没读到数据，就稍微休息一下，yield CPU
//          rt_thread_mdelay(1);
//      }
//  }
//  rt_kprintf("Read RPM timeout\n");
//  return -RT_ETIMEOUT;
//}

/************************************************************************/
