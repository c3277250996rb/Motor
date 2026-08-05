#include "my_tim.h"

#if 1   // 三相逆变电路
/************************************ 霍尔接口初始化 ******************************************/

/**
 * @brief  霍尔传感器接口初始化
 * @param  无
 * @retval 无
 */
void hall_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    HALL1_U_GPIO_CLK_ENABLE();
    HALL1_V_GPIO_CLK_ENABLE();
    HALL1_W_GPIO_CLK_ENABLE();

    /* 霍尔通道 1 引脚初始化 */
    gpio_init_struct.Pin = HALL1_TIM_CH1_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(HALL1_TIM_CH1_GPIO, &gpio_init_struct);

    /* 霍尔通道 2 引脚初始化 */
    gpio_init_struct.Pin = HALL1_TIM_CH2_PIN;
    HAL_GPIO_Init(HALL1_TIM_CH2_GPIO, &gpio_init_struct);

    /* 霍尔通道 3 引脚初始化 */
    gpio_init_struct.Pin = HALL1_TIM_CH3_PIN;
    HAL_GPIO_Init(HALL1_TIM_CH3_GPIO, &gpio_init_struct);

}

/**
 * @brief       获取霍尔传感器引脚状态
 * @param       motor_id ： 电机接口号
 * @retval      霍尔传感器引脚状态
 */
uint32_t hallsensor_get_state(uint8_t motor_id)
{
    __IO static uint32_t state ;
    state  = 0;
    if(motor_id == MOTOR_1)
    {
        if(HAL_GPIO_ReadPin(HALL1_TIM_CH1_GPIO,HALL1_TIM_CH1_PIN) != GPIO_PIN_RESET)  /* 霍尔传感器状态获取 */
        {
            state |= 0x01U;
        }
        if(HAL_GPIO_ReadPin(HALL1_TIM_CH2_GPIO,HALL1_TIM_CH2_PIN) != GPIO_PIN_RESET)  /* 霍尔传感器状态获取 */
        {
            state |= 0x02U;
        }
        if(HAL_GPIO_ReadPin(HALL1_TIM_CH3_GPIO,HALL1_TIM_CH3_PIN) != GPIO_PIN_RESET)  /* 霍尔传感器状态获取 */
        {
            state |= 0x04U;
        }
    }
    return state;
}

/************************************* BLDC相关函数 *************************************/

/**
 * @brief  关闭电机运转
 * @param  无
 * @retval 无
 */
void stop_motor1(void)
{
    /* 关闭半桥芯片输出 */
    SHUTDOWN_OFF;
    /* 关闭PWM输出 */
    HAL_TIM_PWM_Stop(&g_atimx_handle,TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&g_atimx_handle,TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&g_atimx_handle,TIM_CHANNEL_3);
    /* 上下桥臂全部关断 */
    g_atimx_handle.Instance->CCR2 = 0;
    g_atimx_handle.Instance->CCR1 = 0;
    g_atimx_handle.Instance->CCR3 = 0;
    HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}

/**
 * @brief  开启电机运转
 * @param  无
 * @retval 无
 */
void start_motor1(void)
{
    SHUTDOWN_EN;
    /* 使能PWM输出 */
    HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_3);
}

/*************************** 上下桥臂的导通情况，共6种，也称为6步换向（接口一） ****************************/

/*  六步换向函数指针数组 */
pctr pfunclist_m1[6] =
{
    &m1_uhwl, &m1_vhul, &m1_vhwl,
    &m1_whvl, &m1_uhvl, &m1_whul
};

/**
 * @brief  U相上桥臂导通，V相下桥臂导通
 * @param  无
 * @retval 无
 */
void m1_uhvl(void)
{
    g_atimx_handle.Instance->CCR1 = g_bldc_motor1.pwm_duty;                 /* U相上桥臂PWM */
    g_atimx_handle.Instance->CCR2 = 0;
    g_atimx_handle.Instance->CCR3 = 0;
    HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_SET);   /* V相下桥臂导通 */
    HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET); /* U相下桥臂关闭 */
    HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET); /* W相下桥臂关闭 */
}

/**
 * @brief  U相上桥臂导通，W相下桥臂导通
 * @param  无
 * @retval 无
 */
void m1_uhwl(void)
{
    g_atimx_handle.Instance->CCR1 = g_bldc_motor1.pwm_duty;
    g_atimx_handle.Instance->CCR2 = 0;
    g_atimx_handle.Instance->CCR3 = 0;
    HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
}

/**
 * @brief  V相上桥臂导通，W相下桥臂导通
 * @param  无
 * @retval 无
 */
void m1_vhwl(void)
{
    g_atimx_handle.Instance->CCR1=0;
    g_atimx_handle.Instance->CCR2 = g_bldc_motor1.pwm_duty;
    g_atimx_handle.Instance->CCR3=0;
    HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
}

/**
 * @brief  V相上桥臂导通，U相下桥臂导通
 * @param  无
 * @retval 无
 */
void m1_vhul(void)
{
    g_atimx_handle.Instance->CCR1 = 0;
    g_atimx_handle.Instance->CCR2 = g_bldc_motor1.pwm_duty;
    g_atimx_handle.Instance->CCR3 = 0;
    HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}

/**
 * @brief  W相上桥臂导通，U相下桥臂导通
 * @param  无
 * @retval 无
 */
void m1_whul(void)
{
    g_atimx_handle.Instance->CCR1 = 0;
    g_atimx_handle.Instance->CCR2 = 0;
    g_atimx_handle.Instance->CCR3 = g_bldc_motor1.pwm_duty;

    HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}

/**
 * @brief  W相上桥臂导通，V相下桥臂导通
 * @param  无
 * @retval 无
 */
void m1_whvl(void)
{
    g_atimx_handle.Instance->CCR1 = 0;
    g_atimx_handle.Instance->CCR2 = 0;
    g_atimx_handle.Instance->CCR3 = g_bldc_motor1.pwm_duty;

    HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}


/******************************************************************************************/
/* 定时器配置句柄 定义 */

/* 高级定时器PWM */
TIM_HandleTypeDef g_atimx_handle;           /* 定时器x句柄 */
TIM_OC_InitTypeDef g_atimx_oc_chy_handle;   /* 定时器输出句柄 */
extern _bldc_obj g_bldc_motor1;
/******************************************************************************************/

/**
 * @brief       高级定时器TIMX PWM输出初始化函数
 * @note
 *              高级定时器的时钟来自APB2, 而PCLK2 = 168Mhz, 我们设置PPRE2不分频, 因此
 *              高级定时器时钟 = 168Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_oc_chy_init(uint16_t arr, uint16_t psc)
{
    ATIM_TIMX_PWM_CHY_CLK_ENABLE();                             /* TIMX 时钟使能 */


    g_atimx_handle.Instance = ATIM_TIMX_PWM;                    /* 定时器x */
    g_atimx_handle.Init.Prescaler = psc;                        /* 定时器分频 */
    g_atimx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;       /* 向上计数模式 */
    g_atimx_handle.Init.Period = arr;                           /* 自动重装载值 */
    g_atimx_handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   /* 分频因子 */
    g_atimx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /*使能TIMx_ARR进行缓冲*/
    g_atimx_handle.Init.RepetitionCounter = 0;                  /* 开始时不计数*/
    HAL_TIM_PWM_Init(&g_atimx_handle);                          /* 初始化PWM */

    g_atimx_oc_chy_handle.OCMode = TIM_OCMODE_PWM1;             /* 模式选择PWM1 */
    g_atimx_oc_chy_handle.Pulse = 0;
    g_atimx_oc_chy_handle.OCPolarity = TIM_OCPOLARITY_HIGH;     /* 输出比较极性为高 */
    g_atimx_oc_chy_handle.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    g_atimx_oc_chy_handle.OCFastMode = TIM_OCFAST_DISABLE;
    g_atimx_oc_chy_handle.OCIdleState = TIM_OCIDLESTATE_RESET;
    g_atimx_oc_chy_handle.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&g_atimx_handle, &g_atimx_oc_chy_handle, ATIM_TIMX_PWM_CH1); /* 配置TIMx通道y */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_handle, &g_atimx_oc_chy_handle, ATIM_TIMX_PWM_CH2); /* 配置TIMx通道y */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_handle, &g_atimx_oc_chy_handle, ATIM_TIMX_PWM_CH3); /* 配置TIMx通道y */

    /* 开启定时器通道1输出PWM */
    HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_1);

    /* 开启定时器通道2输出PWM */
    HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_2);

    /* 开启定时器通道3输出PWM */
    HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_3);
}

/**
 * @brief       定时器底层驱动，时钟使能，引脚配置
                此函数会被HAL_TIM_PWM_Init()调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIM_TIMX_PWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        ATIM_TIMX_PWM_CHY_CLK_ENABLE();                             /* 定时器时钟使能 */
        
        /* 上桥臂的IO时钟使能 */
        ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE();                      
        ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE();                   
        ATIM_TIMX_PWM_CH3_GPIO_CLK_ENABLE();                       
        
        /* 下桥臂的IO时钟使能 */
        M1_LOW_SIDE_U_GPIO_CLK_ENABLE();                         
        M1_LOW_SIDE_V_GPIO_CLK_ENABLE();                    
        M1_LOW_SIDE_W_GPIO_CLK_ENABLE();                            

        /* 下桥臂的IO初始化 */
        gpio_init_struct.Pin = M1_LOW_SIDE_U_PIN;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_HIGH;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                /* 推挽输出模式 */
        HAL_GPIO_Init(M1_LOW_SIDE_U_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = M1_LOW_SIDE_V_PIN;
        HAL_GPIO_Init(M1_LOW_SIDE_V_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = M1_LOW_SIDE_W_PIN;
        HAL_GPIO_Init(M1_LOW_SIDE_W_PORT, &gpio_init_struct);


        /* 上桥臂即定时器IO初始化 */
        gpio_init_struct.Pin = ATIM_TIMX_PWM_CH1_GPIO_PIN;          /* 通道y的GPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_NOPULL;                        /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
        gpio_init_struct.Alternate = ATIM_TIMX_PWM_CHY_GPIO_AF;     /* 端口复用 */
        HAL_GPIO_Init(ATIM_TIMX_PWM_CH1_GPIO_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = ATIM_TIMX_PWM_CH2_GPIO_PIN;          /* 通道y的CPIO口 */
        HAL_GPIO_Init(ATIM_TIMX_PWM_CH2_GPIO_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = ATIM_TIMX_PWM_CH3_GPIO_PIN;          /* 通道y的CPIO口 */
        HAL_GPIO_Init(ATIM_TIMX_PWM_CH3_GPIO_PORT, &gpio_init_struct);
        
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2, 2);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    }
}

/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_PWM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_atimx_handle);
}

/**
 * @brief       定时器中断回调
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == ATIM_TIMX_PWM)                                     /* 55us */
    {
#ifdef H_PWM_L_ON
        if(g_bldc_motor1.run_flag == RUN)
        {
            if(g_bldc_motor1.dir == CW)                                     /* 正转 */
            {
                g_bldc_motor1.step_sta = hallsensor_get_state(MOTOR_1);     /* 顺序6,2,3,1,5,4 */
            }
            else                                                            /* 反转 */
            {
                g_bldc_motor1.step_sta = 7 - hallsensor_get_state(MOTOR_1); /* 顺序5,1,3,2,6,4 。使用7减完后可与数组pfunclist_m1对应上顺序 实际霍尔值为：2,6,4,5,1,3*/
            }
            
            if((g_bldc_motor1.step_sta <= 6)&&(g_bldc_motor1.step_sta >= 1))/* 判断霍尔组合值是否正常 */
            {
                pfunclist_m1[g_bldc_motor1.step_sta-1]();                   /* 通过数组成员查找对应的函数指针 */
                
            }
            else                                                            /* 霍尔传感器错误、接触不良、断开等情况 */
            {
                stop_motor1();
                g_bldc_motor1.run_flag = STOP;
            }
        }

#endif
    }
}



#endif


