//#include <ultrasonic_ver2.0/Inc/ultrasonic.h>
//#include <rthw.h>
//
///* Global distance array */
//volatile float us_distance_cm[US_CHANNEL_NUM] = {0};
//
///* Semaphores for rising/falling edges */
//static struct rt_semaphore sem_start[US_CHANNEL_NUM];
//static struct rt_semaphore sem_end[US_CHANNEL_NUM];
//
///* Edge-capture state and timestamps */
//static uint8_t  echo_state[US_CHANNEL_NUM];
//static uint32_t t_start[US_CHANNEL_NUM];
//static uint32_t t_end[US_CHANNEL_NUM];
//
///* Pin arrays */
//static const rt_base_t trig_pins[US_CHANNEL_NUM] = {
//    PIN_TRIG_FRONT, PIN_TRIG_RIGHT, PIN_TRIG_BACK, PIN_TRIG_LEFT
//};
//static const rt_base_t echo_pins[US_CHANNEL_NUM] = {
//    PIN_ECHO_FRONT, PIN_ECHO_RIGHT, PIN_ECHO_BACK, PIN_ECHO_LEFT
//};
//
///* Common IRQ handler for all channels */
//static void echo_irq(void *args)
//{
//    int idx = (int)( (uintptr_t)args );
//    bool lvl = (rt_pin_read(echo_pins[idx]) == PIN_HIGH);
//    if (lvl && echo_state[idx] == 0)
//    {
//        t_start[idx] = __HAL_TIM_GET_COUNTER(&htim2);
//        rt_sem_release(&sem_start[idx]);
//        echo_state[idx] = 1;
//    }
//    else if (!lvl && echo_state[idx] == 1)
//    {
//        t_end[idx] = __HAL_TIM_GET_COUNTER(&htim2);
//        rt_sem_release(&sem_end[idx]);
//        echo_state[idx] = 0;
//    }
//}
//
///* Measurement thread entry */
//static void ultrasonic_thread_entry(void *parameter)
//{
//    while (1)
//    {
//        for (int i = 0; i < US_CHANNEL_NUM; i++)
//        {
//            /* Clear residual signals */
//            rt_sem_take(&sem_start[i], RT_WAITING_NO);
//            rt_sem_take(&sem_end[i],   RT_WAITING_NO);
//            echo_state[i] = 0;
//
//            /* Send 10µs trigger pulse */
//            rt_pin_write(trig_pins[i], PIN_LOW);
//            rt_thread_mdelay(2);
//            rt_pin_write(trig_pins[i], PIN_HIGH);
//            rt_hw_us_delay(10);
//            rt_pin_write(trig_pins[i], PIN_LOW);
//
//            /* Wait for rising + falling edge (up to 60ms) */
//            if (rt_sem_take(&sem_start[i], rt_tick_from_millisecond(60)) != RT_EOK ||
//                rt_sem_take(&sem_end[i],   rt_tick_from_millisecond(60)) != RT_EOK)
//            {
//                /* Timeout: skip this channel */
//                continue;
//            }
//
//            /* Calculate distance */
//            uint32_t us = (t_end[i] >= t_start[i])
//                          ? (t_end[i] - t_start[i])
//                          : (0xFFFF - t_start[i] + t_end[i] + 1);
//            float d = us * 0.01715f;
//            us_distance_cm[i] = d;
//        }
//
//        /* Print all four distances with two decimal places */
//        for (int i = 0; i < US_CHANNEL_NUM; i++)
//        {
//            int ip = (int)us_distance_cm[i];
//            int fp = (int)(us_distance_cm[i] * 100) % 100;
//            if (fp < 0) fp = -fp;
//            rt_kprintf("[%d]=%d.%02dcm ", i, ip, fp);
//        }
//        rt_kprintf("\r\n");
//
//        /* Pause 100ms before next round */
//        rt_thread_mdelay(100);
//    }
//}
//
///* Initialization: semaphores, pins, IRQs, timer, thread */
//int ultrasonic_init(void)
//{
//    for (int i = 0; i < US_CHANNEL_NUM; i++)
//    {
//        rt_sem_init(&sem_start[i],  "us_s", 0, RT_IPC_FLAG_FIFO);
//        rt_sem_init(&sem_end[i],    "us_e", 0, RT_IPC_FLAG_FIFO);
//
//        rt_pin_mode(trig_pins[i], PIN_MODE_OUTPUT);
//        rt_pin_write(trig_pins[i], PIN_LOW);
//
//        rt_pin_mode(echo_pins[i], PIN_MODE_INPUT_PULLDOWN);
//        rt_pin_attach_irq(echo_pins[i],
//                          PIN_IRQ_MODE_RISING_FALLING,
//                          echo_irq,
//                          (void*)(uintptr_t)i);
//        rt_pin_irq_enable(echo_pins[i], PIN_IRQ_ENABLE);
//    }
//
//    /* Start TIM2 as 1µs base timer */
//    HAL_TIM_Base_Start(&htim2);
//
//    /* Create and start the measurement thread */
//    rt_thread_t tid = rt_thread_create("us4",
//                                       ultrasonic_thread_entry,
//                                       RT_NULL,
//                                       1024,
//                                       11,
//                                       20);
//    if (tid)
//        rt_thread_startup(tid);
//
//    return tid ? RT_EOK : -RT_ERROR;
//}
//
///* Auto initialization */
//INIT_APP_EXPORT(ultrasonic_init);
