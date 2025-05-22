/*******************************************************************************
* ļ: LobotServoController.h
* : ֻƼ
* ڣ20160806
* LSCϵжưοʾ
*******************************************************************************/

#ifndef LOBOTSERVOCONTROLLER_H_
#define LOBOTSERVOCONTROLLER_H_

#ifdef Error_Handler
#  undef Error_Handler
#endif
#include "usart.h"

/* 舵机ID定义 */
#define ARM_ROTATE_SERVO_ID    1    /* 旋转台舵机ID */
#define ARM_EXTEND_SERVO_ID    2    /* 机械臂伸展舵机ID */
#define ARM_GRAB_SERVO_ID      3    /* 机械臂抓取舵机ID */


/* 舵机位置定义 */
#define ARM_EXTENDED_POS      0     /* 后端伸缩：0 → 机械臂伸到货架 */
#define ARM_RETRACTED_POS   135     /* 后端伸缩：135 → 机械臂完全收回 */

#define GRAB_OPEN_POS       300     /* 爪子张开准备抓物品 */
#define GRAB_CLOSE_POS      700     /* 爪子闭合抓住物品 */

#define ROTATE_FRONT_POS      0     /* 旋转台朝向车头 */
#define ROTATE_BACK_POS     750     /* 顺时针 180° 对准货架 */

#include "drv_common.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rtdevice.h>

#define FRAME_HEADER 0x55             //֡ͷ
#define CMD_SERVO_MOVE 0x03           //����ƶ�ָ��
#define CMD_ACTION_GROUP_RUN 0x06     //���ж�����ָ��
#define CMD_ACTION_GROUP_STOP 0x07    //ֹͣ������ָ��
#define CMD_ACTION_GROUP_SPEED 0x0B   //���ö����������ٶ�
#define CMD_GET_BATTERY_VOLTAGE 0x0F  //��ȡ��ص�ѹָ��

extern rt_bool_t isUartRxCompleted;
extern uint8_t LobotRxBuf[16];
extern uint16_t batteryVolt;
extern void receiveHandle(void);
extern volatile rt_device_t gyro_uart;


typedef struct _lobot_servo_ {  //ID,Ŀλ
	uint8_t ID;
	uint16_t Position;
} LobotServo;


void moveServo(uint8_t servoID, uint16_t Position, uint16_t Time);
void moveServosByArray(LobotServo servos[], uint8_t Num, uint16_t Time);
void moveServos(uint8_t Num, uint16_t Time, ...);
void runActionGroup(uint8_t numOfAction, uint16_t Times);
void stopActionGroup(void);
void setActionGroupSpeed(uint8_t numOfAction, uint16_t Speed);
void setAllActionGroupSpeed(uint16_t Speed);
void getBatteryVoltage(void);



#endif
