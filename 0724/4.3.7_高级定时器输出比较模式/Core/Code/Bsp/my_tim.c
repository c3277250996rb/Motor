#include "my_tim.h"
#include "my_led.h"


/*  TIM2的PWM输出和TIM8的输入捕获 测量验证PWM的实验  zat*/
#if 1
TIM_HandleTypeDef  TIM_PWMOUTPUT_Handle;
TIM_HandleTypeDef  TIM_PWMINPUT_Handle;
__IO uint16_t IC2Value = 0;
__IO uint16_t IC1Value = 0;
__IO float DutyCycle = 0;
__IO float Frequency = 0;
/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启定时器相关的GPIO外设时钟*/
    GENERAL_OCPWM_GPIO_CLK_ENABLE();
    ADVANCE_ICPWM_GPIO_CLK_ENABLE(); 

/*  TIM2  PWM  PA5  zat*/
    /* 定时器功能引脚初始化 */
    /* 通用定时器PWM输出引脚 */	
    GPIO_InitStructure.Pin = GENERAL_OCPWM_PIN;	
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; 	
    GPIO_InitStructure.Alternate = GENERAL_OCPWM_AF;
    HAL_GPIO_Init(GENERAL_OCPWM_GPIO_PORT, &GPIO_InitStructure);
/*  -------------------  */
    
/*  TIM8  INPUT  PC6  zat*/
    /* 高级定时器输入捕获引脚 */
    GPIO_InitStructure.Pin = ADVANCE_ICPWM_PIN;	
    GPIO_InitStructure.Alternate = ADVANCE_ICPWM_AF;	
    HAL_GPIO_Init(ADVANCE_ICPWM_GPIO_PORT, &GPIO_InitStructure);
/*  -----------------  */
}

  /**
  * @brief  高级控制定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
/*  中断优先级  zat*/
    //设置抢占优先级，子优先级
    HAL_NVIC_SetPriority(ADVANCE_TIM_IRQn, 0, 3);
    // 设置中断来源
    HAL_NVIC_EnableIRQ(ADVANCE_TIM_IRQn);
}

/*
  * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
  * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
  * 另外三个成员是通用定时器和高级定时器才有.
  *-----------------------------------------------------------------------------
  * TIM_Prescaler         都有
  * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
  * TIM_Period            都有
  * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
  * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
  *-----------------------------------------------------------------------------
  */
static void TIM_PWMOUTPUT_Config(void)
{	
/*  TIM2  PWM  config  zat*/
    TIM_OC_InitTypeDef TIM_OCInitStructure;
    // 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
    GENERAL_TIM_CLK_ENABLE(); 
    /* 定义定时器的句柄即确定定时器寄存器的基地址*/
    TIM_PWMOUTPUT_Handle.Instance = GENERAL_TIM;
    /* 累计 TIM_Period个后产生一个更新或者中断*/		
    //当定时器从0计数到9999，即为10000次，为一个定时周期
    TIM_PWMOUTPUT_Handle.Init.Period = 50000-1;
/*  这里也不能写printf语句  zat*/
    TIM_PWMOUTPUT_Handle.Init.Prescaler = 84-1;	
    // 采样时钟分频
    TIM_PWMOUTPUT_Handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    // 计数方式
    TIM_PWMOUTPUT_Handle.Init.CounterMode=TIM_COUNTERMODE_UP;
    // 重复计数器
    TIM_PWMOUTPUT_Handle.Init.RepetitionCounter=0;	
    // 初始化定时器TIMx, x[1,8]
    HAL_TIM_PWM_Init(&TIM_PWMOUTPUT_Handle);

    /*PWM模式配置*/
    //配置为PWM模式1
    TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
/*  这里的Pulse不能用Period来赋值, 详见"f407_2026-07-24_ChatGPT_TIM_OCInitStructure.Pulse比较值设置异常.md"  zat*/
    TIM_OCInitStructure.Pulse = 25000; // 比较值 & 有效电平持续时间
    TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
    TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
    TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    //初始化通道1输出PWM 
    HAL_TIM_PWM_ConfigChannel(&TIM_PWMOUTPUT_Handle,&TIM_OCInitStructure,TIM_CHANNEL_1);

    /* 定时器通道1输出PWM */
    HAL_TIM_PWM_Start(&TIM_PWMOUTPUT_Handle,TIM_CHANNEL_1);
/*  ------------------------  */
}
        
static void TIM_PWMINPUT_Config(void)
{	
/*  TIM8  INPUT  config  zat*/
    TIM_IC_InitTypeDef  	TIM_ICInitStructure;
    TIM_SlaveConfigTypeDef  TIM_SlaveConfigStructure;
    // 开启TIMx_CLK,x[1,8] 
    ADVANCE_TIM_CLK_ENABLE(); 
    /* 定义定时器的句柄即确定定时器寄存器的基地址*/
    TIM_PWMINPUT_Handle.Instance = ADVANCE_TIM;
    TIM_PWMINPUT_Handle.Init.Period = 0xFFFF; 	
    
    TIM_PWMINPUT_Handle.Init.Prescaler = 168-1;	
    // 采样时钟分频
    TIM_PWMINPUT_Handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    // 计数方式
    TIM_PWMINPUT_Handle.Init.CounterMode=TIM_COUNTERMODE_UP;	
    // 初始化定时器TIMx, x[1,8]
    HAL_TIM_IC_Init(&TIM_PWMINPUT_Handle);
    
    /* IC1捕获：上升沿触发 TI1FP1 */
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_RISING;
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.ICFilter = 0x0;
    HAL_TIM_IC_ConfigChannel(&TIM_PWMINPUT_Handle,&TIM_ICInitStructure,ADVANCE_IC1PWM_CHANNEL);

    /* IC2捕获：下降沿触发 TI1FP2 */	
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_FALLING;
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.ICFilter = 0x0;
    
    HAL_TIM_IC_ConfigChannel(&TIM_PWMINPUT_Handle,&TIM_ICInitStructure,ADVANCE_IC2PWM_CHANNEL);

    /* 选择从模式: 复位模式 */
    TIM_SlaveConfigStructure.SlaveMode = TIM_SLAVEMODE_RESET;
    /* 选择定时器输入触发: TI1FP1 */
    TIM_SlaveConfigStructure.InputTrigger = TIM_TS_TI1FP1;
    HAL_TIM_SlaveConfigSynchronization(&TIM_PWMINPUT_Handle,&TIM_SlaveConfigStructure);
    
    /* 使能捕获/比较2中断请求 */
    HAL_TIM_IC_Start_IT(&TIM_PWMINPUT_Handle,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&TIM_PWMINPUT_Handle,TIM_CHANNEL_2);
/* --------------------- */
}
/**
  * @brief  初始化高级控制定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{
    TIMx_GPIO_Config();
    
    TIMx_NVIC_Configuration();	
  
    TIM_PWMOUTPUT_Config();
    
    TIM_PWMINPUT_Config();
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  htim : hadc handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
/*  TIM8  INPUT  calback  zat*/
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
      /* 获取输入捕获值 */
      IC1Value = HAL_TIM_ReadCapturedValue(&TIM_PWMINPUT_Handle,ADVANCE_IC1PWM_CHANNEL);
      IC2Value = HAL_TIM_ReadCapturedValue(&TIM_PWMINPUT_Handle,ADVANCE_IC2PWM_CHANNEL);	
      if (IC1Value != 0)
      {
/*  计算占空比原理详见"f407_2026-07-23_DeepSeek-TIM2模式选择解读.md"  zat*/
        /* 占空比计算 */
        DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);

        /* 频率计算 */
        Frequency = 168*1000*1000 / 168 / (float)(IC1Value+1);
        
      }
      else
      {
        DutyCycle = 0;
        Frequency = 0;
      }

  }
/*  -----------------------  */
}


void ADVANCE_TIM_IRQHandler(void)
{
/*  TIM8  INPUT  IRQHandler  zat*/
  HAL_TIM_IRQHandler(&TIM_PWMINPUT_Handle);
}

#endif


/*  TIM1带死区的互补输出PWM & 刹车输入  zat*/
#if 0

TIM_HandleTypeDef g_timx_cplm_pwm_handle;                                /* 定时器x句柄 */
TIM_BreakDeadTimeConfigTypeDef g_sbreak_dead_time_config = {0};          /* 死区时间设置 */

/**
* @brief 高级定时器 TIMX 互补输出 初始化函数（使用 PWM 模式 1）
* @note
* 配置高级定时器 TIMX 互补输出, 一路 OCy 一路 OCyN, 并且可以设置死区时间
*
* 定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=定时器工作频率, 单位 : Mhz
*
* @param arr: 自动重装值。
* @param psc: 预分频系数
* @retval 无
*/
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
  	GPIO_InitTypeDef gpio_init_struct = {0};
    TIM_OC_InitTypeDef tim_oc_cplm_pwm = {0};
    ATIM_TIMX_CPLM_CLK_ENABLE(); /* 开启定时器 X 时钟 */
    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE(); /* 开启主输出通道的 GPIO 时钟 */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE(); /* 开启互补通道的 GPIO 时钟 */
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE(); /* 开启刹车输入的 GPIO 时钟 */

    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHY_GPIO_PIN; /* 主输出通道的 IO */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP; /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHY_GPIO_AF; /* IO 复用 */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHY_GPIO_PORT, &gpio_init_struct);
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHYN_GPIO_PIN; /* 互补通道的 IO */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, &gpio_init_struct);
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_BKIN_GPIO_PIN; /* 刹车输入的 IO */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, &gpio_init_struct);

    /* 设置定时器 */
    g_timx_cplm_pwm_handle.Instance = ATIM_TIMX_CPLM; /* 定时器 x */
    g_timx_cplm_pwm_handle.Init.Prescaler = psc; /* 预分频系数 */
    g_timx_cplm_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP; /* 递增计数*/
    g_timx_cplm_pwm_handle.Init.Period = arr; /* 自动重装载值 */
    /* CKD[1:0] = 10, tDTS = 4 * tCK_INT = Ft / 4*/
    g_timx_cplm_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    g_timx_cplm_pwm_handle.Init.AutoReloadPreload =
    TIM_AUTORELOAD_PRELOAD_ENABLE; /* 使能影子寄存器 TIMx_ARR */
    HAL_TIM_PWM_Init(&g_timx_cplm_pwm_handle) ;

    /* 设置 PWM 输出 */
    tim_oc_cplm_pwm.OCMode = TIM_OCMODE_PWM1; /* PWM 模式 1 */
    tim_oc_cplm_pwm.OCPolarity = TIM_OCPOLARITY_LOW; /* OCy 低电平有效 */
    tim_oc_cplm_pwm.OCNPolarity = TIM_OCNPOLARITY_LOW; /* OCyN 低电平有效 */
    HAL_TIM_PWM_ConfigChannel(&g_timx_cplm_pwm_handle, &tim_oc_cplm_pwm,
    ATIM_TIMX_CPLM_CHY);

    /* 设置死区参数，开启死区中断 */
    g_sbreak_dead_time_config.LockLevel = TIM_LOCKLEVEL_OFF;/* 不用寄存器锁功能*/
    g_sbreak_dead_time_config.BreakState = TIM_BREAK_ENABLE;/* 使能刹车输入 */

    /* 刹车输入有效信号极性为低 */
    g_sbreak_dead_time_config.BreakPolarity = TIM_BREAKPOLARITY_LOW;
    /* 使能 AOE 位，允许刹车结束后自动恢复输出 */
    g_sbreak_dead_time_config.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle,
    &g_sbreak_dead_time_config);

    /* OCy 输出使能 */
    HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY);
    /* OCyN 输出使能 */
    HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY);
}

/**
* @brief 定时器 TIMX 设置输出比较值 & 死区时间
* @param ccr: 输出比较值
* @param dtg: 死区时间
* @arg dtg[7:5]=0xx 时, 死区时间 = dtg[7:0] * tDTS
* @arg dtg[7:5]=10x 时, 死区时间 = (64 + dtg[6:0]) * 2 * tDTS
* @arg dtg[7:5]=110 时, 死区时间 = (32 + dtg[5:0]) * 8 * tDTS
* @arg dtg[7:5]=111 时, 死区时间 = (32 + dtg[5:0]) * 16 * tDTS
* @note tDTS = 1 / (Ft / CKD[1:0]) = 1 / 42M = 23.8ns
* @retval 无
*/
void atim_timx_cplm_pwm_set(uint16_t ccr, uint8_t dtg)
{
    g_sbreak_dead_time_config.DeadTime = dtg; /* 死区时间设置 */
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle,    &g_sbreak_dead_time_config); /* 重设死区时间 */
    __HAL_TIM_MOE_ENABLE(&g_timx_cplm_pwm_handle); /* MOE=1,使能主输出 */
    ATIM_TIMX_CPLM_CHY_CCRY = ccr; /* 设置比较寄存器 */
}

#endif


/*  TIM1(ch1,ch2,ch3)3通道输出比较模式 & 翻转作用 & 多相位  zat*/
#if 3

TIM_HandleTypeDef g_timx_comp_pwm_handle;       /* 定时器x句柄 */

/**
 * @brief       高级定时器TIMX 输出比较模式 初始化函数（使用输出比较模式）
 * @note
 *              配置高级定时器TIMX 3路输出比较模式PWM输出,实现50%占空比,不同相位控制
 *              注意,本例程输出比较模式,每2个计数周期才能完成一个PWM输出,因此输出频率减半
 *              另外,我们还可以开启中断在中断里面修改CCRx,从而实现不同频率/不同相位的控制
 *              但是我们不推荐这么使用,因为这可能导致非常频繁的中断,从而占用大量CPU资源
 *
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    g_timx_comp_pwm_handle.Instance = ATIM_TIMX_COMP;                               /* 定时器x */
    g_timx_comp_pwm_handle.Init.Prescaler = psc  ;                                  /* 定时器分频 */
    g_timx_comp_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                   /* 向上计数模式 */
    g_timx_comp_pwm_handle.Init.Period = arr;                                       /* 自动重装载值 */
    g_timx_comp_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;  /* 使能影子寄存器TIMx_ARR */
    HAL_TIM_OC_Init(&g_timx_comp_pwm_handle);                                       /* 输出比较模式初始化 */

    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;                                           /* 比较输出模式 */
    sConfigOC.Pulse = 250 - 1;                                                      /* 设置输出比较寄存器的值 */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;                                     /* 输出比较极性为高 */
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC,  TIM_CHANNEL_1);  /* 初始化定时器的输出比较通道1 */
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_1);            /* 通道1预装载使能 */

    sConfigOC.Pulse = 500 - 1;
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_2);   /* 初始化定时器的输出比较通道2 */
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_2);            /* 通道2预装载使能 */

    sConfigOC.Pulse = 750 - 1;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC,  TIM_CHANNEL_3);  /* 初始化定时器的输出比较通道3 */
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);            /* 通道3预装载使能 */

    HAL_TIM_OC_Start(&g_timx_comp_pwm_handle,TIM_CHANNEL_1);                        /* 开启通道x输出 */
    HAL_TIM_OC_Start(&g_timx_comp_pwm_handle,TIM_CHANNEL_2);
    HAL_TIM_OC_Start(&g_timx_comp_pwm_handle,TIM_CHANNEL_3);

}

/**
 * @brief       定时器底层驱动，时钟使能，引脚配置
                此函数会被HAL_TIM_OC_Init()调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIM_TIMX_COMP)
    {
        GPIO_InitTypeDef gpio_init_struct;
        
        ATIM_TIMX_COMP_CLK_ENABLE();                                                /* 使能定时器x时钟 */

        ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE();                                       /* 使能通道1的IO时钟 */
        ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE();                                       /* 使能通道2的IO时钟 */
        ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE();                                       /* 使能通道3的IO时钟 */

        gpio_init_struct.Pin = ATIM_TIMX_COMP_CH1_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                                    /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_NOPULL;                                        /* 不上下拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                              /* 高速 */
        gpio_init_struct.Alternate = ATIM_TIMX_COMP_GPIO_AF;                        /* 端口复用 */
        HAL_GPIO_Init(ATIM_TIMX_COMP_CH1_GPIO_PORT, &gpio_init_struct);             /* 初始化通道1的IO */

        gpio_init_struct.Pin = ATIM_TIMX_COMP_CH2_GPIO_PIN;
        HAL_GPIO_Init(ATIM_TIMX_COMP_CH2_GPIO_PORT, &gpio_init_struct);             /* 初始化通道2的IO */

        gpio_init_struct.Pin = ATIM_TIMX_COMP_CH3_GPIO_PIN;
        HAL_GPIO_Init(ATIM_TIMX_COMP_CH3_GPIO_PORT, &gpio_init_struct);             /* 初始化通道3的IO */
    }
}

#endif


/*  my_pwm_self_test  zat*/
#if 9
uint8_t time = 0;
int direction = 1;
uint16_t compare_value = 0;
#include "my_number.h"
void my_pwm_self_test(){

    while(1){
        if(time > 50){
            time = 0;
            int delay = ((int)compare_value - (int)IC2Value+1) / direction;
            my_led_self_test(); /* 控制 LED0 闪烁, 提示程序运行状态 */
            printf("IC1Value = %d  IC2Value = %d compare_value = %d 延时 = %dus  平均延时 = %dus  ",IC1Value,IC2Value+1, compare_value, delay, moving_avarage(delay));
            printf("占空比：%0.2f%%   频率：%0.2fHz\r\n",DutyCycle,Frequency);	
        }

        // compare_value += direction * 1;

        if(compare_value > 500 - 1){
            direction = -1; //超过比较值时递减
        }
        if(compare_value == 0){
            direction = 1;  //减到0时递增
        }
        
        /* 修改比较值控制占空比 */
        // __HAL_TIM_SET_COMPARE(&TIM_PWMOUTPUT_Handle, TIM_CHANNEL_1, compare_value);
        // __HAL_TIM_SET_COMPARE(&g_timx_cplm_pwm_handle, TIM_CHANNEL_1, compare_value);

        time++;
        
        delay_ms(2);
    }

}
#endif
