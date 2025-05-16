#include <ultrasonic/Inc/ultrasonic.h>
#include <rthw.h>

/* 测距参数 */
#define CYCLE_MS        100         /* 总周期：100 ms 一次测距 */
#define WINDOW_MS       30          /* 窗口时长：30 ms 最长声波往返 */
#define MAX_TIMEOUT_US  (WINDOW_MS * 1000UL)  /* 最大脉宽：30 000 µs */

volatile float UltrasonicDistance_cm[SENSOR_COUNT] = {0};

/* 状态机数组：窗口打开标志 & 捕获阶段 */
static volatile bool    measuring[SENSOR_COUNT];
static volatile uint8_t capture_state[SENSOR_COUNT];  /* 0=待上升, 1=待下降 */

/* 上升沿时间戳 (µs) */
static uint32_t t_start_us[SENSOR_COUNT];

/**
 * @brief Echo 中断回调，由 RT-Thread 底层的 EXTI4/5/6/7_IRQHandler 调用
 * @param args 传入的传感器序号 (0~3)
 */
static void echo_irq_handler(void *args)
{
    int idx = (int)( (rt_base_t)args );  /* 用 rt_base_t 代替 rt_intptr_t */
    if (idx < 0 || idx >= SENSOR_COUNT) return;

    if (!measuring[idx]) return;

    if (capture_state[idx] == 0 && rt_pin_read(echo_pins[idx]) == PIN_HIGH)
    {
        t_start_us[idx]   = __HAL_TIM_GET_COUNTER(&htim2);
        capture_state[idx] = 1;
    }
    else if (capture_state[idx] == 1 && rt_pin_read(echo_pins[idx]) == PIN_LOW)
    {
        uint32_t t_end_us = __HAL_TIM_GET_COUNTER(&htim2);
        uint32_t diff;
        if (t_end_us >= t_start_us[idx])
            diff = t_end_us - t_start_us[idx];
        else
            diff = (0x10000UL - t_start_us[idx]) + t_end_us;

        if (diff > 0 && diff <= MAX_TIMEOUT_US)
            UltrasonicDistance_cm[idx] = diff * 0.01715f;

        measuring[idx]     = false;
        capture_state[idx] = 0;
    }
}


/**
 * @brief 四路超声波测距线程
 */
static void ultrasonic_thread_entry(void *parameter)
{
    HAL_TIM_Base_Start(&htim2);

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        rt_pin_mode(trig_pins[i], PIN_MODE_OUTPUT);
        rt_pin_write(trig_pins[i], PIN_LOW);

        rt_pin_mode(echo_pins[i], PIN_MODE_INPUT_PULLDOWN);
        rt_pin_attach_irq(echo_pins[i],
                          PIN_IRQ_MODE_RISING_FALLING,
                          echo_irq_handler,
                          (void *)(rt_base_t)i);  /* 用 rt_base_t 转回 void* */
        rt_pin_irq_enable(echo_pins[i], PIN_IRQ_ENABLE);
    }

    while (1)
    {
        for (int i = 0; i < SENSOR_COUNT; i++)
        {
            measuring[i]     = true;
            capture_state[i] = 0;
        }

        rt_thread_mdelay(2);
        for (int i = 0; i < SENSOR_COUNT; i++)
            rt_pin_write(trig_pins[i], PIN_HIGH);
        rt_hw_us_delay(10);
        for (int i = 0; i < SENSOR_COUNT; i++)
            rt_pin_write(trig_pins[i], PIN_LOW);

        rt_thread_mdelay(WINDOW_MS);
        for (int i = 0; i < SENSOR_COUNT; i++)
            measuring[i] = false;

        rt_kprintf("Front: %.2f cm\tRight: %.2f cm\tBack: %.2f cm\tLeft: %.2f cm\n",
                   UltrasonicDistance_cm[0],
                   UltrasonicDistance_cm[1],
                   UltrasonicDistance_cm[2],
                   UltrasonicDistance_cm[3]);

        rt_thread_mdelay(CYCLE_MS - WINDOW_MS);
    }
}


/**
 * @brief 初始化四路超声波测距
 */
int ultrasonic_thread_init(void)
{
    rt_thread_t tid = rt_thread_create("us_all",
                                       ultrasonic_thread_entry,
                                       RT_NULL,
                                       2048,   /* 2 KB 栈，足够浮点输出 */
                                       10,     /* 优先级，可根据系统调整 */
                                       20);    /* 时间片 */
    if (tid)
        rt_thread_startup(tid);
    return tid ? RT_EOK : -RT_ERROR;
}

/* 在应用初始化阶段自动调用 */
INIT_APP_EXPORT(ultrasonic_thread_init);
/**/
