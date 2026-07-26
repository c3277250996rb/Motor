#include "my_adc.h"

/*  ADC1(独立模式,单通道,中断)测量滑动变阻器电压  zat*/
/* adc的相关计算详见"f407_2026-07-25_ChatGPT_ADC单通道中断配置详解&计算方式.md" */
#if 0
__IO uint16_t ADC_ConvertedValue;
DMA_HandleTypeDef DMA_Init_Handle;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;

static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能 GPIO 时钟
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();
        
    // 配置 IO
    GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
    GPIO_InitStructure.Pull = GPIO_NOPULL ; //不上拉不下拉
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);		
}

static void Rheostat_ADC_Mode_Config(void)
{
    // 开启ADC时钟
    RHEOSTAT_ADC_CLK_ENABLE();
    // -------------------ADC Init 结构体 参数 初始化------------------------
    // ADC1
    ADC_Handle.Instance = RHEOSTAT_ADC;
    // 时钟为fpclk 4分频	
    ADC_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // ADC 分辨率
    ADC_Handle.Init.Resolution = ADC_RESOLUTION_12B;
    // 禁止扫描模式，多通道采集才需要	
    ADC_Handle.Init.ScanConvMode = DISABLE; 
    // 连续转换	
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    // 非连续转换	
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    // 非连续转换个数
    ADC_Handle.Init.NbrOfDiscConversion   = 0;
    //禁止外部边沿触发    
    ADC_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //使用软件触发，外部触发不用配置，注释掉即可
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //数据右对齐	
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //转换通道 1个
    ADC_Handle.Init.NbrOfConversion = 1;
    //使能连续转换请求
    ADC_Handle.Init.DMAContinuousRequests = ENABLE;
    //转换完成标志
    ADC_Handle.Init.EOCSelection          = DISABLE;    
    // 初始化ADC	                          
    HAL_ADC_Init(&ADC_Handle);
    //---------------------------------------------------------------------------
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    // 采样时间间隔	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_56CYCLES;
    ADC_Config.Offset       = 0;
    // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为3个时钟周期
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

    HAL_ADC_Start_IT(&ADC_Handle);
}

// 配置中断优先级
static void Rheostat_ADC_NVIC_Config(void)
{
  HAL_NVIC_SetPriority(Rheostat_ADC_IRQ, 0, 3);
  HAL_NVIC_EnableIRQ(Rheostat_ADC_IRQ);
}

void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
    Rheostat_ADC_NVIC_Config();
}

/**
  * @brief  转换完成中断回调函数（非阻塞模式）
  * @param  AdcHandle : ADC句柄
  * @retval 无
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  /* 获取结果 */
  ADC_ConvertedValue = HAL_ADC_GetValue(AdcHandle);
}

/**
  * @brief  This function handles ADC interrupt request.
  * @param  None
  * @retval None
  */
void ADC_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&ADC_Handle);
}
#endif

/*  ADC1(单通道,DMA读取)测量滑动变阻器电压  zat*/
#if 0
__IO uint16_t ADC_ConvertedValue;
DMA_HandleTypeDef DMA_Init_Handle;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;
uint16_t g_adc_dma_buf[ADC_CH_NUM * ADC_DMA_BUF_SIZE];      /* ADC DMA BUF */
uint16_t g_adc_val[ADC_CH_NUM];                             /* ADC平均值存放数组 */

/**
 * @brief       ADC初始化函数
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    // 开启ADC时钟
    RHEOSTAT_ADC_CLK_ENABLE();
    // -------------------ADC Init 结构体 参数 初始化------------------------
    // ADC1
    ADC_Handle.Instance = RHEOSTAT_ADC;
    // 时钟为fpclk 4分频	
    ADC_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // ADC 分辨率
    ADC_Handle.Init.Resolution = ADC_RESOLUTION_12B;
    // 禁止扫描模式，多通道采集才需要	
    ADC_Handle.Init.ScanConvMode = DISABLE; 
    // 连续转换	
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    // 非连续转换	
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    // 非连续转换个数
    ADC_Handle.Init.NbrOfDiscConversion   = 0;
    //禁止外部边沿触发    
    ADC_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //使用软件触发，外部触发不用配置，注释掉即可
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //数据右对齐	
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //转换通道 1个
    ADC_Handle.Init.NbrOfConversion = 1;
    //使能连续转换请求
    ADC_Handle.Init.DMAContinuousRequests = ENABLE;
    //转换完成标志
    ADC_Handle.Init.EOCSelection          = DISABLE;    
    // 初始化ADC	                          
    HAL_ADC_Init(&ADC_Handle);
    //---------------------------------------------------------------------------
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    // 采样时间间隔	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_56CYCLES;
    ADC_Config.Offset       = 0;
    // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为3个时钟周期
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
}

/**
 * @brief       ADC DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void adc_dma_init(void)
{
    // ------------------DMA Init 结构体参数 初始化--------------------------
    // ADC1使用DMA2，数据流0，通道0，这个是手册固定死的
    // 开启DMA时钟
    RHEOSTAT_ADC_DMA_CLK_ENABLE();
    // 数据传输通道
    DMA_Init_Handle.Instance = RHEOSTAT_ADC_DMA_STREAM;
    // 数据传输方向为外设到存储器	
    DMA_Init_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;	
    // 外设寄存器只有一个，地址不用递增
    DMA_Init_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    // 存储器地址固定
    DMA_Init_Handle.Init.MemInc = DMA_MINC_ENABLE; 
    // // 外设数据大小为半字，即两个字节 
    DMA_Init_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; 
    //	存储器数据大小也为半字，跟外设数据大小相同
    DMA_Init_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	
    // 循环传输模式
    DMA_Init_Handle.Init.Mode = DMA_CIRCULAR;
    // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
    DMA_Init_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    // 禁止DMA FIFO	，使用直连模式
    DMA_Init_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;  
    // FIFO 大小，FIFO模式禁止时，这个不用配置	
    DMA_Init_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    DMA_Init_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
    DMA_Init_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;  
    // 选择 DMA 通道，通道存在于流中
    DMA_Init_Handle.Init.Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
    //初始化DMA流，流相当于一个大的管道，管道里面有很多通道
    HAL_DMA_Init(&DMA_Init_Handle);
    __HAL_LINKDMA(&ADC_Handle,DMA_Handle,DMA_Init_Handle);
    
    adc_init();                                                             /* 初始化ADC */

    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 3, 3);                        /* 设置DMA中断优先级为3，子优先级为3 */
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);                                /* 使能DMA中断 */
}

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无
 * @retval      无
 */
void ADC_ADCX_DMASx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&DMA_Init_Handle);
}

/**
 * @brief       ADC转换完成的回调函数
 * @param       无
 * @retval      无
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == RHEOSTAT_ADC) 
    { 
        HAL_ADC_Stop_DMA(&ADC_Handle);                                                                         /* 关闭DMA转换 */
        calc_adc_val(g_adc_dma_buf, g_adc_val, ADC_CH_NUM, ADC_DMA_BUF_SIZE);                                    /* ADC数值转换 */
        HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t *)&g_adc_dma_buf, (uint32_t)(ADC_CH_NUM * ADC_DMA_BUF_SIZE)); /* 再启动DMA转换 */
    }
}

/**
 * @brief       ADC底层驱动，引脚配置，时钟使能
                此函数会被HAL_ADC_Init()调用
 * @param       hadc:ADC句柄
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == RHEOSTAT_ADC)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        // 使能 GPIO 时钟
        RHEOSTAT_ADC_GPIO_CLK_ENABLE();
        // 配置 IO
        GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
        GPIO_InitStructure.Pull = GPIO_NOPULL ; //不上拉不下拉
        HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);	
    }
}

/**
 * @brief       计算ADC的平均值（滤波）
 * @param       * p ：代入ADC原始值
 * @param       * buf ：存放滤波后的ADC值
 * @param       ch_num ：采集的ADC通道数量
 * @param       len ：每个通道的采集次数
 * @note        此函数对电压、温度、电流对应的ADC值进行滤波
 * @retval      无
 */
void calc_adc_val(uint16_t * p, uint16_t * buf , uint16_t ch_num, uint16_t len)
{
    uint32_t temp[ADC_CH_NUM] = {0};                        /* 定义一个缓存数组 */
    int i , j;                                              
    for (i = 0 ; i < len ; i++)                             /* 根据ADC通道数循环获取，并累加 */
    {
        for (j = 0 ; j < ch_num ; j++)                      /* 将采集到的ADC值，各通道进行累加 */
        {
            temp[j] += p[j + i * ch_num];
        }
    }
    for (j = 0 ; j < ch_num ; j++)
    {
        temp[j] /= len;                                     /* 获取平均值 */
        buf[j] = temp[j];                                   /* 将滤波后的值存到buf里 */
    }
}
#endif

/*  ADC1(3通道,DMA读取)  zat*/
#if 0
__IO uint16_t ADC_ConvertedValue;
DMA_HandleTypeDef DMA_Init_Handle;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;
uint16_t g_adc_dma_buf[ADC_CH_NUM * ADC_DMA_BUF_SIZE];      /* ADC DMA BUF */
uint16_t g_adc_val[ADC_CH_NUM];                             /* ADC平均值存放数组 */

/**
 * @brief       ADC初始化函数
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    // 开启ADC时钟
    RHEOSTAT_ADC_CLK_ENABLE();
    // -------------------ADC Init 结构体 参数 初始化------------------------
    // ADC1
    ADC_Handle.Instance = RHEOSTAT_ADC;
    // 时钟为fpclk 4分频	
    ADC_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // ADC 分辨率
    ADC_Handle.Init.Resolution = ADC_RESOLUTION_12B;
    // 扫描模式，多通道采集才需要	
    ADC_Handle.Init.ScanConvMode = ENABLE; 
    // 连续转换	
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    // 非连续转换	
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    // 非连续转换个数
    ADC_Handle.Init.NbrOfDiscConversion   = 0;
    //禁止外部边沿触发    
    ADC_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //使用软件触发，外部触发不用配置，注释掉即可
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //数据右对齐	
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //转换通道
    ADC_Handle.Init.NbrOfConversion = RHEOSTAT_NOFCHANEL;
    //使能连续转换请求
    ADC_Handle.Init.DMAContinuousRequests = ENABLE;
    //转换完成标志
    ADC_Handle.Init.EOCSelection          = DISABLE;    
    // 初始化ADC	                          
    HAL_ADC_Init(&ADC_Handle);
    //---------------------------------------------------------------------------
    // 配置 ADC 通道1转换顺序为1，第一个转换
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL1;
    ADC_Config.Rank         = 1;    
    ADC_Config.SamplingTime = ADC_SAMPLETIME_56CYCLES;// 采样时间间隔	
    ADC_Config.Offset       = 0;
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
    
    // 配置 ADC 通道2转换顺序为2，第二个转换
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL2;
    ADC_Config.Rank         = 2;
    ADC_Config.SamplingTime = ADC_SAMPLETIME_56CYCLES; // 采样时间间隔	
    ADC_Config.Offset       = 0;
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
    
    // 配置 ADC 通道3转换顺序为3，第三个转换
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL3;
    ADC_Config.Rank         = 3;    	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_56CYCLES;// 采样时间间隔
    ADC_Config.Offset       = 0;
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

}

/**
 * @brief       ADC DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void adc_dma_init(void)
{
    // ------------------DMA Init 结构体参数 初始化--------------------------
    // ADC1使用DMA2，数据流0，通道0，这个是手册固定死的
    // 开启DMA时钟
    RHEOSTAT_ADC_DMA_CLK_ENABLE();
    // 数据传输通道
    DMA_Init_Handle.Instance = RHEOSTAT_ADC_DMA_STREAM;
    // 数据传输方向为外设到存储器	
    DMA_Init_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;	
    // 外设寄存器只有一个，地址不用递增
    DMA_Init_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    // 存储器地址固定
    DMA_Init_Handle.Init.MemInc = DMA_MINC_ENABLE; 
    // // 外设数据大小为半字，即两个字节 
    DMA_Init_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; 
    //	存储器数据大小也为半字，跟外设数据大小相同
    DMA_Init_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	
    // 循环传输模式
    DMA_Init_Handle.Init.Mode = DMA_CIRCULAR;
    // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
    DMA_Init_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    // 禁止DMA FIFO	，使用直连模式
    DMA_Init_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;  
    // FIFO 大小，FIFO模式禁止时，这个不用配置	
    DMA_Init_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    DMA_Init_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
    DMA_Init_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;  
    // 选择 DMA 通道，通道存在于流中
    DMA_Init_Handle.Init.Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
    //初始化DMA流，流相当于一个大的管道，管道里面有很多通道
    HAL_DMA_Init(&DMA_Init_Handle);
    __HAL_LINKDMA(&ADC_Handle,DMA_Handle,DMA_Init_Handle);
    
    adc_init();                                                             /* 初始化ADC */

    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 3, 3);                        /* 设置DMA中断优先级为3，子优先级为3 */
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);                                /* 使能DMA中断 */
}

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无
 * @retval      无
 */
void ADC_ADCX_DMASx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&DMA_Init_Handle);
}

/**
 * @brief       ADC转换完成的回调函数
 * @param       无
 * @retval      无
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == RHEOSTAT_ADC) 
    { 
        HAL_ADC_Stop_DMA(&ADC_Handle);                                                                         /* 关闭DMA转换 */
        calc_adc_val(g_adc_dma_buf, g_adc_val, ADC_CH_NUM, ADC_DMA_BUF_SIZE);                                    /* ADC数值转换 */
        HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t *)&g_adc_dma_buf, (uint32_t)(ADC_CH_NUM * ADC_DMA_BUF_SIZE)); /* 再启动DMA转换 */
    }
}

/**
 * @brief       ADC底层驱动，引脚配置，时钟使能
                此函数会被HAL_ADC_Init()调用
 * @param       hadc:ADC句柄
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == RHEOSTAT_ADC)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        /*=====================通道1======================*/
        // 使能 GPIO 时钟
        RHEOSTAT_ADC_GPIO_CLK1_ENABLE();    
        // 配置 IO
        GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN1;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
        GPIO_InitStructure.Pull = GPIO_NOPULL ; //不上拉不下拉
        HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT1, &GPIO_InitStructure);
        
        /*=====================通道2======================*/
        // 使能 GPIO 时钟
        RHEOSTAT_ADC_GPIO_CLK2_ENABLE();    
        // 配置 IO
        GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN2;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
        GPIO_InitStructure.Pull = GPIO_NOPULL ; //不上拉不下拉
        HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT2, &GPIO_InitStructure);
        
        /*=====================通道3======================*/
        // 使能 GPIO 时钟
        RHEOSTAT_ADC_GPIO_CLK3_ENABLE();    
        // 配置 IO
        GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN3;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
        GPIO_InitStructure.Pull = GPIO_NOPULL ; //不上拉不下拉
        HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT3, &GPIO_InitStructure); 
    }
}

/**
 * @brief       计算ADC的平均值（滤波）
 * @param       * p ：代入ADC原始值
 * @param       * buf ：存放滤波后的ADC值
 * @param       ch_num ：采集的ADC通道数量
 * @param       len ：每个通道的采集次数
 * @note        此函数对电压、温度、电流对应的ADC值进行滤波
 * @retval      无
 */
void calc_adc_val(uint16_t * p, uint16_t * buf , uint16_t ch_num, uint16_t len)
{
    uint32_t temp[ADC_CH_NUM] = {0};                        /* 定义一个缓存数组 */
    int i , j;                                              
    for (i = 0 ; i < len ; i++)                             /* 根据ADC通道数循环获取，并累加 */
    {
        for (j = 0 ; j < ch_num ; j++)                      /* 将采集到的ADC值，各通道进行累加 */
        {
            temp[j] += p[j + i * ch_num];
        }
    }
    for (j = 0 ; j < ch_num ; j++)
    {
        temp[j] /= len;                                     /* 获取平均值 */
        buf[j] = temp[j];                                   /* 将滤波后的值存到buf里 */
    }
}

#endif

/*  ADC1(内部温度传感器通道)测量MCU温度 zat*/
#if 4

ADC_HandleTypeDef g_adc_handle;   /* ADC句柄 */

/**
 * @brief       ADC内部温度传感器 初始化函数
 * @param       无
 * @retval      无
 */
void adc_temperature_init(void)
{
    ADC_ADCX_CHY_CLK_ENABLE();
    
    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;            /* 4分频，ADCCLK = SYSCLK/4 = 170/4 = 42.5Mhz */
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12位模式 */
    // g_adc_handle.Init.GainCompensation = 0;                                 /* 不需要增益补偿 */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* 右对齐 */
    g_adc_handle.Init.ScanConvMode = DISABLE;                               /* 非扫描模式 */
    g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                   /* 单通道转换结束标志 */
    // g_adc_handle.Init.LowPowerAutoWait = DISABLE;                           /* 禁用低功耗延迟模式 */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;                         /* 失能连续转换 */
    g_adc_handle.Init.NbrOfConversion = 1;                                  /* 1个转换在规则序列中 也就是只转换规则序列1 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;                              /* 不连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发 */
    g_adc_handle.Init.DMAContinuousRequests = DISABLE;                      /* 关闭DMA请求 */
    // g_adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                   /* 数据溢出时覆盖ADC数据 */
    // g_adc_handle.Init.OversamplingMode = DISABLE;                           /* 禁用过采样功能 */
    HAL_ADC_Init(&g_adc_handle);                                            /* 初始化 */
}

/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_2CYCLES_5,  2.5个ADC时钟周期        ADC_SAMPLETIME_6CYCLES_5, 6.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_12CYCLES_5, 12.5个ADC时钟周期       ADC_SAMPLETIME_24CYCLES_5, 24.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_47CYCLES_5, 47.5个ADC时钟周期       ADC_SAMPLETIME_92CYCLES_5, 92.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_247CYCLES_5 , 247.5个ADC时钟周期    ADC_SAMPLETIME_640CYCLES_5,640.5个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channel1的rank=1，channel2的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就是通道1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    /* 配置对应ADC通道 */
    ADC_ChannelConfTypeDef adc_channel = {0};
    adc_channel.Channel = ch;                           /* 设置ADCX对通道ch */
    adc_channel.Rank = rank;                            /* 设置采样序列 */
    adc_channel.SamplingTime = stime;                   /* 设置采样时间 */
    // adc_channel.SingleDiff = ADC_SINGLE_ENDED;          /* 单端输入模式 */
    adc_channel.Offset = 0;                             /* 偏移量为0 */
    // adc_channel.OffsetNumber = ADC_OFFSET_NONE;         /* 不使用偏移量的通道 */
    HAL_ADC_ConfigChannel( adc_handle, &adc_channel );   
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道值 0~17，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      无
 */
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&g_adc_handle, ch, 1, ADC_SAMPLETIME_480CYCLES);  /* 设置通道，序列和采样时间 */
    HAL_ADC_Start(&g_adc_handle);                                                        /* 开启ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, 10);                                        /* 轮询转换 */

    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);                                    /* 返回最近一次ADC1规则组的转换结果 */
}

/**
 * @brief       获取通道ch的转换值，取times次, 然后平均
 * @param       ch      : 通道号, ADC_CHANNEL_0 ~ ADC_CHANNEL_17
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* 获取times次数据 */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;        /* 返回平均值 */
}


/**
 * @brief       获取内部温度传感器温度值
 * @param       无
 * @retval      温度值(扩大了100倍,单位:℃)
 */
float adc_get_temperature(void)
{
    uint32_t adcx;
    float temperature;
    float temp = 0;
    float per = 1;
    uint16_t ts_cal1, ts_cal2;
    
    ts_cal1 = *(volatile uint16_t *)(TEMPSENSOR_CAL1_ADDR);           /* 获取TS_CAL1 */     printf("ts_cal1: %d    ", ts_cal1);
    ts_cal2 = *(volatile uint16_t *)(TEMPSENSOR_CAL2_ADDR);           /* 获取TS_CAL2 */     printf("ts_cal2: %d    ", ts_cal2);
    temp = (float)((float)(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) / (float)(ts_cal2 - ts_cal1));   /* 获取比例因子 */     printf("TEMPSENSOR_CAL1_TEMP: %d    TEMPSENSOR_CAL2_TEMP: %d    ", TEMPSENSOR_CAL1_TEMP, TEMPSENSOR_CAL2_TEMP);
    
    adcx = adc_get_result_average(ADC_TEMPSENSOR_CHX, 20);  /* 读取内部温度传感器通道,20次取平均 */     printf("adcx: %d    ", adcx);
    
    per = 3.3F / ((float)TEMPSENSOR_CAL_VREFANALOG / 1000.0F);   /* ts_cal1和ts_cal2是在VREF+为TEMPSENSOR_CAL_VREFANALOG测试的，所以需要将adcx进行转换 */    printf("per: %f    ", per);
    
    temperature = temp * ((float)adcx * per - (float)ts_cal1) + (float)TEMPSENSOR_CAL1_TEMP;    /* 转化为电压值 */       printf("temperature: %f\r\n", temperature);
    
    return temperature;
}


#endif

#if 9
void my_adc_self_test(void){
    // 局部变量，用于保存转换计算后的电压值
    float ADC_Vol; 

    while(1){
        // for(int i = 0; i < RHEOSTAT_NOFCHANEL; i++){
        //     ADC_ConvertedValue = g_adc_val[i];
        //     ADC_Vol = (float)g_adc_val[i] * (3.3 / 4096);
        //     printf("[%d]The current AD value = 0x%04X\r\n", i, ADC_ConvertedValue); 
        //     printf("[%d]The current AD value = %f V\r\n\r\n", i, ADC_Vol); 
        // }

        printf("temperature: %f\r\n", adc_get_temperature());
        delay_ms(100);
    }

}
#endif
