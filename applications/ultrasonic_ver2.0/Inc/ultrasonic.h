//// ultrasonic.h
//
//#ifndef __ULTRASONIC_H__
//#define __ULTRASONIC_H__
//
//#ifdef Error_Handler
//#  undef Error_Handler
//#endif
//
//#include "tim.h"
//#include "gpio.h"
//
//#include <stdint.h>
//#include <stdbool.h>
//#include <rtthread.h>
//#include <rtdevice.h>
//#include <board.h>
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#define US_CHANNEL_FRONT   0
//#define US_CHANNEL_RIGHT   1
//#define US_CHANNEL_BACK    2
//#define US_CHANNEL_LEFT    3
//#define US_CHANNEL_NUM     4
//
///* Trigger pins (order: front, right, back, left) */
//#define PIN_TRIG_FRONT   GET_PIN(G,0)
//#define PIN_TRIG_RIGHT   GET_PIN(G,1)
//#define PIN_TRIG_BACK    GET_PIN(G,2)
//#define PIN_TRIG_LEFT    GET_PIN(G,3)
//
///* Echo pins (order: front, right, back, left) */
//#define PIN_ECHO_FRONT   GET_PIN(G,4)
//#define PIN_ECHO_RIGHT   GET_PIN(G,5)
//#define PIN_ECHO_BACK    GET_PIN(G,6)
//#define PIN_ECHO_LEFT    GET_PIN(G,7)
//
///* Measured distances in cm: [0]=front, [1]=right, [2]=back, [3]=left */
//extern volatile float us_distance_cm[US_CHANNEL_NUM];
//
///* Initialize 4-channel ultrasonic measurement */
//int ultrasonic_init(void);
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif /* __ULTRASONIC_H__ */
