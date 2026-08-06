#ifndef __MY_TIM_H
#define __MY_TIM_H

#include "sys.h"
#include "my_motor.h"

#if 0   // 三相逆变电路

/***************************************** 霍尔传感器接口 *************************************************/

#define HALL1_TIM_CH1_PIN           GPIO_PIN_10     /* U */
#define HALL1_TIM_CH1_GPIO          GPIOH
#define HALL1_U_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)    /* PH口时钟使能 */

#define HALL1_TIM_CH2_PIN           GPIO_PIN_11     /* V */
#define HALL1_TIM_CH2_GPIO          GPIOH
#define HALL1_V_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)    /* PH口时钟使能 */

#define HALL1_TIM_CH3_PIN           GPIO_PIN_12     /* W */
#define HALL1_TIM_CH3_GPIO          GPIOH
#define HALL1_W_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)    /* PH口时钟使能 */

/****************************************** 电机相关系数 **************************************************/

#define MAX_PWM_DUTY    (((10000) - 1)*0.96)        /* 最大占空比限制 */

#define H_PWM_L_ON
#ifndef H_PWM_L_ON
#define H_PWM_L_PWM
#endif

#define CCW                         (1)                 /* 逆时针 */
#define CW                          (2)                 /* 顺时针 */
#define HALL_ERROR                  (0xF0)              /* 霍尔错误标志 */
#define RUN                         (1)                 /* 电机运动标志 */
#define STOP                        (0)                 /* 电机停机标志 */



typedef void(*pctr) (void);

void stop_motor1(void);                                 /* 停机 */
void start_motor1(void);                                /* 启动电机 */

#define quick_stop_motor1()     do{stop_motor1(); g_bldc_motor1.run_flag = STOP; g_bldc_motor1.pwm_duty = 0;}while(0)                                    /* 停机 */
#define quick_start_motor1(compare_value)       do{g_bldc_motor1.run_flag = RUN; start_motor1(); g_bldc_motor1.pwm_duty = compare_value;}while(0)              /* 启动电机 */

/************************************* 函数声明 *****************************************/

void hall_gpio_init(void);                              /* 霍尔接口初始化 */
uint32_t hallsensor_get_state(uint8_t motor_id);        /* 获取霍尔状态 */
/*  六步换相 */
extern pctr pfunclist_m1[6];                            /* 六步换相函数指针数组 */
void m1_uhvl(void);
void m1_uhwl(void);
void m1_vhwl(void);
void m1_vhul(void);
void m1_whul(void);
void m1_whvl(void);


/******************************************************************************************/
/* 高级定时器 定义 */

/* TIM_CHx通道 上桥臂IO定义 */
#define ATIM_TIMX_PWM_CH1_GPIO_PORT            GPIOA
#define ATIM_TIMX_PWM_CH1_GPIO_PIN             GPIO_PIN_8
#define ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */

#define ATIM_TIMX_PWM_CH2_GPIO_PORT            GPIOA
#define ATIM_TIMX_PWM_CH2_GPIO_PIN             GPIO_PIN_9
#define ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */

#define ATIM_TIMX_PWM_CH3_GPIO_PORT            GPIOA
#define ATIM_TIMX_PWM_CH3_GPIO_PIN             GPIO_PIN_10
#define ATIM_TIMX_PWM_CH3_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */

/* 下桥臂IO定义 */
#define M1_LOW_SIDE_U_PORT                      GPIOB
#define M1_LOW_SIDE_U_PIN                       GPIO_PIN_13
#define M1_LOW_SIDE_U_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */

#define M1_LOW_SIDE_V_PORT                      GPIOB
#define M1_LOW_SIDE_V_PIN                       GPIO_PIN_14
#define M1_LOW_SIDE_V_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */

#define M1_LOW_SIDE_W_PORT                      GPIOB
#define M1_LOW_SIDE_W_PIN                       GPIO_PIN_15
#define M1_LOW_SIDE_W_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */

/* 复用到TIM1 */
#define ATIM_TIMX_PWM_CHY_GPIO_AF              GPIO_AF1_TIM1

#define ATIM_TIMX_PWM                          TIM1
#define ATIM_TIMX_PWM_IRQn                     TIM1_UP_TIM10_IRQn
#define ATIM_TIMX_PWM_IRQHandler               TIM1_UP_TIM10_IRQHandler
#define ATIM_TIMX_PWM_CH1                      TIM_CHANNEL_1                               /* 通道1 */
#define ATIM_TIMX_PWM_CH2                      TIM_CHANNEL_2                               /* 通道2 */
#define ATIM_TIMX_PWM_CH3                      TIM_CHANNEL_3                               /* 通道3 */
#define ATIM_TIMX_PWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 时钟使能 */


extern TIM_HandleTypeDef g_atimx_handle;                                                    /* 定时器x句柄 */
/******************************************************************************************/

void atim_timx_oc_chy_init(uint16_t arr, uint16_t psc);                                     /* 高级定时器 PWM初始化函数 */


#endif




#if 2

/**
 ****************************************************************************************************
 * @file        bldc_tim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-19
 * @brief       定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 F407电机开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211019
 * 第一次发布
 *
 ****************************************************************************************************
 */

 #ifndef __BLDC_TIM_H
 #define __BLDC_TIM_H
 
 #include "sys.h"
 
 /****************************************** 高级定时器 定义 ************************************************/
 
 extern TIM_HandleTypeDef g_atimx_handle;                                                       /* 定时器x句柄 */
 
 #define ATIM_TIMX_PWM_CH1_GPIO_PORT            GPIOA
 #define ATIM_TIMX_PWM_CH1_GPIO_PIN             GPIO_PIN_8
 #define ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */
 
 #define ATIM_TIMX_PWM_CH2_GPIO_PORT            GPIOA
 #define ATIM_TIMX_PWM_CH2_GPIO_PIN             GPIO_PIN_9
 #define ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */
 
 #define ATIM_TIMX_PWM_CH3_GPIO_PORT            GPIOA
 #define ATIM_TIMX_PWM_CH3_GPIO_PIN             GPIO_PIN_10
 #define ATIM_TIMX_PWM_CH3_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */
 
 /* 互补通道IO */
 #define M1_LOW_SIDE_U_PORT                      GPIOB
 #define M1_LOW_SIDE_U_PIN                       GPIO_PIN_13
 #define M1_LOW_SIDE_U_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */
 
 #define M1_LOW_SIDE_V_PORT                      GPIOB
 #define M1_LOW_SIDE_V_PIN                       GPIO_PIN_14
 #define M1_LOW_SIDE_V_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */
 
 #define M1_LOW_SIDE_W_PORT                      GPIOB
 #define M1_LOW_SIDE_W_PIN                       GPIO_PIN_15
 #define M1_LOW_SIDE_W_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */
 
 #define ATIM_TIMX_PWM_CHY_GPIO_AF              GPIO_AF1_TIM1
 
 #define ATIM_TIMX_PWM                          TIM1
 #define ATIM_TIMX_PWM_IRQn                     TIM1_UP_TIM10_IRQn
 #define ATIM_TIMX_PWM_IRQHandler               TIM1_UP_TIM10_IRQHandler
 #define ATIM_TIMX_PWM_CH1                      TIM_CHANNEL_1                                    /* 通道1 */
 #define ATIM_TIMX_PWM_CH2                      TIM_CHANNEL_2                                    /* 通道2 */
 #define ATIM_TIMX_PWM_CH3                      TIM_CHANNEL_3                                    /* 通道3 */
 
 #define ATIM_TIMX_PWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)       /* TIM8 时钟使能 */
 
 extern TIM_HandleTypeDef g_atimx_handle;                                                        /* 定时器x句柄 */
 
 /***************************************** 基本定时器 定义 *************************************************/
 
 #define BTIM_TIMX_INT                       TIM6
 #define BTIM_TIMX_INT_IRQn                  TIM6_DAC_IRQn
 #define BTIM_TIMX_INT_IRQHandler            TIM6_DAC_IRQHandler
 #define BTIM_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM6_CLK_ENABLE(); }while(0)          /* TIM6 时钟使能 */
 
 /******************************************* 函数声明 ***********************************************/
 
 void btim_timx_int_init(uint16_t arr, uint16_t psc);                                            /* 基本定时器 定时中断初始化函数 */
 void atim_timx_oc_chy_init(uint16_t arr, uint16_t psc);                                         /* 高级定时器 PWM初始化函数 */
 
 #endif
 
 
 


#endif



#endif
