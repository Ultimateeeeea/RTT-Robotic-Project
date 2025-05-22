//#include <rtthread.h>
//#include <rtdevice.h>
//#include "yolo_comm/Inc/yolo_serial_com.h"
//
//#define DBG_TAG "yolo.uart5"
//#define DBG_LVL DBG_LOG
//#include <rtdbg.h>
//
///* ====================== 协议/硬件参数 ====================== */
//#define UART_RX_NAME     "uart5"
//#define UART_BAUD        115200
//#define HDR_H            0xAA
//#define HDR_L            0x55
//
///* ====================== 线程资源 ====================== */
//#define UART_STACK       2048    /* 原 1KB 对密集打印可能不够，这里改为 2KB */
//#define UART_PRIO        25
//#define UART_TICK        10
//
//#define MS_TO_TICKS(ms) ((rt_tick_t)((ms) * RT_TICK_PER_SECOND / 1000))
//
//static rt_device_t uart_dev = RT_NULL;
//
///* ====================== 工具函数 ====================== */
//static rt_uint8_t xor_crc(const rt_uint8_t *d, rt_uint8_t len)
//{
//    rt_uint8_t c = 0;
//    while (len--) c ^= *d++;
//    return c;
//}
//
///* ====================== 负载处理 ====================== */
//static void handle_payload(const rt_uint8_t *pl, rt_uint8_t len)
//{
//    rt_uint8_t cmd = pl[0];
//
//    /* -------------------- 物品信息帧 -------------------- */
//    if (cmd == YOLO_CMD_ITEM_INFO)
//    {
//        /* host 若没有检测到目标，发送 cls = 0xFF ，长度至少为 2 */
//        if (len >= 2 && pl[1] == 0xFF)
//        {
//            /* 无物体 */
//            g_latest_yolo_result.detected     = RT_FALSE;
//            g_latest_yolo_result.x_offset_cm  = 0.0f;
//            g_latest_yolo_result.item_class   = 0;
//            g_new_yolo_data_flag              = RT_TRUE;
//
//            /* 打印节流：每 500ms 才输出一次 "No item detected"，避免刷屏 */
//            {
//                static rt_tick_t _last_print = 0;
//                if (rt_tick_get() - _last_print > MS_TO_TICKS(500))
//                {
//                    _last_print = rt_tick_get();
//                    rt_kprintf("YOLO RX: No item detected.\r\n");
//                }
//            }
//            return;
//        }
//
//        /* 正常物体信息：cls(1) + dx(2) [+ 可选字段]  长度应 ≥4 */
//        if (len >= 4)
//        {
//            rt_uint8_t  cls    = pl[1];
//
//            /* 仅处理 cls = 0,1,2,3,8 这五种，其余视为未检测到 */
//            if (!(cls == 0 || cls == 1 || cls == 2 || cls == 3 || cls == 8))
//            {
//                /* 非目标类别，视为未检测到 */
//                g_latest_yolo_result.detected = RT_FALSE;
//                g_new_yolo_data_flag = RT_TRUE;
//                return;
//            }
//
//            rt_int16_t dx_raw = (rt_int16_t)(pl[2] | (pl[3] << 8));
//
//            /* 记录到全局结果（仍然用 cm 浮点保存，打印时转整型）*/
//            g_latest_yolo_result.x_offset_cm = (float)dx_raw / 10.0f;
//            g_latest_yolo_result.item_class  = cls;
//            g_latest_yolo_result.detected    = RT_TRUE;
//            g_new_yolo_data_flag             = RT_TRUE;
//
//            /* rt_kprintf 不支持 %f，改用整数打印 (单位: mm) */
//            rt_kprintf("YOLO RX: len:%d cls:%d, dx_raw:%dmm\r\n", len, cls, dx_raw);
//
//            if (cls == last_cls)  same_cnt++;
//            else { same_cnt = 1; last_cls = cls; }
//            if (same_cnt >= CONFIRM_N) {
//                g_latest_yolo_result.confirmed = RT_TRUE;
//                same_cnt = 0;              /* 或者锁 1 秒窗口 */
//            }
//            return;
//        }
//    }
//
//    /* -------------------- 其它未知帧 -------------------- */
//    rt_kprintf("YOLO RX: Unknown cmd:0x%02X len:%d\r\n", cmd, len);
//    g_latest_yolo_result.detected = RT_FALSE;
//    g_new_yolo_data_flag          = RT_TRUE;
//}
//
///* ====================== UART 接收线程 ====================== */
//static void uart_rx_entry(void *parameter)
//{
//    enum { WAIT_H, WAIT_L, WAIT_LEN, WAIT_DATA } state = WAIT_H;
//    rt_uint8_t frame[32];
//    rt_uint8_t need  = 0, idx = 0, ch;
//
//    while (1)
//    {
//        if (rt_device_read(uart_dev, 0, &ch, 1) != 1)
//        {
//            rt_thread_mdelay(1);
//            continue;
//        }
//
//        switch (state)
//        {
//        case WAIT_H:  state = (ch == HDR_H) ? WAIT_L : WAIT_H; break;
//        case WAIT_L:  state = (ch == HDR_L) ? WAIT_LEN : WAIT_H; break;
//        case WAIT_LEN:
//            need = ch + 1;                    /* len + crc */
//            idx  = 0;
//            frame[idx++] = ch;                /* 保存 len */
//            state = WAIT_DATA;
//            break;
//        case WAIT_DATA:
//            frame[idx++] = ch;
//            if (--need == 0)                  /* 一帧收完 */
//            {
//                rt_uint8_t crc_calc = xor_crc(frame, idx - 1);
//                if (crc_calc == frame[idx - 1])
//                    handle_payload(&frame[1], frame[0]); /* frame[0]=len */
//                else
//                    LOG_W("CRC mismatch calc:0x%02X recv:0x%02X", crc_calc, frame[idx-1]);
//                state = WAIT_H;               /* 重新找下一帧 */
//            }
//            break;
//        }
//    }
//}
//
///* ====================== 初始化函数 (替代 main) ====================== */
//int yolo_uart5_init(void)
//{
//    uart_dev = rt_device_find(UART_RX_NAME);
//    if (!uart_dev)
//    {
//        LOG_E("Device %s not found!", UART_RX_NAME);
//        return -RT_ENOSYS;
//    }
//
//    struct serial_configure cfg = RT_SERIAL_CONFIG_DEFAULT;
//    cfg.baud_rate = UART_BAUD;
//    rt_device_control(uart_dev, RT_DEVICE_CTRL_CONFIG, &cfg);
//    if (rt_device_open(uart_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
//    {
//        LOG_E("Failed to open %s", UART_RX_NAME);
//        return -RT_ERROR;
//    }
//
//    rt_thread_t tid = rt_thread_create("uart5_rx",
//                                       uart_rx_entry,
//                                       RT_NULL,
//                                       UART_STACK,
//                                       UART_PRIO,
//                                       UART_TICK);
//    if (tid)
//        rt_thread_startup(tid);
//    else
//        LOG_E("Failed to start uart5_rx thread");
//
//    return RT_EOK;
//}
//INIT_APP_EXPORT(yolo_uart5_init);
//
//// 如果需要在shell中手动初始化，可以添加MSH命令
//#ifdef RT_USING_FINSH
//#include <finsh.h>
//static void yolo_init_cmd(int argc, char**argv)
//{
//    if (argc > 1) {
//        yolo_uart5_init();
//    } else {
//        rt_kprintf("Usage: yolo_init_cmd <uart_device_name>\n");
//        rt_kprintf("Example: yolo_init_cmd uart5\n");
//    }
//}
//MSH_CMD_EXPORT(yolo_init_cmd, Initialize YOLO serial communication: yolo_init_cmd uartX);
//#endif
//
//volatile yolo_detection_result_t g_latest_yolo_result = {RT_FALSE, 0, 0.0f};
//volatile rt_bool_t g_new_yolo_data_flag = RT_FALSE;
//
//#define CONFIRM_N  3          /* 连续 N 帧一致才算真 */
//static rt_uint8_t same_cnt = 0;
//static rt_uint8_t last_cls = 0;
