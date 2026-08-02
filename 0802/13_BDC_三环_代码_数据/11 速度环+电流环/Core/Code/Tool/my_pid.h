#ifndef __PID_H
#define __PID_H

#include "sys.h"

/******************************************************************************************/
/* PID相关参数 */

#define  INCR_LOCT_SELECT  0         /* 0：位置式 ，1：增量式 */

#if INCR_LOCT_SELECT

/* 定义速度环（外环）PID参数相关宏 */
#define  S_KP      1.500f            /* P参数 */
#define  S_KI      0.023f            /* I参数 */
#define  S_KD      0.010f            /* D参数 */

/* 定义电流环（内环）PID参数相关宏 */
#define  C_KP      1.00f             /* P参数 */
#define  C_KI      3.00f             /* I参数 */
#define  C_KD      0.00f             /* D参数 */
#define  SMAPLSE_PID_SPEED  50       /* 采样周期 单位ms */

#else

/*定义速度环（外环）PID参数相关宏*/
#define  S_KP      1.500f            /* P参数 */
#define  S_KI      0.023f            /* I参数 */
#define  S_KD      0.002f            /* D参数 */

/* 定义电流环（内环）PID参数相关宏 */
#define  C_KP      1.00f             /* P参数 */
#define  C_KI      3.75f             /* I参数 */
#define  C_KD      0.00f             /* D参数 */
#define  SMAPLSE_PID_SPEED  50       /* 采样周期 单位ms */

#endif

/* PID参数结构体 */
typedef struct
{
    __IO float  SetPoint;            /* 设定目标 */
    __IO float  ActualValue;         /* 期望输出值 */
    __IO float  SumError;            /* 误差累计 */
    __IO float  Proportion;          /* 比例常数 P */
    __IO float  Integral;            /* 积分常数 I */
    __IO float  Derivative;          /* 微分常数 D */
    __IO float  Error;               /* Error[1] */
    __IO float  LastError;           /* Error[-1] */
    __IO float  PrevError;           /* Error[-2] */
} PID_TypeDef;

extern PID_TypeDef  g_speed_pid;     /*速度环PID参数结构体*/
extern PID_TypeDef  g_current_pid;   /* 电流环PID参数结构体 */

/******************************************************************************************/

void pid_init(void);                 /* pid初始化 */
int32_t increment_pid_ctrl(PID_TypeDef *PID,float Feedback_value);      /* pid闭环控制 */
void integral_limit( PID_TypeDef *PID , float max_limit, float min_limit );     /* 积分限幅 */

void speed_pid_sample(uint8_t ms);

#endif
