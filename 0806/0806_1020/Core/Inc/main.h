/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BLDC_H_PF10_Pin GPIO_PIN_10
#define BLDC_H_PF10_GPIO_Port GPIOF
#define ADC1_CH0_TEMP_PA0_Pin GPIO_PIN_0
#define ADC1_CH0_TEMP_PA0_GPIO_Port GPIOA
#define ADC1_CH3_A_W_PA3_Pin GPIO_PIN_3
#define ADC1_CH3_A_W_PA3_GPIO_Port GPIOA
#define ADC1_CH6_A_V_PA6_Pin GPIO_PIN_6
#define ADC1_CH6_A_V_PA6_GPIO_Port GPIOA
#define ADC1_CH8_A_U_PB0_Pin GPIO_PIN_0
#define ADC1_CH8_A_U_PB0_GPIO_Port GPIOB
#define ADC1_CH9_VOLT_PB1_Pin GPIO_PIN_1
#define ADC1_CH9_VOLT_PB1_GPIO_Port GPIOB
#define HALL_INPUT_U_PH10_Pin GPIO_PIN_10
#define HALL_INPUT_U_PH10_GPIO_Port GPIOH
#define HALL_INPUT_V_PH11_Pin GPIO_PIN_11
#define HALL_INPUT_V_PH11_GPIO_Port GPIOH
#define HALL_INPUT_W_PH12_Pin GPIO_PIN_12
#define HALL_INPUT_W_PH12_GPIO_Port GPIOH
#define TIM1_CH1N_UL_PB13_Pin GPIO_PIN_13
#define TIM1_CH1N_UL_PB13_GPIO_Port GPIOB
#define TIM1_CH2N_VL_PB14_Pin GPIO_PIN_14
#define TIM1_CH2N_VL_PB14_GPIO_Port GPIOB
#define TIM1_CH3N_WL_PB15_Pin GPIO_PIN_15
#define TIM1_CH3N_WL_PB15_GPIO_Port GPIOB
#define TIM1_CH1_UH_PA8_Pin GPIO_PIN_8
#define TIM1_CH1_UH_PA8_GPIO_Port GPIOA
#define TIM1_CH2_VH_PA9_Pin GPIO_PIN_9
#define TIM1_CH2_VH_PA9_GPIO_Port GPIOA
#define TIM1_CH3_WH_PA10_Pin GPIO_PIN_10
#define TIM1_CH3_WH_PA10_GPIO_Port GPIOA
#define USART1_TX_PB6_Pin GPIO_PIN_6
#define USART1_TX_PB6_GPIO_Port GPIOB
#define USART1_RX_PB7_Pin GPIO_PIN_7
#define USART1_RX_PB7_GPIO_Port GPIOB
#define LED_RED_PE0_Pin GPIO_PIN_0
#define LED_RED_PE0_GPIO_Port GPIOE
#define LED_GREEN_PE1_Pin GPIO_PIN_1
#define LED_GREEN_PE1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
