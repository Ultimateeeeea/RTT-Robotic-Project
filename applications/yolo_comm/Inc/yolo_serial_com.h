//#ifndef __YOLO_SERIAL_COM_H__
//#define __YOLO_SERIAL_COM_H__
//
//#include <rtthread.h>
//#include <rtdevice.h>
//
///* ====================== 协议/硬件参数 ====================== */
//#define YOLO_UART_NAME     "uart5" // 确保这是YOLO实际连接的UART口
//#define YOLO_UART_BAUD     115200
//#define YOLO_FRAME_HEADER_0  0xAA
//#define YOLO_FRAME_HEADER_1  0x55
//#define YOLO_CMD_ITEM_INFO 0x02   // 命令：物品信息 (cls, dx)
//
//// 用于从 yolo_serial_com.c 传递数据到 shelf_grab_task.c 的结构体
//typedef struct {
//    rt_bool_t detected;    // 是否检测到物品
//    rt_uint8_t item_class; // 物品类别 (如果需要)
//    float x_offset_cm;   // X轴偏移量 (单位: 厘米)
//} yolo_detection_result_t;
//
//// 全局变量，用于在 yolo_comm 和 shelf_grab_task 之间共享检测结果
//// 使用 volatile 确保编译器不会过度优化对它的访问
//extern volatile yolo_detection_result_t g_latest_yolo_result;
//extern volatile rt_bool_t g_new_yolo_data_flag; // 新数据标志
//
//// 初始化YOLO通信模块
//int yolo_uart5_init(void);
//
//// (如果需要从外部触发清空检测结果或标志位)
//// void yolo_clear_detection_result(void);
//
//#endif // __YOLO_SERIAL_COM_H__
