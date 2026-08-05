#include "my_tim.h"
#include "math.h"

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

    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
                                               
    HAL_TIM_Base_Start_IT(&g_atimx_handle);                     /* 启动高级定时器1 */
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
int32_t  temp_pwm1=0.0;
int32_t motor_pwm_s= 0;

#define ADC_AMP_OFFSET_TIMES 50                     /* 停机状态三相电流的ADC采集次数 */
uint16_t adc_amp_offset[3][ADC_AMP_OFFSET_TIMES+1]; /* 停机状态下的ADC数据缓冲区 */
uint8_t adc_amp_offset_p = 0;
int16_t adc_amp[3];

int16_t adc_amp_un[3];                  
float  adc_amp_bus = 0.0f;

volatile uint16_t adc_val_m1[ADC_CH_NUM];           /* ADC数据缓冲区 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint8_t bldc_dir=0;
    uint8_t i;
    static uint8_t times_count=0;           /* 定时器时间记录 */
    int16_t temp_speed=0;                   /* 临时速度存储 */
    if(htim->Instance == ATIM_TIMX_PWM)     /* 55us */
    {
#ifdef H_PWM_L_ON
        if(g_bldc_motor1.run_flag == RUN)
        {
            g_bldc_motor1.count_j++;
            if(g_bldc_motor1.dir == CW)     /* 顺时针旋转 */
            {
                g_bldc_motor1.step_sta = hallsensor_get_state(MOTOR_1);
            }
            else                            /* 逆时针旋转 */
            {
                g_bldc_motor1.step_sta = 7 - hallsensor_get_state(MOTOR_1);
            }
            if((g_bldc_motor1.step_sta <= 6)&&(g_bldc_motor1.step_sta >= 1))
            {
                pfunclist_m1[g_bldc_motor1.step_sta-1]();
            }
            else                            /* 编码器错误、接触不良、断开等情况 */
            {
                stop_motor1();
                g_bldc_motor1.run_flag = STOP;
            }
            g_bldc_motor1.hall_sta_edge = uemf_edge(g_bldc_motor1.hall_single_sta); /* 检测单个霍尔信号的变化 */
            if(g_bldc_motor1.hall_sta_edge == 0)                                    /* 统计单个霍尔信号的高电平时间 */
            {
                /* 计算速度 */
                if(g_bldc_motor1.dir == CW)
                    temp_speed = (SPEED_COEFF/g_bldc_motor1.count_j);
                else
                    temp_speed = -(SPEED_COEFF/g_bldc_motor1.count_j);
                FirstOrderRC_LPF(g_bldc_motor1.speed, temp_speed, 0.2379);          /* 一阶滤波 */
                g_bldc_motor1.no_single = 0;
                g_bldc_motor1.count_j = 0;
            }
            if(g_bldc_motor1.hall_sta_edge == 1)                                    /* 当采集到下降沿时数据清0 */
            {
                g_bldc_motor1.no_single = 0;
                g_bldc_motor1.count_j = 0;
            }
            if(g_bldc_motor1.hall_sta_edge == 2)
            {
                g_bldc_motor1.no_single++;                                          /* 不换相时间累计 超时则判定速度为0 */
                
                if(g_bldc_motor1.no_single > 15000)
                {
                    
                    g_bldc_motor1.no_single = 0;
                    g_bldc_motor1.speed = 0;                                        /* 超时换向 判定为停止 速度为0 */
                }
            }
            if(g_bldc_motor1.step_last != g_bldc_motor1.step_sta)
            {
                g_bldc_motor1.hall_keep_t = 0;
                bldc_dir = check_hall_dir(&g_bldc_motor1);
                if(bldc_dir == CCW)
                {
                    g_bldc_motor1.pos -= 1;
                }
                else if(bldc_dir == CW)
                {
                    g_bldc_motor1.pos += 1;
                }
                g_bldc_motor1.step_last = g_bldc_motor1.step_sta;
            }
            else if(g_bldc_motor1.run_flag == RUN)                                      /* 运行且霍尔保持时 */
            {
                g_bldc_motor1.hall_keep_t++;                                            /* 换向一次所需计数值（时间） 单位1/18k */
            }       
            /* 三相电流采集 */
            for(i = 0; i < 3; i++)
            {
                adc_val_m1[i] = g_adc_val[i+2];
                adc_amp[i] = adc_val_m1[i] - adc_amp_offset[i][ADC_AMP_OFFSET_TIMES];   /* 运动状态ADC值 - 停机状态ADC值 = 实际作用ADC值 */
                if(adc_amp[i] >= 0)                                                     /* 去除反电动势引起的负电流数据 */
                    adc_amp_un[i] = adc_amp[i];
            }
            /* 运算母线电流（母线电流为任意两个有开关动作的相电流之和） */
            if(g_bldc_motor1.step_sta == 0x05)
            {
                adc_amp_bus= (adc_amp_un[0] + adc_amp_un[1])*ADC2CURT;   /* UV */
            }
            else if(g_bldc_motor1.step_sta == 0x01)
            {
                adc_amp_bus= (adc_amp_un[0] + adc_amp_un[2])*ADC2CURT;   /* UW */
            }
            else if(g_bldc_motor1.step_sta == 0x03)
            {
                adc_amp_bus= (adc_amp_un[1] + adc_amp_un[2])*ADC2CURT;   /* VW */
            }
            else if(g_bldc_motor1.step_sta == 0x02)
            {
                adc_amp_bus= (adc_amp_un[0] + adc_amp_un[1])*ADC2CURT;   /* UV */
            }
            else if(g_bldc_motor1.step_sta == 0x06)
            {
                adc_amp_bus= (adc_amp_un[0] + adc_amp_un[2])*ADC2CURT;   /* WU */
            }
            else if(g_bldc_motor1.step_sta == 0x04)
            {
                adc_amp_bus= (adc_amp_un[2] + adc_amp_un[1])*ADC2CURT;   /* WV */
            }         
        }
#endif
    }
    else if(htim->Instance == TIM6)
    {
        /* 计算未开始启动时的基准电压 */
        times_count++;
        if(g_bldc_motor1.run_flag == STOP)
        {
            uint8_t i;
            uint32_t avg[3] = {0,0,0};
            adc_amp_offset[0][adc_amp_offset_p] = g_adc_val[2];     /* 获取电机停机状态下的三相电流 U */
            adc_amp_offset[1][adc_amp_offset_p] = g_adc_val[3];     /* V */
            adc_amp_offset[2][adc_amp_offset_p] = g_adc_val[4];     /* W */
            adc_amp_offset_p ++;
            NUM_CLEAR(adc_amp_offset_p,ADC_AMP_OFFSET_TIMES);       /* 如果溢出，从头开始计数 */
            for(i = 0; i < ADC_AMP_OFFSET_TIMES; i++)
            {
                avg[0] += adc_amp_offset[0][i];                     /* 各相数值累加 */
                avg[1] += adc_amp_offset[1][i];
                avg[2] += adc_amp_offset[2][i];
            }
            for(i = 0; i < 3; i++)
            {
                avg[i] /= ADC_AMP_OFFSET_TIMES;                     /* 取平均 */
                adc_amp_offset[i][ADC_AMP_OFFSET_TIMES] = avg[i];   /* 赋值 */
            }
        }
    }
}



#endif

#if 1

extern TIM_HandleTypeDef g_atimx_handle;  

const uint8_t hall_table_cw[6] = {6,2,3,1,5,4};         /* 顺时针旋转表 */
const uint8_t hall_table_ccw[6] = {5,1,3,2,6,4};        /* 逆时针旋转表 */

const uint8_t hall_cw_table[12] = {0x62,0x23,0x31,0x15,0x54,0x46,0x63,0x21,0x35,0x14,0x56,0x42};
const uint8_t hall_ccw_table[12] = {0x45,0x51,0x13,0x32,0x26,0x64,0x41,0x53,0x12,0x36,0x24,0x65};

/**
 * @brief       方向检测函数
 * @param       obj ： 电机控制句柄
 * @retval      res ： 旋转方向
 */
uint8_t check_hall_dir(_bldc_obj * obj)
{
    uint8_t temp,res = HALL_ERROR;
    if((obj->step_last <= 6)&&(obj->step_sta <= 6))
    {
        temp = ((obj->step_last & 0x0F) << 4)|(obj->step_sta & 0x0F);
        if((temp == hall_ccw_table[0])||(temp == hall_ccw_table[1])||\
                (temp == hall_ccw_table[2])||(temp == hall_ccw_table[3])||\
                (temp == hall_ccw_table[4])||(temp == hall_ccw_table[5]))
        {
            res  = CCW;
        }
        else if((temp == hall_cw_table[0])||(temp == hall_cw_table[1])||\
                (temp == hall_cw_table[2])||(temp == hall_cw_table[3])||\
                (temp == hall_cw_table[4])||(temp == hall_cw_table[5]))
        {
            res  = CW;
        }
    }
    return res;
}


/**
 * @brief       检测输入信号是否发生变化
 * @param       val :输入信号
 * @note        测量速度使用，获取输入信号状态翻转情况，计算速度
 * @retval      0：计算高电平时间，1：计算低电平时间，2：信号未改变
 */
uint8_t uemf_edge(uint8_t val)
{
    /* 主要是检测val信号从0 - 1 在从 1 - 0的过程，即高电平所持续的过程 */
    static uint8_t oldval = 0;
    if(oldval != val)
    {
        oldval = val;
        if(val == 0) return 0;
        else return 1;
    }
    return 2;
}


/******************************************* 基本定时器初始化 **********************************************/
TIM_HandleTypeDef timx_handler;         /* 定时器参数句柄 */

/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    timx_handler.Instance = BTIM_TIMX_INT;                      /* 基本定时器X */
    timx_handler.Init.Prescaler = psc;                          /* 设置预分频器  */
    timx_handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    timx_handler.Init.Period = arr;                             /* 自动装载值 */
    timx_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* 时钟分频因子 */
    HAL_TIM_Base_Init(&timx_handler);
    
    HAL_TIM_Base_Start_IT(&timx_handler);                       /* 使能基本定时器x和及其更新中断：TIM_IT_UPDATE */
    __HAL_TIM_CLEAR_IT(&timx_handler,TIM_IT_UPDATE);            /* 清除更新中断标志位 */
}

/**
 * @brief       定时器底册驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        BTIM_TIMX_INT_CLK_ENABLE();                     /* 使能TIM时钟*/
        HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 1, 3); /* 抢占1，子优先级3，组2 */
        HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);         /* 开启ITM3中断*/
    }
}

/**
 * @brief       基本定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&timx_handler);                  /* 定时器回调函数 */
}





#endif
