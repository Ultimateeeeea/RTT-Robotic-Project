///*
//* 超市机器人挑战赛 - 货架抓取任务头文件
//*/
//
//#ifndef __SHELF_GRAB_TASK_H__
//#define __SHELF_GRAB_TASK_H__
//
//#include <rtthread.h>
//#include <rtdevice.h>
//#include <board.h>
//
///* 任务状态定义 */
//typedef enum {
//  SHELF_TASK_INIT = 0,           /* 初始化状态 */
//  SHELF_ADJUST_POSITION,          /* 位置调整阶段 */
//  SHELF_SCAN_ITEMS,               /* 扫描物品阶段 */
//  SHELF_GRAB_ITEM,                /* 抓取物品阶段 */
//  SHELF_PLACE_ITEM,               /* 放置物品阶段 */
//  SHELF_LEAVE_AREA,               /* 离开区域阶段 */
//  SHELF_TASK_COMPLETE              /* 任务完成状态 */
//} shelf_task_state_t;
//
///* 函数声明 */
//int shelf_grab_task_init(void);                  /* 初始化货架抓取任务 */
//shelf_task_state_t get_shelf_task_state(void);   /* 获取当前货架任务状态 */
//void set_item_detected(rt_bool_t detected, float x_offset); /* 设置物品检测状态 */
//
//#endif /* __SHELF_GRAB_TASK_H__ */
