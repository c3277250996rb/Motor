#include "my_dac.h"
#include "my_adc.h"

/*  DAC普通输出  zat*/
#if 0

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
#if 0

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

/*  DAC(TIM2更新触发输出,DMA搬运数据)输出正弦波  zat*/
#if 3

DMA_HandleTypeDef g_dma_dac_handle;             /* 定义要搬运DAC数据的DMA句柄 */
DAC_HandleTypeDef g_dac_dma_handle;             /* 定义DAC（DMA输出）句柄 */

uint16_t g_dac_sin_buf[512];                   /* 发送数据缓冲区 */

#include "math.h"
/**
 * @brief       产生正弦波函序列
 * @note        需保证: maxval > samples/2
 *
 * @param       maxval : 峰值(0 < maxval < 2048)
 * @param       samples: 采样点的个数
 *
 * @retval      无
 */
void dac_creat_sin_buf(uint16_t maxval, uint16_t samples)
{
    #ifndef __math_h
    #error "请先包含头文件math.h, 不然正弦函数(sin)计算不对"
    #endif
    uint8_t i;
    float outdata = 0;
    
    if(maxval <= (samples / 2))return ;         /* 数据不合法 */
    
    /* 
     * 正弦波最小正周期为2π，约等于2 * 3.1415926
     * 曲线上相邻的两个点在x轴上的间隔 = 2 * 3.1415926 / 采样点数量
     */
    float inc = (2 * 3.1415926) / samples;      /* 计算相邻两个点的x轴间隔 */

    for (i = 0; i < samples; i++)               /* 连续打samples个点 */
    {
        /* 
         * 正弦波函数解析式：y = Asin(wx + φ）+ b
         * 计算每个点的y值，将峰值放大maxval倍，并将曲线向上偏移maxval到正数区域
         * 注意：DAC无法输出负电压，所以需要将曲线向上偏移一个峰值的量，让整个曲线都落在正数区域
         */
        outdata = maxval * sin(inc * i) + maxval;
        
        if (outdata > 4095)
        {
            outdata = 4095;                     /* 上限限定 */
        }
        g_dac_sin_buf[i] = outdata;
    }
}

/**
 * @brief       DAC DMA初始化函数
 * @param       outx: 要初始化的通道.1,通道1; 2,通道2
 * @param       cndtr: DMA通道单次传输数据量（采样点数量）
 * @retval      无
 */
void dac_init(uint8_t outx,uint16_t cndtr)
{
    GPIO_InitTypeDef gpio_init_struct;
    DAC_ChannelConfTypeDef DACCH1_Config;
    
    __HAL_RCC_DAC_CLK_ENABLE();                                             /* 使能DAC的时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                                           /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    gpio_init_struct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;              /* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG; 
    gpio_init_struct.Pull = GPIO_PULLUP;      
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    g_dma_dac_handle.Instance = DMA1_Stream5;                               /* DMA1数据流5 */
    g_dma_dac_handle.Init.Channel = DMA_CHANNEL_7;                          /* 通道7 */
    g_dma_dac_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;                 /* 从存储器到外设模式 */
    g_dma_dac_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_dac_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_dac_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_dac_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_dac_handle.Init.Mode = DMA_CIRCULAR;                              /* 循环模式 */
    g_dma_dac_handle.Init.Priority = DMA_PRIORITY_LOW;                      /* 中等优先级 */
    g_dma_dac_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;                  /* 不开启FIFO */
    HAL_DMA_Init(&g_dma_dac_handle);                                        /* 初始化DMA */

    __HAL_LINKDMA(&g_dac_dma_handle,DMA_Handle1,g_dma_dac_handle);          /* 把DAC和DMA关联 */
    
    g_dac_dma_handle.Instance = DAC;
    HAL_DAC_Init(&g_dac_dma_handle);                                        /* 初始化DAC */

    DACCH1_Config.DAC_Trigger = DAC_TRIGGER_T2_TRGO;                        /* 使用定时器2的TRGO事件触发DAC转换 */
    DACCH1_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;              /* DAC1输出缓冲关闭 */
    
    dac_creat_sin_buf(2048,cndtr);                                          /* 产生正弦波序列，即画点 */
    
    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&g_dac_dma_handle,&DACCH1_Config,DAC_CHANNEL_1);                                    /* DAC通道1配置 */
            HAL_DAC_Start_DMA(&g_dac_dma_handle,DAC_CHANNEL_1,(uint32_t *)g_dac_sin_buf,cndtr,DAC_ALIGN_12B_R);       /* 开启DAC通道1 */
            break;
        case 2:
            HAL_DAC_ConfigChannel(&g_dac_dma_handle,&DACCH1_Config,DAC_CHANNEL_2);                                    /* DAC通道2配置 */
            HAL_DAC_Start_DMA(&g_dac_dma_handle,DAC_CHANNEL_2,(uint32_t *)g_dac_sin_buf,cndtr,DAC_ALIGN_12B_R);       /* 开启DAC通道2 */
            break;
        default:break;
    }   
}

#include "my_tim.h"
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

  /* USER CODE END DMA1_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&g_dma_dac_handle);
  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

  /* USER CODE END DMA1_Stream5_IRQn 1 */
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
    // dac_triangular_wave(3999, 1000, 1000, 10);  /* 区间0-3999, 采样点间隔1000us, 1000个采样点, 10个波形 */
    my_adc_self_test();
}
#endif

