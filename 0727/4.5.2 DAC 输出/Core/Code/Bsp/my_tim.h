#ifndef __MY_TIM_H
#define	__MY_TIM_H

#include "stm32f4xx.h"

/*  TIM2的PWM输出和TIM8的输入捕获 测量验证PWM的实验  zat*/
/**
 * TIM2 -> PA5 -> PWM
 * TIM8 <- PC6 <- input
 */
#if 1

/* 通用定时器 */
#define GENERAL_TIM           		    	TIM2
#define GENERAL_TIM_CLK_ENABLE()       		__TIM2_CLK_ENABLE()

/* 通用定时器PWM输出 */
/* PWM输出引脚 */
#define GENERAL_OCPWM_PIN            		GPIO_PIN_5              
#define GENERAL_OCPWM_GPIO_PORT      		GPIOA                      
#define GENERAL_OCPWM_GPIO_CLK_ENABLE()		__GPIOA_CLK_ENABLE()
#define GENERAL_OCPWM_AF					GPIO_AF1_TIM2

/* 高级控制定时器 */
#define ADVANCE_TIM           		    	TIM8
#define ADVANCE_TIM_CLK_ENABLE()      		__TIM8_CLK_ENABLE()

/* 捕获/比较中断 */
#define ADVANCE_TIM_IRQn					TIM8_CC_IRQn
#define ADVANCE_TIM_IRQHandler        		TIM8_CC_IRQHandler
/* 高级控制定时器PWM输入捕获 */
/* PWM输入捕获引脚 */
#define ADVANCE_ICPWM_PIN              		GPIO_PIN_6              
#define ADVANCE_ICPWM_GPIO_PORT        		GPIOC                      
#define ADVANCE_ICPWM_GPIO_CLK_ENABLE()  	__GPIOC_CLK_ENABLE()
#define ADVANCE_ICPWM_AF					GPIO_AF3_TIM8
#define ADVANCE_IC1PWM_CHANNEL        		TIM_CHANNEL_1
#define ADVANCE_IC2PWM_CHANNEL        		TIM_CHANNEL_2

extern TIM_HandleTypeDef  TIM_PWMOUTPUT_Handle;
extern TIM_HandleTypeDef  TIM_PWMINPUT_Handle;

void TIMx_Configuration(void);

#endif

/*  TIM1带死区的互补输出PWM & 刹车输入  zat*/
/**
 * TIM1 -> PE9 -> CH1 -> PWM
 * TIM1 -> PE8 -> CH1N -> PWM
 * TIM1 <- PA6 <- BKIN <- input
 */
#if 2

/* 主输出通道引脚 */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT TIM1_PWM_CH1_PE9_GPIO_Port
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN TIM1_PWM_CH1_PE9_Pin
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE()	do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0) /* 时钟使能 */

/* 互补输出通道引脚 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT TIM1_PWM_CH1N_PE8_GPIO_Port
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN TIM1_PWM_CH1N_PE8_Pin
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE()	do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* 时钟使能 */

/* 刹车输入引脚 */
#define ATIM_TIMX_CPLM_BKIN_GPIO_PORT TIM1_BKIN_PA6_GPIO_Port
#define ATIM_TIMX_CPLM_BKIN_GPIO_PIN TIM1_BKIN_PA6_Pin
#define ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE()	do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* 时钟使能 */

/* TIMX 引脚复用设置
* 必须开启复用,才能用作 TIM1 的互补输出及刹车引脚。
*/
#define ATIM_TIMX_CPLM_CHY_GPIO_AF GPIO_AF1_TIM1
#define ATIM_TIMX_CPLM TIM1
#define ATIM_TIMX_CPLM_CHY TIM_CHANNEL_1
#define ATIM_TIMX_CPLM_CHY_CCRY ATIM_TIMX_CPLM->CCR1
#define ATIM_TIMX_CPLM_CLK_ENABLE()	do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0) /* TIM1 时钟使能 */


void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);

void atim_timx_cplm_pwm_set(uint16_t ccr, uint8_t dtg);


#endif



#endif /* __MY_TIM_H */

