//#include <ultrasonic_ver2.0/Inc/ultrasonic.h>
//#include <rthw.h>
//#include <stdlib.h> // 用于 qsort 函数
//
//#define ULTRASONIC_FILTER_SAMPLES 5 // 中位值滤波的样本数量，通常取奇数
//
//// 用于存储每个传感器最近N个样本的缓冲区
//static float ultrasonic_sample_buffer[US_CHANNEL_NUM][ULTRASONIC_FILTER_SAMPLES];
//// 用于记录每个传感器当前样本缓冲区的填充位置索引
//static int ultrasonic_sample_index[US_CHANNEL_NUM] = {0};
//// 用于记录每个传感器样本缓冲区是否已至少被完整填充过一次的标志
//static rt_bool_t ultrasonic_buffer_filled[US_CHANNEL_NUM] = {RT_FALSE};
//
//// qsort 使用的比较函数，用于float类型升序排序
//static int compare_float(const void *a, const void *b) {
//    float fa = *(const float*) a;
//    float fb = *(const float*) b;
//    if (fa < fb) return -1;
//    if (fa > fb) return 1;
//    return 0;
//    // return (fa > fb) - (fa < fb); // 更简洁的写法，但在某些编译器上若bool提升为int非0/1时可能存在移植性问题
//}
//
///**
// * @brief 对单个传感器的原始距离数据应用中位值滤波
// *
// * @param sensor_id 传感器ID (0 到 US_CHANNEL_NUM-1)
// * @param raw_distance 该传感器本次测量的原始距离值
// * @return float 滤波后的距离值
// * @note 滤波会引入一定的延迟，大小约为 (ULTRASONIC_FILTER_SAMPLES - 1) / 2 个采样周期。
// *       这可能会影响依赖距离突变进行判断的逻辑（如快速边缘检测）。
// */
//static float apply_median_filter(int sensor_id, float raw_distance) {
//    if (sensor_id < 0 || sensor_id >= US_CHANNEL_NUM) {
//        // 正常情况下不应发生此错误，如果发生则直接返回原始值
//        rt_kprintf("错误：无效的传感器ID %d 用于滤波\n", sensor_id);
//        return raw_distance;
//    }
//
//    // 存储新的样本到缓冲区
//    ultrasonic_sample_buffer[sensor_id][ultrasonic_sample_index[sensor_id]] = raw_distance;
//    ultrasonic_sample_index[sensor_id] = (ultrasonic_sample_index[sensor_id] + 1) % ULTRASONIC_FILTER_SAMPLES;
//
//    // 如果缓冲区刚刚被完整填充（即sample_index回到0），则设置填充完成标志
//    if (!ultrasonic_buffer_filled[sensor_id] && ultrasonic_sample_index[sensor_id] == 0) {
//        ultrasonic_buffer_filled[sensor_id] = RT_TRUE;
//    }
//
//    // 如果缓冲区尚未被完整填充过，则返回原始距离值
//    // （也可以考虑返回当前已采集样本的平均值等，但为简单起见，在填满前返回原始值）
//    if (!ultrasonic_buffer_filled[sensor_id]) {
//        return raw_distance;
//    }
//
//    // 将当前传感器的样本复制到临时数组中以便排序（qsort会修改原数组）
//    float sorted_samples[ULTRASONIC_FILTER_SAMPLES];
//    for (int i = 0; i < ULTRASONIC_FILTER_SAMPLES; ++i) {
//        sorted_samples[i] = ultrasonic_sample_buffer[sensor_id][i];
//    }
//    // 使用qsort对样本进行升序排序
//    qsort(sorted_samples, ULTRASONIC_FILTER_SAMPLES, sizeof(float), compare_float);
//
//    // 返回排序后的中位值
//    return sorted_samples[ULTRASONIC_FILTER_SAMPLES / 2];
//}
//
///* 全局距离数组，存储各通道测量到的距离值（单位：厘米） */
//volatile float us_distance_cm[US_CHANNEL_NUM] = {0};
//
///* 用于上升沿和下降沿捕获的信号量 */
//static struct rt_semaphore sem_start[US_CHANNEL_NUM];
//static struct rt_semaphore sem_end[US_CHANNEL_NUM];
//
///* 回波捕获状态和时间戳（基于htim2的计数值） */
//static uint8_t  echo_state[US_CHANNEL_NUM];    // 0: 等待上升沿, 1: 等待下降沿
//static uint32_t t_start[US_CHANNEL_NUM];       // 上升沿时间戳
//static uint32_t t_end[US_CHANNEL_NUM];         // 下降沿时间戳
//
///* Trig 引脚数组 (顺序: 前, 右, 后, 左) */
//static const rt_base_t trig_pins[US_CHANNEL_NUM] = {
//    PIN_TRIG_FRONT, PIN_TRIG_RIGHT, PIN_TRIG_BACK, PIN_TRIG_LEFT
//};
///* Echo 引脚数组 (顺序: 前, 右, 后, 左) */
//static const rt_base_t echo_pins[US_CHANNEL_NUM] = {
//    PIN_ECHO_FRONT, PIN_ECHO_RIGHT, PIN_ECHO_BACK, PIN_ECHO_LEFT
//};
//
///* 所有Echo引脚共用的中断服务函数 */
//static void echo_irq(void *args)
//{
//    int idx = (int)( (uintptr_t)args ); // 从参数获取传感器索引
//    rt_base_t current_echo_pin = echo_pins[idx];
//    rt_bool_t lvl = (rt_pin_read(current_echo_pin) == PIN_HIGH); // 读取当前电平
//
//    if (lvl && echo_state[idx] == 0) // 上升沿，并且当前状态是等待上升沿
//    {
//        t_start[idx] = __HAL_TIM_GET_COUNTER(&htim2); // 记录上升沿时间戳
//        rt_sem_release(&sem_start[idx]);              // 释放开始信号量
//        echo_state[idx] = 1;                          // 更新状态为等待下降沿
//    }
//    else if (!lvl && echo_state[idx] == 1) // 下降沿，并且当前状态是等待下降沿
//    {
//        t_end[idx] = __HAL_TIM_GET_COUNTER(&htim2);   // 记录下降沿时间戳
//        rt_sem_release(&sem_end[idx]);                // 释放结束信号量
//        echo_state[idx] = 0;                          // 重置状态为等待上升沿
//    }
//}
//
///* 超声波测量线程入口函数 */
//static void ultrasonic_thread_entry(void *parameter)
//{
//    while (1)
//    {
//        for (int i = 0; i < US_CHANNEL_NUM; i++) // 轮询测量每个通道
//        {
//            /* 清除上一次测量可能残留的信号量 */
//            rt_sem_take(&sem_start[i], RT_WAITING_NO); // 非阻塞获取
//            rt_sem_take(&sem_end[i],   RT_WAITING_NO); // 非阻塞获取
//            echo_state[i] = 0; // 重置回波捕获状态
//
//            /* 发送 10µs 的触发脉冲 */
//            rt_pin_write(trig_pins[i], PIN_LOW);
//            rt_thread_mdelay(2); // 短暂延时确保稳定
//            rt_pin_write(trig_pins[i], PIN_HIGH);
//            rt_hw_us_delay(10);  // 精确延时10微秒
//            rt_pin_write(trig_pins[i], PIN_LOW);
//
//            /* 等待回波的上升沿和下降沿，超时时间设置为60ms */
//            // HC-SR04有效测量范围约2cm-400cm，对应回波时间约116µs - 23320µs (23ms)
//            // 60ms超时足以覆盖大部分情况，并能检测到无反射或超出范围的情况
//            if (rt_sem_take(&sem_start[i], rt_tick_from_millisecond(60)) != RT_EOK ||
//                rt_sem_take(&sem_end[i],   rt_tick_from_millisecond(60)) != RT_EOK)
//            {
//                /* 超时或信号量获取失败，则跳过当前通道的本次测量 */
//                // 可以考虑在此处给 us_distance_cm[i] 赋一个特定值（如最大量程或错误码）
//                // 当前简单跳过，保留上一次的值或初始值0
//                rt_kprintf("传感器 %d 测量超时\n", i);
//                continue;
//            }
//
//            /* 计算回波脉冲宽度 (单位: 微秒) */
//            uint32_t pulse_width_us;
//            if (t_end[i] >= t_start[i]) {
//                pulse_width_us = t_end[i] - t_start[i];
//            } else { // 处理定时器计数器溢出的情况 (htim2是16位定时器，0xFFFF)
//                pulse_width_us = (htim2.Instance->ARR - t_start[i]) + t_end[i] + 1; // ARR是自动重装载值
//            }
//
//            // 原始距离计算：声速约为343m/s，即0.0343 cm/µs。
//            // 距离 = (脉冲宽度 * 声速) / 2
//            // 0.0343 / 2 = 0.01715
//            float raw_dist_cm = pulse_width_us * 0.01715f;
//
//            // 应用中位值滤波
//            us_distance_cm[i] = apply_median_filter(i, raw_dist_cm);
//        }
//
//        /* 打印所有四个通道的距离值，保留两位小数 */
//        // rt_kprintf("超声波距离(cm): "); // 可以取消下面循环前的统一打印头
//        for (int i = 0; i < US_CHANNEL_NUM; i++)
//        {
//            int integer_part = (int)us_distance_cm[i];
//            int fractional_part = (int)(us_distance_cm[i] * 100) % 100;
//            if (fractional_part < 0) fractional_part = -fractional_part; // 处理负数距离（虽然不常见）
//            rt_kprintf("通道[%d]:%d.%02dcm  ", i, integer_part, fractional_part);
//        }
//        rt_kprintf("\r\n");
//
//        /* 暂停100ms，开始下一轮测量 (整体测量频率约为10Hz，每个通道也是) */
//        rt_thread_mdelay(100);
//    }
//}
//
///* 初始化函数：信号量、引脚模式、中断、定时器和测量线程 */
//int ultrasonic_init(void)
//{
//    for (int i = 0; i < US_CHANNEL_NUM; i++)
//    {
//        char sem_name_s[RT_NAME_MAX];
//        char sem_name_e[RT_NAME_MAX];
//        rt_sprintf(sem_name_s, "us_s%d", i);
//        rt_sprintf(sem_name_e, "us_e%d", i);
//
//        rt_sem_init(&sem_start[i],  sem_name_s, 0, RT_IPC_FLAG_FIFO);
//        rt_sem_init(&sem_end[i],    sem_name_e, 0, RT_IPC_FLAG_FIFO);
//
//        rt_pin_mode(trig_pins[i], PIN_MODE_OUTPUT);
//        rt_pin_write(trig_pins[i], PIN_LOW); // 初始拉低Trig引脚
//
//        rt_pin_mode(echo_pins[i], PIN_MODE_INPUT_PULLDOWN); // Echo引脚配置为下拉输入
//        rt_pin_attach_irq(echo_pins[i],
//                          PIN_IRQ_MODE_RISING_FALLING, // 上升沿和下降沿均触发中断
//                          echo_irq,
//                          (void*)(uintptr_t)i); // 将传感器索引作为中断回调参数
//        rt_pin_irq_enable(echo_pins[i], PIN_IRQ_ENABLE); // 使能引脚中断
//    }
//
//    /* 启动 TIM2 作为1µs基准的定时器 */
//    // 注意：htim2 的预分频和自动重装载值需要在CubeMX中配置好，以确保计数周期为1µs
//    // 例如，如果APB1时钟为72MHz，预分频PSC=71，则TIM2时钟为72MHz/(71+1)=1MHz，周期1µs
//    // ARR (自动重装载值) 可以设置为最大0xFFFF，或者根据需要的最大测量时间设置
//    HAL_TIM_Base_Start(&htim2);
//
//    /* 创建并启动超声波测量线程 */
//    rt_thread_t tid = rt_thread_create("us_poll", // 修改线程名为 us_poll 更能反映其轮询特性
//                                       ultrasonic_thread_entry,
//                                       RT_NULL,
//                                       1024,   // 线程栈大小 (字节)
//                                       11,     // 线程优先级
//                                       20);    // 时间片 (ticks)
//    if (tid)
//        rt_thread_startup(tid);
//    else
//        rt_kprintf("创建超声波测量线程失败!\n");
//
//    return tid ? RT_EOK : -RT_ERROR;
//}
//
///* 自动初始化，由RT-Thread的INIT_APP_EXPORT机制调用 */
//INIT_APP_EXPORT(ultrasonic_init);
