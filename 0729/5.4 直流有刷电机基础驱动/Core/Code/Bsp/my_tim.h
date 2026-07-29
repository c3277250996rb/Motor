#ifndef __MY_TIM_H
#define __MY_TIM_H

#include "stm32f4xx.h"

/*  BDC基础驱动  zat*/
#if 1
/******************************* 电机基本驱动  互补输出带死区控制 **************************************/

/* TIMX 互补输出模式 定义 */

/* 主输出通道引脚 */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT            GPIOA
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN             GPIO_PIN_8
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

/* 互补输出通道引脚 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT           GPIOB
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN            GPIO_PIN_13
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

/* TIMX 引脚复用设置
 * 因为PA8/PB13, 默认并不是TIM1的功能脚, 必须开启复用,PA8/PB13才能用作TIM1的功能
 */
#define ATIM_TIMX_CPLM_CHY_GPIO_AF              GPIO_AF1_TIM1

/* 互补输出使用的定时器 */
#define ATIM_TIMX_CPLM                          TIM1
#define ATIM_TIMX_CPLM_CHY                      TIM_CHANNEL_1
#define ATIM_TIMX_CPLM_CLK_ENABLE()             do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)    /* TIM1 时钟使能 */

/******************************************************************************************/

void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);   /* 高级定时器 互补输出 初始化函数 */
#endif









#if 9
void my_tim_self_test(void);
#endif

#endif