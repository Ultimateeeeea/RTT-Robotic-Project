/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TASK_START_Pin GPIO_PIN_13
#define TASK_START_GPIO_Port GPIOC
#define TLY_TX_Pin GPIO_PIN_2
#define TLY_TX_GPIO_Port GPIOA
#define TLY_RX_Pin GPIO_PIN_3
#define TLY_RX_GPIO_Port GPIOA
#define Front_Trig_Pin GPIO_PIN_0
#define Front_Trig_GPIO_Port GPIOG
#define Right_Trig_Pin GPIO_PIN_1
#define Right_Trig_GPIO_Port GPIOG
#define ARM_TX_Pin GPIO_PIN_10
#define ARM_TX_GPIO_Port GPIOB
#define ARM_RX_Pin GPIO_PIN_11
#define ARM_RX_GPIO_Port GPIOB
#define Back_Trig_Pin GPIO_PIN_2
#define Back_Trig_GPIO_Port GPIOG
#define Left_Trig_Pin GPIO_PIN_3
#define Left_Trig_GPIO_Port GPIOG
#define RTT_TX_Pin GPIO_PIN_9
#define RTT_TX_GPIO_Port GPIOA
#define RTT_RX_Pin GPIO_PIN_10
#define RTT_RX_GPIO_Port GPIOA
#define StepMotor_TX_Pin GPIO_PIN_10
#define StepMotor_TX_GPIO_Port GPIOC
#define StepMotor_RX_Pin GPIO_PIN_11
#define StepMotor_RX_GPIO_Port GPIOC
#define Blue_TX_NANO_TX_Pin GPIO_PIN_12
#define Blue_TX_NANO_TX_GPIO_Port GPIOC
#define Blue_RX_NANO_RX_Pin GPIO_PIN_2
#define Blue_RX_NANO_RX_GPIO_Port GPIOD
#define Front_Switch_Pin GPIO_PIN_0
#define Front_Switch_GPIO_Port GPIOE
#define Back_Switch_Pin GPIO_PIN_1
#define Back_Switch_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
