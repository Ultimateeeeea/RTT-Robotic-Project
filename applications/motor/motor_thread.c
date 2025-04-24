#include "motor_state.h"
#include "motor_cmd.h"
#include "motor_motion.h"

static struct rt_event evt;       // 创建事件集对象 /*注意这个事件集只有在该文件内能用*/
#define EVT_MOVE_FORWARD  (1<<0)    // 创建事件集内容
#define EVT_MOVE_STOP     (1<<1)

/* 运动状态扫描线程 */
static void state_thread(void *p)
{
    /* 初始化底层串口uart4 */
    motor_cmd_init("uart4");

    while (1)
    {
        rt_uint32_t set;
        if (rt_event_recv(&evt, 0xFFFF,
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          10, &set) == RT_EOK)
        {
            if (set & EVT_MOVE_FORWARD) motor_state_set(ST_FORWARD);
            if (set & EVT_MOVE_STOP)    motor_state_set(ST_IDLE);
        }
        motor_state_poll();           /* 每循环一次执行一次动作 */
        rt_thread_mdelay(20);         /* 50Hz 刷新 */
    }
}

/* 运动状态线程扫描开启函数 */
int motor_thread_init(void)
{
    rt_event_init(&evt, "evt", RT_IPC_FLAG_FIFO);
    rt_thread_t tid = rt_thread_create("motor",
                                       state_thread,
                                       RT_NULL,
                                       1024,
                                       15,
                                       10);
    if (tid) rt_thread_startup(tid);
    return 0;
}
/*****************************************************/
/* 作为测试用 */
static void test_event_thread(void *p)
{
    while (1)
    {
        rt_event_send(&evt, EVT_MOVE_FORWARD);
        rt_kprintf("[TEST] Sent FORWARD\n");
        rt_thread_mdelay(2000);  // 前进 2 秒

        rt_event_send(&evt, EVT_MOVE_STOP);
        rt_kprintf("[TEST] Sent STOP\n");
        rt_thread_mdelay(2000);  // 停止 2 秒
    }
}

int test_thread_init(void)
{
    rt_thread_t tid = rt_thread_create("test_evt",
                                       test_event_thread,
                                       RT_NULL,
                                       1024,
                                       20,
                                       10);
    if (tid) rt_thread_startup(tid);
    return 0;
}

INIT_APP_EXPORT(test_thread_init);
INIT_APP_EXPORT(motor_thread_init);
