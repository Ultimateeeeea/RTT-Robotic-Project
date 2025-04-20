/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-14     RealThread   first version
 */

#ifdef Error_Handler
#  undef Error_Handler
#endif
#include "tim.h"      // CubeMX 生成的，定义了 htim5, htim8, MX_TIM5_Init() 等
#include "usart.h"
#include "gpio.h"

#include <rtthread.h>
#include <board.h>
#include <drv_common.h>


RT_WEAK void rt_hw_board_init()
{
    extern void hw_board_init(char *clock_src, int32_t clock_src_freq, int32_t clock_target_freq);

    /* Heap initialization */
#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *) HEAP_BEGIN, (void *) HEAP_END);
#endif

    hw_board_init(BSP_CLOCK_SOURCE, BSP_CLOCK_SOURCE_FREQ_MHZ, BSP_CLOCK_SYSTEM_FREQ_MHZ);

    /* Set the shell console output device */
#if defined(RT_USING_DEVICE) && defined(RT_USING_CONSOLE)
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* Board underlying hardware initialization */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    /* 基本初始化 */
        HAL_Init();
        MX_GPIO_Init();
        MX_USART1_UART_Init();

    /* TIM5/TIM8 PWM 初始化 */
       MX_TIM5_Init();
       HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
       HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

       MX_TIM8_Init();
       HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
       HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);

     /* TIM1 编码器初始化*/
       MX_TIM1_Init();

}
