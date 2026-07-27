#include "my_dac.h"
#include "my_adc.h"

/*  DAC普通输出  zat*/
#if 1

DAC_HandleTypeDef g_dac_handle;         /* DAC句柄 */

/**
 * @brief       DAC初始化函数
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void dac_init(uint8_t outx)
{
    GPIO_InitTypeDef gpio_init_struct;
    DAC_ChannelConfTypeDef dac_ch_conf;
    
    __HAL_RCC_DAC_CLK_ENABLE();                                     /* 使能DAC1的时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                                   /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    
    gpio_init_struct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;      /* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG; 
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    g_dac_handle.Instance = DAC;
    HAL_DAC_Init(&g_dac_handle);                                    /* 初始化DAC */

    dac_ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;                     /* 不使用触发功能 */
    dac_ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;        /* DAC1输出缓冲关闭 */
    
    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&g_dac_handle,&dac_ch_conf,DAC_CHANNEL_1);    /* DAC通道1配置 */
            HAL_DAC_Start(&g_dac_handle,DAC_CHANNEL_1);                         /* 开启DAC通道1 */
            break;
        case 2:
            HAL_DAC_ConfigChannel(&g_dac_handle,&dac_ch_conf,DAC_CHANNEL_2);    /* DAC通道2配置 */
            HAL_DAC_Start(&g_dac_handle,DAC_CHANNEL_2);                         /* 开启DAC通道2 */
            break;
        default:break;
    }

}

/**
 * @brief       设置通道1/2输出电压
 * @param       outx: 1,通道1; 2,通道2
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;                                               /* 如果值大于等于4096, 则取4095 */

    if (outx == 1)      /* 通道1 */
    {
        HAL_DAC_SetValue(&g_dac_handle,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);     /* 12位右对齐数据格式设置DAC值 */
    }
    else                /* 通道2 */
    {
        HAL_DAC_SetValue(&g_dac_handle,DAC_CHANNEL_2,DAC_ALIGN_12B_R,temp);     /* 12位右对齐数据格式设置DAC值 */
    }
}

#endif

/*  DAC输出三角波函数  zat*/
#if 2

/**
 * @brief       设置DAC_OUT1输出三角波
 *   @note      输出频率 ≈ 1000 / (dt * samples) Khz, 不过在dt较小的时候,比如小于5us时, 由于delay_us
 *              本身就不准了(调用函数,计算等都需要时间,延时很小的时候,这些时间会影响到延时), 频率会偏小.
 * 
 * @param       max_value : 最大值(1 <= max_value <= 4095), (max_value + 1)必须大于等于samples/2
 * @param       dt     : 每个采样点的延时时间(单位: us)
 * @param       samples: 采样点的个数, samples必须小于等于(max_value + 1) * 2 , 且max_value不能等于0
 * @param       n      : 输出波形个数,0~65535
 *
 * @retval      无
 */
void dac_triangular_wave(uint16_t max_value, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float step;                           /* 递增量 */
    float temp;                           /* 当前值 */
    
    if(samples > ((max_value + 1) * 2)){
        return ;                                        /* 数据不合法 */
    }

    step = (max_value + 1) / (samples / 2);                                          /* 计算递增量 */
    
    for(j = 0; j < n; j++)
    { 
        temp = 0;
        HAL_DAC_SetValue(&g_dac_handle,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);       /* 先输出0 */
        for(i = 0; i < (samples / 2); i++)                                          /* 输出上升沿 */
        {
            temp  +=  step;                                                     /* 新的输出值 */
            // dac_set_voltage(1,temp); /* 异常地三角波顶部呈现梯形 */
            HAL_DAC_SetValue(&g_dac_handle,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);
            my_adc_self_test(); /* 使用VOFA+查看三角波 */
            delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)                                          /* 输出下降沿 */
        {
            temp  -=  step;                                                     /* 新的输出值 */
            // dac_set_voltage(1,temp); /* 异常地三角波顶部呈现梯形 */
            HAL_DAC_SetValue(&g_dac_handle,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);
            my_adc_self_test(); /* 使用VOFA+查看三角波 */
            delay_us(dt);
        }
    }
}


#endif

#if !9
void my_dac_self_test(void){
    uint16_t milli_voltage = 1000;
    int direction = 1;
    uint16_t step = 100;
    uint8_t time = 0;

    while(1){
        if((time % 10) == 0){
            my_adc_self_test(); /* 使用VOFA+查看三角波 */
        }

        if((time % 20) == 0){
            if(milli_voltage >= 3000){
                direction = -1;
            }
            if(milli_voltage <= 1000){
                direction = 1;
            }
            milli_voltage += direction * step;
            dac_set_voltage(1, milli_voltage);
        }

        time++;
        delay_ms(10);
    }
}
#endif
#if 9
void my_dac_self_test(void){
    dac_triangular_wave(3999, 1000, 1000, 10);  /* 区间0-3999, 采样点间隔1000us, 1000个采样点, 10个波形 */
}
#endif

