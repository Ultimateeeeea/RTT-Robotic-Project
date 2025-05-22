#ifndef  __SERVO_UART3_H__
#define  __SERVO_UART3_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <stdbool.h>

#define FRAME_HEADER 0x55      /* Lobot 协议同步头 */
#define RX_BUF_SIZE  32        /* 单帧最大长度 ≤ 16，这里开 32 防溢出 */

int  servo_uart3_init(void);   /* 在应用初始化阶段调用即可 */
void servo_uart3_write(const uint8_t *buf, rt_size_t len);

extern volatile bool  servo_pkt_ready;     /* 接收完成标志  */
extern uint8_t        servo_pkt_buf[16];   /* 完整包缓存    */

#endif
