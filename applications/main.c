/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-14     RT-Thread    first version
 */

#include <rtthread.h>
//#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "dma.h"


int front_left_count = 0;


//void encoder_thread_entry(void *parameter)
//{
//    // 启动编码器模式
//       if (HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL) != HAL_OK)
//       {
//           // 启动失败的错误处理
//           rt_kprintf("Failed to start encoder mode!\n");
//           return; // 这是什么意思？
//       }
//
//
//    while (1)
//    {
//        // 读取 TIM1 的编码器计数器值
//        front_left_count = __HAL_TIM_GET_COUNTER(&htim1);  // 获取编码器计数值
//        __HAL_TIM_SET_COUNTER(&htim1, 0);
//        //输出编码器的值
//        rt_kprintf("Encoder Count: %d\n", front_left_count);
//
//        // 每 10ms 采样一次
//        rt_thread_mdelay(10);  // 延时 100ms
//    }
//}
//
//void pwm_entry(void *parameter)
//{
//    rt_kprintf("PWM OK!\n");
//    // 可以在这里添加代码来修改占空比或做其他操作
//    while (1)
//    {
//        // 每隔一段时间改变 PWM 占空比
//        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 1800);  // 调整左前轮占空比
//        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, 1800);  // 调整右前轮占空比
//        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, 1800);  // 调整左后轮占空比
//        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, 1800);  // 调整右后轮占空比
//        rt_thread_mdelay(100);  // 每100MS调整一次占空比
//    }
//}

//void Test_1_entry(void *p)
//{
//    while(1)
//    {
//        rt_kprintf("I'm One!\n");
//        rt_thread_mdelay(1000);
//    }
//}
//
//void Test_2_entry(void *p)
//{
//    while(1)
//    {
//        rt_kprintf("I'm Two!\n");
//        rt_thread_mdelay(500);
//    }
//}


int main(void)
{
//    static rt_thread_t tid;

    /* 线程调度 */
//       tid = rt_thread_create("encoder_thread_entry", encoder_thread_entry, RT_NULL, 512, 10, 10);
//           if (tid != RT_NULL)
//               rt_thread_startup(tid);

//        tid = rt_thread_create("pwm_entry", pwm_entry, RT_NULL, 512, 15, 10);
//            if (tid != RT_NULL)
//                rt_thread_startup(tid);
//
//        tid = rt_thread_create("Test_1_entry", Test_1_entry, RT_NULL, 512, 15, 10);
//                  if (tid != RT_NULL)
//                      rt_thread_startup(tid);
//
//        tid = rt_thread_create("Test_2_entry", Test_2_entry, RT_NULL, 512, 15, 10);
//                if (tid != RT_NULL)
//                    rt_thread_startup(tid);

    return RT_EOK;
}
