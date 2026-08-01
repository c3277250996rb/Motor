#include "my_adc.h"
#include "my_tim.h"

/*  BDC测量驱动板电压电流温度  zat*/
#if 1

/* 多通道ADC采集 DMA读取 */
ADC_HandleTypeDef g_adc_nch_dma_handle;                 /* 与DMA关联的ADC句柄 */
DMA_HandleTypeDef g_dma_nch_adc_handle;                 /* 与ADC关联的DMA句柄 */

uint16_t g_adc_value[ADC_CH_NUM * ADC_COLL] = {0};      /* 存储ADC原始值 */
uint16_t g_adc_val[ADC_CH_NUM];                         /*ADC平均值存放数组*/

/***************************************  电压、电流、温度 多通道ADC采集(DMA读取)程序*****************************************/

/**
 * @brief       ADC初始化函数
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    g_adc_nch_dma_handle.Instance = ADC_ADCX;                                       /* ADCx */
    g_adc_nch_dma_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;            /* 4分频，ADCCLK = PCLK2/4 = 84/4 = 21Mhz */
    g_adc_nch_dma_handle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12位模式 */
    g_adc_nch_dma_handle.Init.ScanConvMode = ENABLE;                                /* 扫描模式 多通道使用 */
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;                          /* 连续转换模式，转换完成之后接着继续转换 */
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止不连续采样模式 */
    g_adc_nch_dma_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发 */
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* 右对齐 */
    g_adc_nch_dma_handle.Init.NbrOfConversion = ADC_CH_NUM;                         /* 使用转换通道数，需根据实际转换通道去设置 */
    g_adc_nch_dma_handle.Init.DMAContinuousRequests = ENABLE;                       /* 开启DMA连续转换请求 */
    g_adc_nch_dma_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    HAL_ADC_Init(&g_adc_nch_dma_handle);

    /* 配置使用的ADC通道，采样序列里的第几个转换，增加或者减少通道需要修改这部分 */
    sConfig.Channel = ADC_ADCX_CH0;
    sConfig.Rank = 1;
    // sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);

    sConfig.Channel = ADC_ADCX_CH1;
    sConfig.Rank = 2;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);

    sConfig.Channel = ADC_ADCX_CH2;
    sConfig.Rank = 3;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);
    
}

/**
 * @brief       ADC DMA传输 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void adc_nch_dma_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
      
    ADC_ADCX_CHY_CLK_ENABLE();                                  /* 开启ADCx时钟 */
    ADC_ADCX_CH0_GPIO_CLK_ENABLE();                             /* 开启GPIO时钟 */
    ADC_ADCX_CH1_GPIO_CLK_ENABLE();
    ADC_ADCX_CH2_GPIO_CLK_ENABLE();
    
    /* AD采集引脚模式设置,模拟输入 */
    GPIO_InitStruct.Pin = ADC_ADCX_CH0_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_ADCX_CH0_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ADC_ADCX_CH1_GPIO_PIN;
    HAL_GPIO_Init(ADC_ADCX_CH1_GPIO_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = ADC_ADCX_CH2_GPIO_PIN;   
    HAL_GPIO_Init(ADC_ADCX_CH2_GPIO_PORT, &GPIO_InitStruct); 
    
    adc_init();                                                 /* 初始化ADC */
    
    if ((uint32_t)ADC_ADCX_DMASx > (uint32_t)DMA2)              /* 大于DMA1_Channel7, 则为DMA2的通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                            /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                            /* DMA1时钟使能 */
    }

    /* DMA配置 */
    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMASx;                             /* 数据流x */
    g_dma_nch_adc_handle.Init.Channel = ADC_ADCX_DMASx_Chanel;                  /* DMA通道x */
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* DIR = 1 ,外设到存储器模式 */
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_nch_adc_handle.Init.Mode = DMA_CIRCULAR;                              /* 外设流控模式 */
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_nch_adc_handle);
 
    __HAL_LINKDMA(&g_adc_nch_dma_handle,DMA_Handle,g_dma_nch_adc_handle);       /* 把ADC和DMA关联，用DMA传输ADC数据 */

    /* ADC DMA中断配置 */
    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);
    
    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle,(uint32_t *)g_adc_value,ADC_SUM);    /* 开启ADC的DMA传输 */
}

/**
 * @brief       DMA2 数据流4中断服务函数
 * @param       无 
 * @retval      无
 */
void ADC_ADCX_DMASx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_nch_adc_handle);
}

/**
 * @brief       ADC 采集中断服务回调函数
 * @param       无 
 * @retval      无
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    #if 0
        if (hadc->Instance == ADC_ADCX){
            HAL_ADC_Stop_DMA(&g_adc_nch_dma_handle);  /* 关闭DMA转换 */
            calc_adc_val(g_adc_val);                  /* 计算ADC的平均值 */
            HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, (uint32_t *)&g_adc_value, (uint32_t)(ADC_SUM)); /* 启动DMA转换 */
        }
    #endif

    float temp_c = 0.0;
    static float add_adc = 0;
    static float init_adc_value = 0;
    static uint8_t adc_count1 = 0, adc_count2 = 0;
    
    if ( hadc->Instance == ADC_ADCX )                               /* 判断是不是ADC1 */
    { 
        adc_count1++;
        HAL_ADC_Stop_DMA(&g_adc_nch_dma_handle);                    /* 关闭DMA转换 */
        
        calc_adc_val(g_adc_val);                                    /* 计算ADC的平均值 */
        add_adc += g_adc_val[2];                                    /* 取出电流通道对应的ADC值进行累计 */

        if (adc_count1 >= 15)                                       /* 累计15次 */
        {
            add_adc = (float)(add_adc / adc_count1);                /* 取平均值 */

            if (adc_count2 <= 16)                                   /* 采集16次ADC平均值计算参考电压的ADC值 */
            {
                adc_count2++;
                init_adc_value += add_adc;                          /* 对平均值累计求和 */

                if (adc_count2 == 16)                               /* 平均值累计16次 */
                { 
                    adc_count2 = 17;                                /* 不再进入 */
                    init_adc_value = (init_adc_value / 16.0f);      /* 存储初始ADC值 */
                }
            }

            if (adc_count2 >= 17)                                   /* 采集完参考ADC值后，采集电流通道当前ADC值 */
            {
                
                temp_c = (add_adc - init_adc_value) * ADC2CURT;                                                 /* 计算电流 */
                
                g_motor_data.current = (float)((g_motor_data.current * (float)0.60) + ((float)0.40 * temp_c));  /* 一阶低通滤波 */

                if (g_motor_data.current <= 20)                                                                 /* 过滤掉微弱浮动电流 */
                {
                    g_motor_data.current = 0.0;
                }

            }
            add_adc = 0;
            adc_count1 = 0;
        }

        HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, (uint32_t *)&g_adc_value, (uint32_t)(ADC_SUM));                /* 启动DMA转换 */
    }
}

/*************************************    第二部分    电压电流温度采集    **********************************************/

/*
    Rt = Rp *exp(B*(1/T1-1/T2))

    Rt 是热敏电阻在T1温度下的阻值；
    Rp是热敏电阻在T2常温下的标称阻值；
    exp是e的n次方，e是自然常数，就是自然对数的底数，近似等于 2.7182818；
    B值是热敏电阻的重要参数，教程中用到的热敏电阻B值为3380；
    这里T1和T2指的是开尔文温度，T2是常温25℃，即(273.15+25)K
    T1就是所求的温度
*/

const float Rp = 10000.0f;          /* 10K */
const float T2 = (273.15f + 25.0f); /* T2 */
const float Bx = 3380.0f;           /* B */
const float Ka = 273.15f;

/**
 * @brief       计算温度值
 * @param       para: 温度采集对应ADC通道的值（已滤波）
 * @note        计算温度分为两步：
                1.根据ADC采集到的值计算当前对应的Rt
                2.根据Rt计算对应的温度值
 * @retval      温度值
 */
#include "math.h"
float get_temp(uint16_t para)
{
    #ifndef __math_h
    #error "当前没有包含头文件math.h,计算温度的时候(会用到log函数)会算错"
    #endif
    float Rt;
    float temp;
    
    /* 
        第一步：
        Rt = 3.3 * 4700 / VTEMP - 4700 ,其中VTEMP就是温度检测通道采集回来的电压值,VTEMP = ADC值* 3.3/4096
        由此我们可以计算出当前Rt的值：Rt = 3.3f * 4700.0f / (para * 3.3f / 4096.0f ) - 4700.0f; 
    */
    
    Rt = 3.3f * 4700.0f / (para * 3.3f / 4096.0f ) - 4700.0f;       /* 根据当前ADC值计算出Rt的值 */

    /* 
        第二步：
        根据当前Rt的值来计算对应温度值：Rt = Rp *exp(B*(1/T1-1/T2)) 
    */
    
    temp = Rt / Rp;                 /* 解出exp(B*(1/T1-1/T2)) ，即temp = exp(B*(1/T1-1/T2)) */
    temp = log(temp);               /* 解出B*(1/T1-1/T2) ，即temp = B*(1/T1-1/T2) */
    temp /= Bx;                     /* 解出1/T1-1/T2 ，即temp = 1/T1-1/T2 */
    temp += (1.0f / T2);            /* 解出1/T1 ，即temp = 1/T1 */
    temp = 1.0f / (temp);           /* 解出T1 ，即temp = T1 */
    temp -= Ka;                     /* 计算T1对应的摄氏度 */
    return temp;                    /* 返回温度值 */
}

extern uint16_t g_adc_value[ADC_CH_NUM * ADC_COLL];

/**
 * @brief       计算ADC的平均值（滤波）
 * @param       * p ：存放ADC值的指针地址
 * @note        此函数对电压、温度、电流对应的ADC值进行滤波，
 *              p[0]-p[2]对应的分别是电压、温度和电流
 * @retval      无
 */
void calc_adc_val(uint16_t * p)
{
    uint32_t temp[3] = {0,0,0};
    int i;
    for(i=0;i<ADC_COLL;i++)         /* 循环ADC_COLL次取值，累加 */
    {
        temp[0] += g_adc_value[0+i*ADC_CH_NUM];
        temp[1] += g_adc_value[1+i*ADC_CH_NUM];
        temp[2] += g_adc_value[2+i*ADC_CH_NUM];
    }
    temp[0] /= ADC_COLL;            /* 取平均值 */
    temp[1] /= ADC_COLL;
    temp[2] /= ADC_COLL;
    p[0] = temp[0];                 /* 存入电压ADC通道平均值 */
    p[1] = temp[1];                 /* 存入温度ADC通道平均值 */
    p[2] = temp[2];                 /* 存入电流ADC通道平均值 */
}

#endif

#if 9
void my_adc_self_test(void){
    printf("Valtage: %.1fV  Temperature: %.1fC  Ampere: %.1f\r\n", g_adc_val[0]*ADC2VBUS, get_temp(g_adc_val[1]), abs(g_adc_val[2])*ADC2CURT);
}
#endif

