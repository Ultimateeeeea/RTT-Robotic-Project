 #include "arm\Inc\servo_uart3.h"
 #include <string.h>

 /* === 私有全局 === */
 static rt_device_t  u3_dev       = RT_NULL;
 static rt_sem_t     rx_sem       = RT_NULL;
 static uint8_t      rx_ring[RX_BUF_SIZE];
 static rt_size_t    ring_len     = 0;

 volatile bool  servo_pkt_ready   = false;
 uint8_t        servo_pkt_buf[16] = {0};

 /* ---------- 发送 ---------- */
 void servo_uart3_write(const uint8_t *buf, rt_size_t len)
 {
    if (u3_dev)
        rt_device_write(u3_dev, 0, buf, len);
 }

 /* ---------- 串口中断回调 ---------- */
 static rt_err_t u3_rx_ind(rt_device_t dev, rt_size_t size)
 {
    /* 仅通知有数据到达，让后台线程去读 */
    rt_sem_release(rx_sem);
    return RT_EOK;
 }

 /* ---------- 后台解析线程 ---------- */
 static void u3_parse_thread(void *parameter)
 {
    while (1)
    {
        /* 等待中断通知 */
        rt_sem_take(rx_sem, RT_WAITING_FOREVER);

        /* 把 FIFO 中全部读出来放进 ring buffer */
        rt_size_t n = rt_device_read(u3_dev, 0,
                                     rx_ring + ring_len,
                                     RX_BUF_SIZE - ring_len);
        ring_len += n;

        /* 尝试解析若干帧 */
        while (ring_len >= 4)                       /* 至少能包含 55 55 ID LEN */
        {
            /* 1) 找同步头 55 55 */
            if (!(rx_ring[0] == FRAME_HEADER && rx_ring[1] == FRAME_HEADER))
            {
                /* 丢弃一个字节后继续找头 */
                memmove(rx_ring, rx_ring + 1, --ring_len);
                continue;
            }

 //            uint8_t id  = rx_ring[2];
            uint8_t len = rx_ring[3];               /* CMD+DATA 字节数 */

            /* 参数极限检查 */
            if (len < 2      ||                     /* 至少 CMD+1data */
                len > 13     ||                     /* 避免越界 */
                ring_len < 4 + len)                 /* 数据是否已收全 */
            {
                break;                              /* 数据还不完整，等待下次中断 */
            }

            /* 2) 复制出完整包：ID LEN CMD DATA… */
            memcpy(servo_pkt_buf, &rx_ring[2], 2 + len);
            servo_pkt_ready = true;

            /* 3) ring buffer 删除此帧 */
            rt_size_t frame_bytes = 4 + len;        /* 55 55 + ID LEN + data */
            ring_len -= frame_bytes;
            memmove(rx_ring, rx_ring + frame_bytes, ring_len);
        }
    }
 }

 /* ---------- 公共初始化 ---------- */
 int servo_uart3_init(void)
 {
    /* 1. 找到设备并开启中断收 */
    u3_dev = rt_device_find("uart3");
    if (!u3_dev)
    {
        rt_kprintf("❌ uart3 not found!\n");
        return -RT_ERROR;
    }
    /* 配置串口参数：9600-8-N-1 */
    struct serial_configure cfg = RT_SERIAL_CONFIG_DEFAULT;
    cfg.baud_rate = BAUD_RATE_9600;
    rt_device_control(u3_dev, RT_DEVICE_CTRL_CONFIG, &cfg);

    if (rt_device_open(u3_dev, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("❌ uart3 open failed!\n");
        return -RT_ERROR;
    }

    /* 2. 注册 RX 回调 */
    rt_device_set_rx_indicate(u3_dev, u3_rx_ind);

    /* 3. 信号量 + 解析线程 */
    rx_sem = rt_sem_create("u3rx", 0, RT_IPC_FLAG_FIFO);
    RT_ASSERT(rx_sem);

    rt_thread_t tid = rt_thread_create("u3srv",
                                       u3_parse_thread,
                                       RT_NULL,
                                       1024,        /* 栈 */
                                       16,          /* 优先级 */
                                       10);         /* 时间片 */
    RT_ASSERT(tid);
    rt_thread_startup(tid);

    rt_kprintf("uart3 servo init OK.\n");
    return 0;
 }
 INIT_APP_EXPORT(servo_uart3_init);   /* 系统自动执行 */
