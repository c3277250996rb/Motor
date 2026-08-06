#ifndef __MY_MOTOR_H
#define __MY_MOTOR_H

#include "sys.h"
#include "my_number.h"
#include "my_adc.h"

#if 1

/***************************************** 电机状态结构体 ***********************************************/
typedef struct {
    __IO uint8_t    run_flag;       /* 运行标志 */
    __IO uint8_t    locked_rotor;   /* 堵转标记 */
    __IO uint8_t    step_sta;       /* 本次霍尔状态 */
    __IO uint8_t    hall_single_sta;/* 单个霍尔状态 */
    __IO uint8_t    hall_sta_edge;  /* 单个霍尔状态跳变 */
    __IO uint8_t    step_last;      /* 上次霍尔状态 */
    __IO uint8_t    dir;            /* 电机旋转方向 */
    __IO int32_t    pos;            /* 电机位置 */
    __IO int32_t    speed;          /* 电机速度 */
    __IO int16_t    current;        /* 电机速度 */
    __IO uint16_t   pwm_duty;       /* 电机占空比 */
    __IO uint32_t   hall_keep_t;    /* 霍尔保持时间 */
    __IO uint32_t   hall_pul_num;   /* 霍尔传感器脉冲数 */
    __IO uint32_t   lock_time;      /* 电机堵转时间 */
    __IO uint32_t   no_single;
    __IO uint32_t   count_j;
    __IO uint64_t   sum_pos;
} _bldc_obj;

#include "my_tim.h"
/******************************************************************************************/
#define MOTOR_1                     1
extern _bldc_obj g_bldc_motor1;
/***************************************** 半桥芯片的刹车引脚 ***************************************************/

#define SHUTDOWN_PIN                      GPIO_PIN_10     /* PF10 */
#define SHUTDOWN_PIN_GPIO                 GPIOF
#define SHUTDOWN_PIN_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)    /* PF口时钟使能 */

#define SHUTDOWN_EN                       HAL_GPIO_WritePin(SHUTDOWN_PIN_GPIO,SHUTDOWN_PIN,GPIO_PIN_SET);   /* 使能半桥芯片的SD引脚 */
#define SHUTDOWN_OFF                      HAL_GPIO_WritePin(SHUTDOWN_PIN_GPIO,SHUTDOWN_PIN,GPIO_PIN_RESET); /* 失能半桥芯片的SD引脚 */


void bldc_init(uint16_t arr, uint16_t psc);             /* BLDC初始化 */
void bldc_ctrl(uint8_t motor_id,int32_t dir,float duty);/* bldc控制函数 */

#endif


#if 9
void my_motor_self_test(void);
#endif

#endif
