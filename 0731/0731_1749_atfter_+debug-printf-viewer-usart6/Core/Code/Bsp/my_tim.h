#ifndef __MY_TIM_H
#define __MY_TIM_H

#include "stm32f4xx.h"
#include "my_pid.h"
#include "my_motor.h"

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

// void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);   /* 高级定时器 互补输出 初始化函数 */
#endif


/*  BDC编码器测速  zat*/
#if 2
/******************************* 第二部分  电机编码器测速 **************************************/

/* 通用定时器 定义  */
#define GTIM_TIMX_ENCODER_CH1_GPIO_PORT         GPIOC
#define GTIM_TIMX_ENCODER_CH1_GPIO_PIN          GPIO_PIN_6
#define GTIM_TIMX_ENCODER_CH1_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)  /* PC口时钟使能 */

#define GTIM_TIMX_ENCODER_CH2_GPIO_PORT         GPIOC
#define GTIM_TIMX_ENCODER_CH2_GPIO_PIN          GPIO_PIN_7
#define GTIM_TIMX_ENCODER_CH2_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)  /* PC口时钟使能 */

/* TIMX 引脚复用设置
 * 因为PC6/PC7, 默认并不是TIM3的功能脚, 必须开启复用, 才可以用作TIM3的CH1/CH2功能
 */
#define GTIM_TIMX_ENCODERCH1_GPIO_AF            GPIO_AF2_TIM3                                /* 端口复用到TIM3 */
#define GTIM_TIMX_ENCODERCH2_GPIO_AF            GPIO_AF2_TIM3                                /* 端口复用到TIM3 */

#define GTIM_TIMX_ENCODER                       TIM3                                         /* TIM3 */
#define GTIM_TIMX_ENCODER_INT_IRQn              TIM3_IRQn
#define GTIM_TIMX_ENCODER_INT_IRQHandler        TIM3_IRQHandler

#define GTIM_TIMX_ENCODER_CH1                   TIM_CHANNEL_1                                /* 通道1 */
#define GTIM_TIMX_ENCODER_CH1_CLK_ENABLE()      do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)   /* TIM3 时钟使能 */

#define GTIM_TIMX_ENCODER_CH2                   TIM_CHANNEL_2                                /* 通道2 */
#define GTIM_TIMX_ENCODER_CH2_CLK_ENABLE()      do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)   /* TIM3 时钟使能 */


/* 基本定时器 定义 
 * 捕获编码器值，用于计算速度
 */
#define BTIM_TIMX_INT                           TIM6
#define BTIM_TIMX_INT_IRQn                      TIM6_DAC_IRQn
#define BTIM_TIMX_INT_IRQHandler                TIM6_DAC_IRQHandler
#define BTIM_TIMX_INT_CLK_ENABLE()              do{ __HAL_RCC_TIM6_CLK_ENABLE(); }while(0)    /* TIM6 时钟使能 */

/* 编码器参数结构体 */
typedef struct 
{
  int encode_old;                  /* 上一次计数值 */
  int encode_now;                  /* 当前计数值 */
  float speed;                     /* 编码器速度 */
} ENCODE_TypeDef;

extern ENCODE_TypeDef g_encode;    /* 编码器参数变量 */

/******************************************************************************************/

void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);           /* 高级定时器 互补输出初始化 */
void gtim_timx_encoder_chy_init(uint16_t arr, uint16_t psc);        /* 通用定时器 定时中断初始化 */
void btim_timx_int_init(uint16_t arr, uint16_t psc);                /* 基本定时器 定时中断初始化 */
int gtim_get_encode(void);                                          /* 获取编码器总计数值 */

/*************************************    第三部分    编码器测速    ****************************************************/

#define ROTO_RATIO      44  /* 线数*倍频系数，即11*4=44 */
#define REDUCTION_RATIO 30  /* 减速比30:1 */

/* 电机参数结构体 */
typedef struct 
{
  uint8_t state;          /*电机状态*/
  float current;          /*电机电流*/
  float volatage;         /*电机电压*/
  float power;            /*电机功率*/
  float speed;            /*电机实际速度*/
  int32_t motor_pwm;      /*设置比较值大小 */
} Motor_TypeDef;

extern Motor_TypeDef  g_motor_data;  /*电机参数变量*/
extern uint8_t  g_run_flag;                                                /* 电机启停状态标志，0：停止，1：启动 */

/*********************************************************************************************************************/

void speed_computer(int32_t encode_now, uint8_t ms);/* 电机速度计算 */

#endif


#if 9
void my_tim_self_test(void);
#endif

#endif