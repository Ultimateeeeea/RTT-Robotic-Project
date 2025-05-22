///*
//* 超市机器人挑战赛 - 货架抓取任务实现
//* 实现二层货架物品抓取和放回功能
//*/
//
//#include <rtthread.h>
//#include <rtdevice.h>
//#include <board.h>
//#include "drv_common.h"
//#include "math.h"
//#include "motor/Inc/motor_state.h"
//#include "motor/Inc/motor_cmd.h"
//#include "arm/Inc/LobotServoController.h"
//#include "gyro/Inc/sensor_port.h"
//#include "ultrasonic_ver2.0/Inc/ultrasonic.h"
//#include "competition/competition_control.h"
//
///* 任务状态定义 */
//typedef enum {
// SHELF_TASK_INIT = 0,           /* 初始化状态 */
// SHELF_ADJUST_POSITION,          /* 位置调整阶段 */
// SHELF_SCAN_ITEMS,               /* 扫描物品阶段 */
// SHELF_GRAB_ITEM,                /* 抓取物品阶段 */
// SHELF_PLACE_ITEM,               /* 放置物品阶段 */
// SHELF_LEAVE_AREA,               /* 离开区域阶段 */
// SHELF_TASK_COMPLETE              /* 任务完成状态 */
//} shelf_task_state_t;
//
///* 全局变量 */
//static shelf_task_state_t shelf_state = SHELF_TASK_INIT;
//static rt_tick_t task_start_time = 0;
//static rt_bool_t item_detected = RT_FALSE;
//static float item_x_offset = 0.0f;  /* 物品相对于中心的X轴偏移 */
//static rt_uint8_t items_grabbed = 0; /* 已抓取物品数量 */
//static rt_uint8_t current_target_shelf_idx = 1; // 1 代表货架1, 2 代表货架2
//static rt_bool_t initial_scan_setup_done = RT_FALSE;
//#define TOTAL_ITEMS_TO_GRAB 4
//#define MAX_SHELVES 2 // 总共处理两个货架
//
//// 用于边缘检测的确认计数器和阈值
//static rt_uint8_t adjust_s4_edge_confirm_count = 0;
//#define ADJUST_S4_EDGE_CONFIRM_THRESHOLD 3
//
//static rt_uint8_t scan_front_jump_confirm_count = 0;
//#define SCAN_FRONT_JUMP_CONFIRM_THRESHOLD 3
//
//static rt_uint8_t leave_l4_edge_confirm_count = 0;
//#define LEAVE_L4_EDGE_CONFIRM_THRESHOLD 3
//
//
///* 获取当前货架任务状态 */
//shelf_task_state_t get_shelf_task_state(void)
//{
// return shelf_state;
//}
//
///* 设置物品检测状态 */
//void set_item_detected(rt_bool_t detected, float x_offset)
//{
// item_detected = detected;
// item_x_offset = x_offset;
//}
//
///* 舵机ID定义 */
//#define ARM_ROTATE_SERVO_ID    1    /* 旋转台舵机ID */
//#define ARM_EXTEND_SERVO_ID    2    /* 机械臂伸展舵机ID */
//#define ARM_GRAB_SERVO_ID      3    /* 机械臂抓取舵机ID */
//
//
///* 舵机位置定义 */
//#define ARM_EXTENDED_POS      0     /* 后端伸缩：0 → 机械臂伸到货架 */
//#define ARM_RETRACTED_POS   135     /* 后端伸缩：135 → 机械臂完全收回 */
//
//#define GRAB_OPEN_POS       300     /* 爪子张开准备抓物品 */
//#define GRAB_CLOSE_POS      700     /* 爪子闭合抓住物品 */
//
//#define ROTATE_FRONT_POS      0     /* 旋转台朝向车头 */
//#define ROTATE_BACK_POS     750     /* 顺时针 180° 对准货架 */
//
///* 距离阈值定义 */
//#define SHELF_DISTANCE_CM      15.0f  /* 与货架的理想距离(cm) */
//#define DISTANCE_TOLERANCE     2.0f  /* 距离容差(cm) */
//#define ITEM_GRAB_DISTANCE    10.0f  /* 抓取物品的距离(cm) */
//#define POSITION_TOLERANCE     5.0f  /* 位置容差(cm) */
//
///* 电机速度定义 */
//#define NORMAL_SPEED          100    /* 正常移动速度 */
//#define SLOW_SPEED             50    /* 慢速移动速度 */
//#define SCAN_SPEED             30    /* 扫描速度 */
//
///* 升降机构高度定义 */
//#define ELEVATOR_SECOND_LAYER_POS  10000  /* 第二层货架高度位置(脉冲数) - 待测试确定 */
//
///* 函数声明 */
//static void shelf_grab_task_entry(void *parameter);
//static void adjust_position(void);
//static void scan_for_items(void);
//static void grab_item(void);
//static void place_item(void);
//static void leave_area(void);
//static void process_vision_data(void);
//
///* 视觉识别数据处理 - 模拟从上位机接收数据 */
//static void process_vision_data(void)
//{
// /* 实际项目中，这里应该从串口接收上位机发送的视觉识别数据 */
// /* 这里使用模拟数据进行测试 */
// static rt_uint32_t last_check_time = 0;
// rt_uint32_t now = rt_tick_get();
//
// /* 每500ms检查一次 */
// if (now - last_check_time < rt_tick_from_millisecond(500)) {
//     return;
// }
//
// last_check_time = now;
//
// /* 随机模拟是否检测到物品 */
// if (shelf_state == SHELF_SCAN_ITEMS) {
//     rt_int32_t random_val = rt_tick_get() % 10;
//
//     if (random_val < 3) { /* 30%概率检测到物品 */
//         item_detected = RT_TRUE;
//         /* 模拟物品偏移量，范围-10到10 */
//         item_x_offset = (float)((rt_tick_get() % 21) - 10);
//         rt_int32_t ixo_x10 = (rt_int32_t)(item_x_offset * 10.0f);
//         if (ixo_x10 < 0) { rt_kprintf("[视觉模拟] 检测到物品，X偏移: -%d.%d\n", -ixo_x10 / 10, -ixo_x10 % 10); }
//         else { rt_kprintf("[视觉模拟] 检测到物品，X偏移: %d.%d\n", ixo_x10 / 10, ixo_x10 % 10); }
//     } else {
//         item_detected = RT_FALSE;
//     }
// }
//}
//
///**
//* @brief 带确认的条件检查（用于处理传感器噪声）
//*
//* @param condition_met 当前周期条件是否满足
//* @param confirm_counter 指向特定状态的确认计数器的指针
//* @param confirm_threshold 需要连续多少次满足条件才算确认
//* @param reset_on_fail_immediately 如果条件在某周期不满足，是否立即重置计数器为0
//* @return rt_bool_t 如果达到确认阈值则返回 RT_TRUE, 否则返回 RT_FALSE
//*/
//static rt_bool_t check_condition_with_confirmation(
//  rt_bool_t condition_met,
//  rt_uint8_t *confirm_counter,
//  const rt_uint8_t confirm_threshold,
//  rt_bool_t reset_on_fail_immediately)
//{
//  if (condition_met) {
//      (*confirm_counter)++;
//  } else {
//      if (reset_on_fail_immediately) {
//          *confirm_counter = 0;
//      } else {
//          // 如果不立即重置，并且计数器曾经大于0（意味着曾经满足过条件），则也重置
//          // 这种方式要求必须是"连续"满足N次
//          if (*confirm_counter > 0) {
//               *confirm_counter = 0;
//          }
//      }
//  }
//
//  if (*confirm_counter >= confirm_threshold) {
//      *confirm_counter = 0; // 确认后重置计数器，为下一次可能的检测做准备
//      return RT_TRUE;
//  }
//  return RT_FALSE;
//}
//
//
///* 位置调整阶段 */
//static void adjust_position(void)
//{
// static rt_uint8_t adjust_step = 0;
// static rt_tick_t safety_timer = 0;
// static float prev_open_front_dist_adj_s4 = -1.0f;
// static rt_bool_t waiting_for_rotation_completion_s3 = RT_FALSE;
//
// if (adjust_step == 0) {
//     rt_kprintf("[调整任务] 开始位置调整阶段\n");
//     gyro_cmd_zero();
//     rt_thread_mdelay(200); // 等待陀螺仪清零稳定
//     rt_kprintf("[调整任务] 陀螺仪已清零。当前车头方向 (例如全局+Y) 为0度。\n");
//     safety_timer = rt_tick_get();
//     adjust_step = 1;
//     waiting_for_rotation_completion_s3 = RT_FALSE;
//     adjust_s4_edge_confirm_count = 0; // 重置步骤4的确认计数器
//     rt_kprintf("[调整任务] 步骤1: 右平移 (目标: 左超声波14-16cm) 开始\n");
//     return;
// }
//
// switch (adjust_step) {
// case 1: /* 右平移，直到左侧超声波距离达到14-16cm */
//     motor_state_set(ST_RIGHT);
//     float left_us_dist_s1 = us_distance_cm[US_CHANNEL_LEFT];
//     rt_int32_t lus1_x10 = (rt_int32_t)(left_us_dist_s1 * 10.0f);
//
//     if (lus1_x10 < 0) { rt_kprintf("[调整调试] 步骤1: 右平移中。左超声波: -%d.%dcm\n", -lus1_x10 / 10, -lus1_x10 % 10); }
//     else { rt_kprintf("[调整调试] 步骤1: 右平移中。左超声波: %d.%dcm\n", lus1_x10 / 10, lus1_x10 % 10); }
//
//     rt_bool_t left_us_ok_s1 = (left_us_dist_s1 >= 14.0f && left_us_dist_s1 <= 16.0f);
//     rt_bool_t timeout_s1 = (rt_tick_get() - safety_timer > rt_tick_from_millisecond(7000));
//
//     if (left_us_ok_s1 || timeout_s1) {
//         motion_stop();
//         rt_kprintf("[调整任务] 步骤1: 右平移完成。左超声波: %.1fcm. 达到目标: %d, 超时: %d\n", left_us_dist_s1, left_us_ok_s1, timeout_s1);
//         safety_timer = rt_tick_get();
//         adjust_step = 2;
//         rt_kprintf("[调整任务] 步骤2: 调整车尾距离 (目标: 后超声波5-7cm) 开始\n");
//     }
//     break;
//
// case 2: /* 调整车尾与墙壁的距离 (目标后超声波 5-7cm) */
//     {
//         float current_rear_dist_s2 = us_distance_cm[US_CHANNEL_BACK];
//         rt_bool_t rear_dist_ok_s2 = (current_rear_dist_s2 >= 5.0f && current_rear_dist_s2 <= 7.0f);
//         rt_bool_t timeout_s2 = (rt_tick_get() - safety_timer > rt_tick_from_millisecond(5000));
//
//         rt_int32_t crd_s2_x10 = (rt_int32_t)(current_rear_dist_s2 * 10.0f);
//         if (crd_s2_x10 < 0) { rt_kprintf("[调整调试] 步骤2: 调整中。后超声波: -%d.%dcm\n", -crd_s2_x10 / 10, -crd_s2_x10 % 10); }
//         else { rt_kprintf("[调整调试] 步骤2: 调整中。后超声波: %d.%dcm\n", crd_s2_x10 / 10, crd_s2_x10 % 10); }
//
//         if (rear_dist_ok_s2 || timeout_s2) {
//             motion_stop();
//             rt_kprintf("[调整任务] 步骤2: 调整车尾距离完成。后超声波: %.1fcm. 达到目标: %d, 超时: %d\n", current_rear_dist_s2, rear_dist_ok_s2, timeout_s2);
//             safety_timer = rt_tick_get();
//             adjust_step = 3;
//             waiting_for_rotation_completion_s3 = RT_FALSE; // 为此步骤重置
//             rt_kprintf("[调整任务] 步骤3: 顺时针旋转90度开始 (目标航向: -90度或+270度 全局+X)。\n");
//         } else {
//             if (current_rear_dist_s2 > 7.0f) {
//                 motor_state_set(ST_BACKWARD);
//             } else if (current_rear_dist_s2 < 5.0f) {
//                 motor_state_set(ST_FORWARD);
//             } else {
//                 motion_stop();
//             }
//         }
//     }
//     break;
//
// case 3: /* 顺时针旋转90度。目标-90度(全局+X) */
//     if (!waiting_for_rotation_completion_s3) {
//         motor_state_set(ST_PIVOT_RIGHT90);
//         rt_kprintf("[调整调试] 步骤3: 请求旋转90度，等待电机状态变为IDLE...\n");
//         waiting_for_rotation_completion_s3 = RT_TRUE;
//         safety_timer = rt_tick_get(); // 开始旋转安全计时器
//     }
//
//     if (motor_get_current_state() == ST_IDLE && waiting_for_rotation_completion_s3) {
//         float current_yaw_s3 = g_yaw_angle;
//         rt_bool_t angle_ok_neg_s3 = (current_yaw_s3 <= -88.0f && current_yaw_s3 >= -92.0f);
//         rt_bool_t angle_ok_pos_s3 = (current_yaw_s3 >= 268.0f && current_yaw_s3 <= 272.0f);
//
//         if (angle_ok_neg_s3 || angle_ok_pos_s3) {
//              rt_kprintf("[调整任务] 步骤3: 旋转90度确认完成。电机状态IDLE，航向: %.1f°\n", current_yaw_s3);
//              safety_timer = rt_tick_get();
//              adjust_step = 4;
//              prev_open_front_dist_adj_s4 = -1.0f; // 为下一步的检测重置
//              adjust_s4_edge_confirm_count = 0; // 重置步骤4的确认计数器
//              rt_kprintf("[调整任务] 步骤4: 左平移 (目标: 前超声波检测货架边缘) 开始\n");
//         } else {
//              rt_kprintf("[调整调试] 步骤3: 电机IDLE但航向(%.1f)未达标，可能旋转失败或被中断。当前尝试超时等待。\n", current_yaw_s3);
//         }
//     }
//
//     if (waiting_for_rotation_completion_s3 && rt_tick_get() - safety_timer > rt_tick_from_millisecond(8000)) { // 8秒旋转超时
//         motion_stop();
//         motor_state_set(ST_IDLE);
//         rt_kprintf("[调整任务] 步骤3: 旋转90度超时! 当前航向: %.1f°. 强制停止并进入下一步骤。\n", g_yaw_angle);
//         safety_timer = rt_tick_get();
//         adjust_step = 4;
//         prev_open_front_dist_adj_s4 = -1.0f;
//         adjust_s4_edge_confirm_count = 0; // 重置步骤4的确认计数器
//         rt_kprintf("[调整任务] 步骤4: 左平移 (目标: 前超声波检测货架边缘) 开始\n");
//     }
//     break;
//
// case 4: /* 左平移 (车头-90度(全局+X)，向自身左侧(+Y本地)移动，即沿全局+Y移动)，用前超声波检测货架边缘 */
//     motor_state_set(ST_LEFT);
//     float front_dist_s4 = us_distance_cm[US_CHANNEL_FRONT];
//
//     if (prev_open_front_dist_adj_s4 < 0) {
//          // 初始化prev_open_front_dist_adj_s4为一个比货架距离稍大的值，确保初次比较有效
//          prev_open_front_dist_adj_s4 = front_dist_s4 > 100.0f ? front_dist_s4 : 150.0f;
//          rt_kprintf("[调整调试] 步骤4: 初始化 prev_open_front_dist_adj_s4: %.1fcm\n", prev_open_front_dist_adj_s4);
//          adjust_s4_edge_confirm_count = 0; // 确保每次进入步骤4开始时计数器清零
//     }
//
//     rt_int32_t fd_s4_x10 = (rt_int32_t)(front_dist_s4 * 10.0f);
//     if (fd_s4_x10 < 0) { rt_kprintf("[调整调试] 步骤4: 左平移。前超声波: -%d.%dcm (前次开阔距离: %.1f)\n", -fd_s4_x10 / 10, -fd_s4_x10 % 10, prev_open_front_dist_adj_s4); }
//     else { rt_kprintf("[调整调试] 步骤4: 左平移。前超声波: %d.%dcm (前次开阔距离: %.1f)\n", fd_s4_x10 / 10, fd_s4_x10 % 10, prev_open_front_dist_adj_s4); }
//
//     // 初步的边缘检测条件，使用滤波后的超声波数据
//     // 参数需要根据实际滤波效果进行微调
//     rt_bool_t potential_edge_s4 = (prev_open_front_dist_adj_s4 > 75.0f && // 例如，之前是开阔区域
//                                        front_dist_s4 < 55.0f &&          // 现在检测到近距离物体
//                                        (prev_open_front_dist_adj_s4 - front_dist_s4) > 25.0f); // 距离变化足够大
//
//     rt_kprintf("[调整调试] 步骤4: 潜在边缘条件: %d, 当前确认计数: %d\n", potential_edge_s4, adjust_s4_edge_confirm_count);
//
//     rt_bool_t confirmed_edge_s4 = check_condition_with_confirmation(
//                                         potential_edge_s4,
//                                         &adjust_s4_edge_confirm_count,
//                                         ADJUST_S4_EDGE_CONFIRM_THRESHOLD,
//                                         RT_TRUE // 如果一次不满足，则立即重置计数器
//                                     );
//     rt_bool_t timeout_s4 = rt_tick_get() - safety_timer > rt_tick_from_millisecond(10000);
//
//     if (confirmed_edge_s4 || timeout_s4) {
//         motion_stop();
//         rt_kprintf("[调整任务] 步骤4: 左平移完成。检测到货架边缘。前超声波: %.1fcm. 确认边缘: %d, 超时: %d\n", front_dist_s4, confirmed_edge_s4, timeout_s4);
//         shelf_state = SHELF_SCAN_ITEMS;
//         adjust_step = 0;
//         prev_open_front_dist_adj_s4 = -1.0f;
//         // adjust_s4_edge_confirm_count 已在 check_condition_with_confirmation 或下次进入step 0时重置
//         rt_kprintf("[调整任务] 位置调整完成，进入扫描物品阶段。\n");
//     }
//     break;
// }
//}
//
///* 扫描物品阶段 */
//static void scan_for_items(void)
//{
// static rt_tick_t scan_start_time = 0;
// static float initial_front_dist_on_shelf = 0.0f;
//
// if (!initial_scan_setup_done) {
//     rt_kprintf("[扫描任务] 开始扫描物品阶段 (货架%d)\n", current_target_shelf_idx);
//     scan_start_time = rt_tick_get();
//     initial_front_dist_on_shelf = us_distance_cm[US_CHANNEL_FRONT]; // 使用滤波后的数据
//     rt_kprintf("[扫描任务] 初始扫描方向: 向左平移. 当前前方距离(货架): %.1fcm\n", initial_front_dist_on_shelf);
//
//     if (initial_front_dist_on_shelf > 40.0f || initial_front_dist_on_shelf < 5.0f) {
//         initial_front_dist_on_shelf = 15.0f; // 如果初始距离异常，修正为一个合理值
//         rt_kprintf("[扫描任务] 初始前方距离异常，修正为: %.1fcm\n", initial_front_dist_on_shelf);
//     }
//     initial_scan_setup_done = RT_TRUE;
//     scan_front_jump_confirm_count = 0; // 重置扫描阶段的确认计数器
// }
//
// if (items_grabbed >= TOTAL_ITEMS_TO_GRAB) {
//     motion_stop();
//     rt_kprintf("[扫描任务] 已抓取%d个物品，总数已满，任务完成。\n", items_grabbed);
//     shelf_state = SHELF_TASK_COMPLETE;
//     initial_scan_setup_done = RT_FALSE;
//     return;
// }
//
// process_vision_data(); // 处理视觉数据（模拟）
//
// if (item_detected) {
//     motion_stop();
//     rt_kprintf("[扫描任务] (货架%d)检测到物品，X偏移: %.1f, 切换到抓取阶段\n", current_target_shelf_idx, item_x_offset);
//     shelf_state = SHELF_GRAB_ITEM;
//     initial_scan_setup_done = RT_FALSE;
//     return;
// }
//
// float current_front_dist = us_distance_cm[US_CHANNEL_FRONT]; // 使用滤波后的数据
// float current_left_dist = us_distance_cm[US_CHANNEL_LEFT];   // 使用滤波后的数据
//
// // 检测货架末端（前方距离突然变远）的条件
// // 参数需要根据实际滤波效果进行微调
// rt_bool_t potential_front_jump = (current_front_dist > initial_front_dist_on_shelf + 25.0f && // 例如，变化阈值调整
//                                   initial_front_dist_on_shelf < 45.0f); // 确保初始是在货架前
//
// rt_kprintf("[扫描调试] 潜在前方跳变条件: %d, 当前确认计数: %d\n", potential_front_jump, scan_front_jump_confirm_count);
//
// rt_bool_t confirmed_front_dist_jumped = check_condition_with_confirmation(
//                                             potential_front_jump,
//                                             &scan_front_jump_confirm_count,
//                                             SCAN_FRONT_JUMP_CONFIRM_THRESHOLD,
//                                             RT_TRUE // 如果一次不满足，则立即重置计数器
//                                         );
//
// // 其他扫描结束条件
// rt_bool_t left_scan_area_covered = (current_left_dist > 19.0f && current_left_dist < 23.0f && (rt_tick_get() - scan_start_time > rt_tick_from_millisecond(8000)));
// rt_bool_t scan_timeout = (rt_tick_get() - scan_start_time > rt_tick_from_millisecond(15000));
// rt_bool_t total_task_timeout = (rt_tick_get() - task_start_time > rt_tick_from_millisecond(180000)); // 总任务超时，例如3分钟
//
// if (total_task_timeout) {
//     motion_stop();
//     rt_kprintf("[扫描任务] 总任务时间超时，判定任务完成\n");
//     shelf_state = SHELF_TASK_COMPLETE;
//     initial_scan_setup_done = RT_FALSE;
//     return;
// }
//
// if (confirmed_front_dist_jumped || left_scan_area_covered || scan_timeout) {
//     motion_stop();
//     initial_scan_setup_done = RT_FALSE;
//     if (confirmed_front_dist_jumped) rt_kprintf("[扫描任务] (货架%d)左扫描到达末端(前超声波确认突变)，结束扫描此货架\n", current_target_shelf_idx);
//     if (left_scan_area_covered) rt_kprintf("[扫描任务] (货架%d)左扫描到达预定左侧范围且时间较长，结束扫描此货架\n", current_target_shelf_idx);
//     if (scan_timeout) rt_kprintf("[扫描任务] (货架%d)向左扫描超时，结束扫描此货架\n", current_target_shelf_idx);
//
//     // 决定下一步: 是否去下一个货架或结束任务
//     if (items_grabbed >= TOTAL_ITEMS_TO_GRAB) {
//         rt_kprintf("[扫描任务] (货架%d)扫描结束，已抓取%d个物品，总数已满，准备离开货架区域。\n", current_target_shelf_idx, items_grabbed);
//         shelf_state = SHELF_LEAVE_AREA;
//     } else if (current_target_shelf_idx < MAX_SHELVES) {
//         rt_kprintf("[扫描任务] (货架%d)扫描结束，物品未满(%d/%d)，准备前往下一个货架(%d)\n", current_target_shelf_idx, items_grabbed, TOTAL_ITEMS_TO_GRAB, current_target_shelf_idx + 1);
//         shelf_state = SHELF_LEAVE_AREA;
//     } else {
//         rt_kprintf("[扫描任务] (货架%d)扫描结束，已是最后一个货架，但物品(%d/%d)未满，准备离开货架区域。\n", current_target_shelf_idx, items_grabbed, TOTAL_ITEMS_TO_GRAB);
//         shelf_state = SHELF_LEAVE_AREA;
//     }
//     return;
// }
//
// motor_state_set(ST_LEFT); // 继续向左平移扫描
// rt_kprintf("[扫描任务] (货架%d)向左扫描中... 前方: %.1fcm (基准:%.1f), 左侧: %.1fcm, 跳变确认计数: %d\n",
//            current_target_shelf_idx, current_front_dist, initial_front_dist_on_shelf, current_left_dist, scan_front_jump_confirm_count);
//}
//
///* 抓取物品阶段 */
//static void grab_item(void)
//{
// static rt_uint8_t grab_step = 0;
// static rt_tick_t step_start_time = 0;
//
// if (grab_step == 0) {
//     rt_kprintf("[抓取任务] 开始抓取物品阶段\n");
//     step_start_time = rt_tick_get();
//     grab_step = 1;
// }
//
// switch (grab_step) {
// case 1: /* 调整位置，使物品在中心 */
//     if (item_x_offset < -POSITION_TOLERANCE) {
//         motor_state_set(ST_LEFT);
//     } else if (item_x_offset > POSITION_TOLERANCE) {
//         motor_state_set(ST_RIGHT);
//     } else {
//         motion_stop();
//         step_start_time = rt_tick_get();
//         grab_step = 2;
//         rt_kprintf("[抓取任务] 物品居中，准备操作机械臂\n");
//     }
//     break;
//
// case 2: /* 打开爪子 */
//     moveServo(ARM_GRAB_SERVO_ID, GRAB_OPEN_POS, 500); // (舵机ID, 位置, 时间ms)
//     step_start_time = rt_tick_get();
//     grab_step = 3;
//     break;
//
// case 3: /* 等待爪子打开 */
//     if (rt_tick_get() - step_start_time > rt_tick_from_millisecond(600)) { // 增加少许等待时间
//         step_start_time = rt_tick_get();
//         grab_step = 4;
//     }
//     break;
//
// case 4: /* 伸展机械臂 */
//     moveServo(ARM_EXTEND_SERVO_ID, ARM_EXTENDED_POS, 1000);
//     step_start_time = rt_tick_get();
//     grab_step = 5;
//     break;
//
// case 5: /* 等待机械臂伸展 */
//     if (rt_tick_get() - step_start_time > rt_tick_from_millisecond(1100)) { // 增加少许等待时间
//         step_start_time = rt_tick_get();
//         grab_step = 6;
//     }
//     break;
//
// case 6: /* 闭合爪子抓取物品 */
//     moveServo(ARM_GRAB_SERVO_ID, GRAB_CLOSE_POS, 500);
//     step_start_time = rt_tick_get();
//     grab_step = 7;
//     break;
//
// case 7: /* 等待爪子闭合 */
//     if (rt_tick_get() - step_start_time > rt_tick_from_millisecond(600)) { // 增加少许等待时间
//         step_start_time = rt_tick_get();
//         grab_step = 8;
//     }
//     break;
//
// case 8: /* 收回机械臂 */
//     moveServo(ARM_EXTEND_SERVO_ID, ARM_RETRACTED_POS, 1000);
//     step_start_time = rt_tick_get();
//     grab_step = 9;
//     break;
//
// case 9: /* 等待机械臂收回 */
//     if (rt_tick_get() - step_start_time > rt_tick_from_millisecond(1100)) { // 增加少许等待时间
//         items_grabbed++;
//         rt_kprintf("[抓取任务] 物品抓取成功，已抓取 %d 个物品\n", items_grabbed);
//         shelf_state = SHELF_PLACE_ITEM;
//         grab_step = 0;
//     }
//     break;
// }
//}
//
///* 放置物品阶段 */
//static void place_item(void)
//{
// static rt_uint8_t place_step = 0;
// static rt_tick_t step_start_time = 0;
//
// if (place_step == 0) {
//     rt_kprintf("[放置任务] 开始放置物品阶段\n");
//     step_start_time = rt_tick_get();
//     place_step = 1;
// }
//
// switch (place_step) {
// case 1: /* 旋转机械臂到后方 */
//     moveServo(ARM_ROTATE_SERVO_ID, ROTATE_BACK_POS, 1500);
//     step_start_time = rt_tick_get();
//     place_step = 2;
//     break;
// case 2: /* 等待旋转完成 */
//     if (rt_tick_get() - step_start_time > rt_tick_from_millisecond(1600)) { // 增加少许等待时间
//         step_start_time = rt_tick_get();
//         place_step = 3;
//     }
//     break;
// case 3: /* 打开爪子，释放物品 */
//     moveServo(ARM_GRAB_SERVO_ID, GRAB_OPEN_POS, 500);
//     step_start_time = rt_tick_get();
//     place_step = 4;
//     break;
// case 4: /* 等待爪子打开 */
//     if (rt_tick_get() - step_start_time > rt_tick_from_millisecond(600)) { // 增加少许等待时间
//         step_start_time = rt_tick_get();
//         place_step = 5;
//     }
//     break;
// case 5: /* 机械臂旋转回前方 */
//     moveServo(ARM_ROTATE_SERVO_ID, ROTATE_FRONT_POS, 1500);
//     step_start_time = rt_tick_get();
//     place_step = 6;
//     break;
// case 6: /* 等待旋转完成 */
//     if (rt_tick_get() - step_start_time > rt_tick_from_millisecond(1600)) { // 增加少许等待时间
//         rt_kprintf("[放置任务] 物品放置成功 (当前已抓取 %d / %d).\n", items_grabbed, TOTAL_ITEMS_TO_GRAB);
//         place_step = 0;
//         if (items_grabbed >= TOTAL_ITEMS_TO_GRAB) {
//             rt_kprintf("[放置任务] 已抓取%d个物品，总数已满，准备离开当前区域后结束任务!\n", items_grabbed);
//             shelf_state = SHELF_LEAVE_AREA;
//         } else {
//             rt_kprintf("[放置任务] 物品总数未满，返回扫描当前货架 (货架%d).\n", current_target_shelf_idx);
//             shelf_state = SHELF_SCAN_ITEMS;
//             initial_scan_setup_done = RT_FALSE;
//         }
//     }
//     break;
// }
//}
//
///* 离开区域阶段 */
//static void leave_area(void)
//{
// static rt_uint8_t leave_step = 0;
// static rt_tick_t safety_timer = 0;
// static float prev_open_front_dist_leave_l4 = -1.0f;
// static rt_bool_t waiting_for_rotation_completion_l2 = RT_FALSE;
//
// if (leave_step == 0) {
//     rt_kprintf("[离开任务] 开始离开货架%d区域的完整流程。\n", current_target_shelf_idx);
//     safety_timer = rt_tick_get();
//     leave_step = 1;
//     waiting_for_rotation_completion_l2 = RT_FALSE;
//     leave_l4_edge_confirm_count = 0; // 重置离开阶段L4的确认计数器
//     rt_kprintf("[离开任务] L1: 左平移 (目标: 左超声波20-22cm) 开始\n");
//     return;
// }
//
// switch (leave_step) {
// case 1: /* L1: 左平移，直到左侧超声波距离达到20-22cm */
//     motor_state_set(ST_LEFT);
//     float left_dist_l1 = us_distance_cm[US_CHANNEL_LEFT];
//     rt_kprintf("[离开调试] L1: 左平移中。左超声波: %.1fcm\n", left_dist_l1);
//
//     rt_bool_t left_dist_ok_l1 = (left_dist_l1 >= 20.0f && left_dist_l1 <= 22.0f);
//     rt_bool_t timeout_l1 = (rt_tick_get() - safety_timer > rt_tick_from_millisecond(5000));
//
//     if (left_dist_ok_l1 || timeout_l1) {
//         motion_stop();
//         rt_kprintf("[离开任务] L1: 左平移完成. 左距: %.1fcm. 达到目标:%d, 超时:%d\n", left_dist_l1, left_dist_ok_l1, timeout_l1);
//         safety_timer = rt_tick_get();
//         leave_step = 2;
//         waiting_for_rotation_completion_l2 = RT_FALSE;
//         rt_kprintf("[离开任务] L2: 原地顺时针旋转180度开始\n");
//     }
//     break;
//
// case 2: /* L2: 原地顺时针旋转180度 */
//     if (!waiting_for_rotation_completion_l2) {
//         motor_state_set(ST_PIVOT_RIGHT180);
//         rt_kprintf("[离开调试] L2: 请求旋转180度，等待电机状态变为IDLE...\n");
//         waiting_for_rotation_completion_l2 = RT_TRUE;
//         safety_timer = rt_tick_get();
//     }
//
//     if (motor_get_current_state() == ST_IDLE && waiting_for_rotation_completion_l2) {
//         rt_kprintf("[离开任务] L2: 旋转180度确认完成 (电机状态IDLE). 当前航向: %.1f°\n", g_yaw_angle);
//         safety_timer = rt_tick_get();
//         leave_step = 3;
//         rt_kprintf("[离开任务] L3: 直线倒车 (目标后距5-7cm) 开始\n");
//     }
//
//     if (waiting_for_rotation_completion_l2 && rt_tick_get() - safety_timer > rt_tick_from_millisecond(8000)) { // 8秒旋转超时
//         motion_stop();
//         motor_state_set(ST_IDLE);
//         rt_kprintf("[离开任务] L2: 旋转180度超时! 当前航向: %.1f°. 强制停止并进入下一步骤。\n", g_yaw_angle);
//         safety_timer = rt_tick_get();
//         leave_step = 3;
//         rt_kprintf("[离开任务] L3: 直线倒车 (目标后距5-7cm) 开始\n");
//     }
//     break;
//
// case 3: /* L3: 直线倒车，直到后端超声波5-7cm */
//     motor_state_set(ST_BACKWARD);
//     float rear_dist_l3 = us_distance_cm[US_CHANNEL_BACK];
//     rt_kprintf("[离开调试] L3: 倒车中。后超声波: %.1fcm\n", rear_dist_l3);
//
//     rt_bool_t rear_dist_ok_l3 = (rear_dist_l3 >= 5.0f && rear_dist_l3 <= 7.0f);
//     rt_bool_t timeout_l3 = (rt_tick_get() - safety_timer > rt_tick_from_millisecond(7000));
//
//     if (rear_dist_ok_l3 || timeout_l3) {
//         motion_stop();
//         rt_kprintf("[离开任务] L3: 直线倒车完成. 后距: %.1fcm. 达到目标:%d, 超时:%d\n", rear_dist_l3, rear_dist_ok_l3, timeout_l3);
//         safety_timer = rt_tick_get();
//         prev_open_front_dist_leave_l4 = -1.0f; // 为下一步检测重置
//         leave_l4_edge_confirm_count = 0; // 重置L4的确认计数器
//         rt_kprintf("[离开任务] L4: 左平移 (目标: 前超声波检测到下一个货架边缘) 开始\n");
//     }
//     break;
//
// case 4: /* L4: 左平移，用前超声波检测到下一个货架边缘 */
//     motor_state_set(ST_LEFT);
//     float front_dist_l4 = us_distance_cm[US_CHANNEL_FRONT];
//
//     if (prev_open_front_dist_leave_l4 < 0) {
//          // 初始化prev_open_front_dist_leave_l4为一个比货架距离稍大的值
//          prev_open_front_dist_leave_l4 = front_dist_l4 > 80.0f ? front_dist_l4 : 150.0f;
//          rt_kprintf("[离开调试] L4: 初始化 prev_open_front_dist_leave_l4: %.1fcm\n", prev_open_front_dist_leave_l4);
//          leave_l4_edge_confirm_count = 0; // 确保每次进入L4开始时计数器清零
//     }
//     rt_kprintf("[离开调试] L4: 左平移中。前超声波: %.1fcm (前次开阔: %.1f)\n", front_dist_l4, prev_open_front_dist_leave_l4);
//
//     // 初步的边缘检测条件，使用滤波后的超声波数据
//     // 参数需要根据实际滤波效果进行微调
//     rt_bool_t potential_shelf_edge_l4 = (prev_open_front_dist_leave_l4 > 70.0f && // 例如，之前是开阔区域
//                                           front_dist_l4 < 45.0f &&          // 现在检测到近距离物体 (下一个货架)
//                                           (prev_open_front_dist_leave_l4 - front_dist_l4) > 25.0f); // 距离变化足够大
//
//     rt_kprintf("[离开调试] L4: 潜在边缘条件: %d, 当前确认计数: %d\n", potential_shelf_edge_l4, leave_l4_edge_confirm_count);
//
//     rt_bool_t confirmed_shelf_edge_l4 = check_condition_with_confirmation(
//                                             potential_shelf_edge_l4,
//                                             &leave_l4_edge_confirm_count,
//                                             LEAVE_L4_EDGE_CONFIRM_THRESHOLD,
//                                             RT_TRUE // 如果一次不满足，则立即重置计数器
//                                         );
//     rt_bool_t timeout_l4 = rt_tick_get() - safety_timer > rt_tick_from_millisecond(10000);
//
//     if (confirmed_shelf_edge_l4 || timeout_l4) {
//         motion_stop();
//         rt_kprintf("[离开任务] L4: 左平移完成. 假设已完成到下一个区域的过渡. 前距: %.1fcm. 确认边缘:%d, 超时:%d\n", front_dist_l4, confirmed_shelf_edge_l4, timeout_l4);
//
//         if (items_grabbed >= TOTAL_ITEMS_TO_GRAB) {
//             rt_kprintf("[离开任务] 所有物品 (%d/%d) 已抓取。完成离开动作后，任务结束。\n", items_grabbed, TOTAL_ITEMS_TO_GRAB);
//             shelf_state = SHELF_TASK_COMPLETE;
//         } else {
//             if (current_target_shelf_idx < MAX_SHELVES) {
//                 current_target_shelf_idx++;
//                 rt_kprintf("[离开任务] 成功移动到新区域，准备扫描货架%d。\n", current_target_shelf_idx);
//                 shelf_state = SHELF_SCAN_ITEMS;
//                 initial_scan_setup_done = RT_FALSE;
//             } else {
//                 rt_kprintf("[离开任务] 已离开最后一个货架 (%d)，但物品 (%d/%d) 未满。没有更多货架，任务结束。\n", current_target_shelf_idx, items_grabbed, TOTAL_ITEMS_TO_GRAB);
//                 shelf_state = SHELF_TASK_COMPLETE;
//             }
//         }
//         leave_step = 0;
//         prev_open_front_dist_leave_l4 = -1.0f;
//         // leave_l4_edge_confirm_count 已在 check_condition_with_confirmation 或下次进入step 0时重置
//     }
//     break;
// }
//}
//
///* 货架抓取任务线程入口 */
//static void shelf_grab_task_entry(void *parameter)
//{
// /* 等待比赛控制系统进入运行状态 */
// while (get_competition_state() != COMP_RUNNING) {
//     rt_thread_mdelay(100); // 降低等待时的CPU占用
// }
//
// /* 初始化任务 */
// shelf_state = SHELF_TASK_INIT;
// task_start_time = rt_tick_get();
// items_grabbed = 0;
// current_target_shelf_idx = 1; // 确保每次任务开始时都从第一个货架开始
// initial_scan_setup_done = RT_FALSE;
//
//
// rt_kprintf("[主任务] 开始二层货架抓取任务流程\n");
//
// /* 任务状态机 */
// while (1) {
//     if (get_competition_state() == COMP_RESTART_WAITING || is_in_restart_phase()) {
//         motion_stop();
//         rt_kprintf("[主任务] 检测到重启等待，任务暂停，等待 COMP_RUNNING 状态...\n");
//          // 清理当前任务状态以便重新开始调整
//         shelf_state = SHELF_TASK_INIT;
//         adjust_step = 0;
//         // scan_step = 0; // 如果有scan_step
//         // grab_step = 0;
//         // place_step = 0;
//         // leave_step = 0;
//         initial_scan_setup_done = RT_FALSE;
//         current_target_shelf_idx = 1;
//         items_grabbed = 0;
//         adjust_s4_edge_confirm_count = 0;
//         scan_front_jump_confirm_count = 0;
//         leave_l4_edge_confirm_count = 0;
//
//         while(get_competition_state() == COMP_RESTART_WAITING || is_in_restart_phase()) {
//             rt_thread_mdelay(100);
//         }
//         rt_kprintf("[主任务] 比赛已恢复运行，重新开始货架任务...\n");
//         task_start_time = rt_tick_get(); // 重置任务开始时间
//         // shelf_state 已经是 SHELF_TASK_INIT，将自动进入 ADJUST_POSITION
//     }
//
//     if (get_competition_state() != COMP_RUNNING) {
//          // 如果不是RUNNING也不是RESTART_WAITING (例如变成IDLE或FINISHED)
//         motion_stop();
//         rt_kprintf("[主任务] 比赛非运行状态 (%d)，货架任务结束。\n", get_competition_state());
//         break; // 退出货架任务主循环
//     }
//
//     switch (shelf_state) {
//     case SHELF_TASK_INIT:
//         shelf_state = SHELF_ADJUST_POSITION;
//         // 重置所有子状态机的步骤和确认计数器，确保从干净状态开始
//         // adjust_position内部会在adjust_step=0时重置其内部状态和s4计数器
//         // scan_for_items内部会在initial_scan_setup_done=FALSE时重置其内部状态和scan计数器
//         // leave_area内部会在leave_step=0时重置其内部状态和l4计数器
//         // grab_item和place_item在其step=0时重置自身状态
//         rt_kprintf("[主任务] 状态初始化完成，进入位置调整。\n");
//         break;
//
//     case SHELF_ADJUST_POSITION:
//         adjust_position();
//         break;
//
//     case SHELF_SCAN_ITEMS:
//         scan_for_items();
//         break;
//
//     case SHELF_GRAB_ITEM:
//         grab_item();
//         break;
//
//     case SHELF_PLACE_ITEM:
//         place_item();
//         break;
//
//     case SHELF_LEAVE_AREA:
//         leave_area();
//         break;
//
//     case SHELF_TASK_COMPLETE:
//         rt_kprintf("[主任务] 货架任务已完成。等待比赛状态改变或新指令。\n");
//         // 在这里可以等待比赛结束或进入下一个任务（如果设计有的话）
//         // 当前逻辑下，如果比赛仍在RUNNING，它会卡在这里。最好是让外部控制决定是否重新开始
//         // 或者，如果这是比赛的唯一主要任务，则可以在此等待比赛结束信号
//         while(get_competition_state() == COMP_RUNNING && !is_in_restart_phase()) {
//              rt_thread_mdelay(100);
//         }
//         if (get_competition_state() != COMP_RUNNING) {
//             rt_kprintf("[主任务] 比赛状态不再是RUNNING，任务彻底结束。\n");
//             break; // 退出主循环
//         }
//         // 如果是因为restart phase跳出上面while，外层循环会处理
//         break;
//     }
//
//     rt_thread_mdelay(50); // 任务主状态机轮询周期，可以适当调整
// }
//  rt_kprintf("[主任务] shelf_grab_task_entry 线程退出。\n");
//}
//
///* 初始化货架抓取任务 */
//int shelf_grab_task_init(void)
//{
// rt_thread_t tid = rt_thread_create("shelf_grab",
//                                    shelf_grab_task_entry,
//                                    RT_NULL,
//                                    3072, // 增加线程栈大小以防万一
//                                    11,
//                                    20);
// if (tid) rt_thread_startup(tid);
// else rt_kprintf("创建shelf_grab任务线程失败!\n");
// return 0;
//}
//// INIT_APP_EXPORT(shelf_grab_task_init); // 通常由比赛控制模块在适当时候启动，而不是自启动
