#include "my_pid.h"
#include "my_motor.h"
#include "my_number.h"

PID_TypeDef  g_location_pid;    /* 位置环PID参数结构体 */
PID_TypeDef  g_speed_pid;           /* 速度环PID参数结构体 */
PID_TypeDef  g_current_pid;     /* 电流环PID参数结构体 */

/**
 * @brief       pid初始化
 * @param       无
 * @retval      无
 */
void pid_init(void)
{
    /* 初始化位置环PID参数 */
    g_location_pid.SetPoint = 0.0;      /* 目标值 */
    g_location_pid.ActualValue = 0.0;   /* 期望输出值 */
    g_location_pid.SumError = 0.0;      /* 积分值*/
    g_location_pid.Error = 0.0;         /* Error[1]*/
    g_location_pid.LastError = 0.0;     /* Error[-1]*/
    g_location_pid.PrevError = 0.0;     /* Error[-2]*/
    g_location_pid.Proportion = L_KP;   /* 比例常数 Proportional Const */
    g_location_pid.Integral = L_KI;     /* 积分常数 Integral Const */
    g_location_pid.Derivative = L_KD;   /* 微分常数 Derivative Const */

    /* 初始化速度环PID参数 */
    g_speed_pid.SetPoint = 0;           /* 目标值 */
    g_speed_pid.ActualValue = 0.0;      /* 期望输出值 */
    g_speed_pid.SumError = 0.0;         /* 积分值 */
    g_speed_pid.Error = 0.0;            /* Error[1] */
    g_speed_pid.LastError = 0.0;        /* Error[-1] */
    g_speed_pid.PrevError = 0.0;        /* Error[-2] */
    g_speed_pid.Proportion = S_KP;      /* 比例常数 Proportional Const */
    g_speed_pid.Integral = S_KI;        /* 积分常数 Integral Const */
    g_speed_pid.Derivative = S_KD;      /* 微分常数 Derivative Const */

    /* 初始化电流环PID参数 */
    g_current_pid.SetPoint = 0.0;       /* 目标值 */
    g_current_pid.ActualValue = 0.0;    /* 期望输出值 */
    g_current_pid.SumError = 0.0;       /* 积分值*/
    g_current_pid.Error = 0.0;          /* Error[1]*/
    g_current_pid.LastError = 0.0;      /* Error[-1]*/
    g_current_pid.PrevError = 0.0;      /* Error[-2]*/
    g_current_pid.Proportion = C_KP;    /* 比例常数 Proportional Const */
    g_current_pid.Integral = C_KI;      /* 积分常数 Integral Const */
    g_current_pid.Derivative = C_KD;    /* 微分常数 Derivative Const */
}

/**
 * @brief       pid闭环控制
 * @param       *PID：PID结构体变量地址
 * @param       Feedback_value：当前实际值
 * @retval      期望输出值
 */
volatile float speed_target = 0;
volatile float speed_actual = 0;
volatile float speed_error = 0;
int32_t increment_pid_ctrl(PID_TypeDef *PID,float Feedback_value)
{
    #if 1
        speed_target = PID->SetPoint;
        speed_actual = Feedback_value;
        speed_error = speed_actual - speed_target;
    #endif

    PID->Error = (float)(PID->SetPoint - Feedback_value);                   /* 计算偏差 */
    
#if  INCR_LOCT_SELECT                                                       /* 增量式PID */
    
    PID->ActualValue += (PID->Proportion * (PID->Error - PID->LastError))                          /* 比例环节 */
                        + (PID->Integral * PID->Error)                                             /* 积分环节 */
                        + (PID->Derivative * (PID->Error - 2 * PID->LastError + PID->PrevError));  /* 微分环节 */
    
    PID->PrevError = PID->LastError;                                        /* 存储偏差，用于下次计算 */
    PID->LastError = PID->Error;
    
#else                                                                       /* 位置式PID */
    
    PID->SumError += PID->Error;
    PID->ActualValue = (PID->Proportion * PID->Error)                       /* 比例环节 */
                    + (PID->Integral * PID->SumError)                    /* 积分环节 */
                    + (PID->Derivative * (PID->Error - PID->LastError)); /* 微分环节 */
    PID->LastError = PID->Error;
    
#endif
    return ((int32_t)(PID->ActualValue));                                   /* 返回计算后输出的数值 */
}

/**
 * @brief       积分限幅
 * @param       *PID：PID结构体变量地址
 * @param       max_limit：最大值
 * @param       min_limit：最小值
 * @retval      无
 */
void integral_limit( PID_TypeDef *PID , float max_limit, float min_limit )
{
    if (PID->SumError >= max_limit)                           /* 超过限幅 */
    {
        PID->SumError = max_limit;                            /* 限制积分 */
    }
    else if (PID->SumError <= min_limit)                      /* 超过限幅 */
    {
        PID->SumError = min_limit;
    }
}


void speed_pid_sample(uint8_t ms){
    static uint8_t time = 0;

    if(time % ms == 0){
        if(1 == g_run_flag){
            /* PID计算，输出比较值（占空比） */
            g_motor_data.motor_pwm = increment_pid_ctrl(&g_speed_pid, g_motor_data.speed);
        }
        time = 0;
    }

    time++;
}
