#ifndef __MY_MOTOR_H
#define __MY_MOTOR_H

#include "stm32f407xx.h"
#include "my_tim.h"
#include "my_number.h"

#define MOTOR_PWM_LIMIT     4200
#define MOTOR_AMPERE_INTEGRAL_LIMIT     1250
#define MOTOR_AMPERE_LIMIT     200
#define MOTOR_DIRECTION_CHECK(x) do{\
    if((x) > 0){dcmotor_dir(0);}\
    else{(x) = -(x); dcmotor_dir(1);}\
}while(0)

/*  BDC基础驱动  zat*/
#if 1
/*************************************    基本驱动    *****************************************************/

/* 停止引脚操作宏定义 
 * 此引脚控制H桥是否生效以达到开启和关闭电机的效果
 */
#define SHUTDOWN1_Pin                 GPIO_PIN_10
#define SHUTDOWN1_GPIO_Port           GPIOF

#define SHUTDOWN2_Pin                 GPIO_PIN_2
#define SHUTDOWN2_GPIO_Port           GPIOF
#define SHUTDOWN_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF口时钟使能 */

/* 电机停止引脚定义 这里默认是接口1 */
#define ENABLE_MOTOR    HAL_GPIO_WritePin(SHUTDOWN1_GPIO_Port,SHUTDOWN1_Pin,GPIO_PIN_SET)
#define DISABLE_MOTOR   HAL_GPIO_WritePin(SHUTDOWN1_GPIO_Port,SHUTDOWN1_Pin,GPIO_PIN_RESET)

/******************************************************************************************/

void dcmotor_init(void);                /* 直流有刷电机初始化 */
void dcmotor_start(void);               /* 开启电机 */
void dcmotor_stop(void);                /* 关闭电机 */  
void dcmotor_dir(uint8_t para);         /* 设置电机方向 */
void dcmotor_speed(uint16_t para);      /* 设置电机速度 */
void motor_pwm_set(float para);         /* 电机控制 */
#endif    


#if 9
void my_motor_self_test(void);
#endif


#endif
