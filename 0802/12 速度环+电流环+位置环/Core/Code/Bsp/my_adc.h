#ifndef __MY_ADC_H
#define __MY_ADC_H

#include "stm32f4xx.h"

/*  BDC测量驱动板电压电流温度  zat*/
#if 1

/***************************************  电压、温度、电流 多通道ADC采集(DMA读取)*****************************************/
/* ADC及引脚 定义 */

#define ADC_ADCX_CH0_GPIO_PORT              GPIOB
#define ADC_ADCX_CH0_GPIO_PIN               GPIO_PIN_1
#define ADC_ADCX_CH0_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)         /* PB口时钟使能 */

#define ADC_ADCX_CH1_GPIO_PORT              GPIOA
#define ADC_ADCX_CH1_GPIO_PIN               GPIO_PIN_0
#define ADC_ADCX_CH1_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

#define ADC_ADCX_CH2_GPIO_PORT              GPIOB
#define ADC_ADCX_CH2_GPIO_PIN               GPIO_PIN_0
#define ADC_ADCX_CH2_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)         /* PB口时钟使能 */

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CH0                        ADC_CHANNEL_9                                       /* 电压测量通道 */ 
#define ADC_ADCX_CH1                        ADC_CHANNEL_0                                       /* 温度测量通道 */ 
#define ADC_ADCX_CH2                        ADC_CHANNEL_8                                       /* 电流测量通道 */ 

#define ADC_ADCX_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)          /* ADC1 时钟使能 */

#define ADC_CH_NUM                          3                                                   /* 需要转换的通道数目 */
#define ADC_COLL                            1000                                                /* 单采集次数 */
#define ADC_SUM                             ADC_CH_NUM * ADC_COLL                               /* 总采集次数 */

/*  DMA传输相关 定义 */
#define ADC_ADCX_DMASx                      DMA2_Stream4                                        /* 数据流4 */
#define ADC_ADCX_DMASx_Chanel               DMA_CHANNEL_0                                       /* 通道0 */
#define ADC_ADCX_DMASx_IRQn                 DMA2_Stream4_IRQn
#define ADC_ADCX_DMASx_IRQHandler           DMA2_Stream4_IRQHandler

/******************************************************************************************/

void adc_init(void);                        /* ADC初始化 */
void adc_nch_dma_init(void);                /* ADC DMA传输 初始化函数 */

/* 电流计算公式：
 * I=（最终输出电压-初始参考电压）/（6*0.02）A
 * ADC值转换为电压值：电压=ADC值*3.3/4096，这里电压单位为V，我们换算成mV,4096/1000=4.096，后面就直接算出为mA
 * 整合公式可以得出电流 I= （当前ADC值-初始参考ADC值）* （3.3 / 4.096 / 0.12）
 */
#define ADC2CURT    (float)(3.3f / 4.096f / 0.12f)

/* 电压计算公式：
 * V_POWER = V_BUS * 25
 * ADC值转换为电压值：电压=ADC值*3.3/4096
 * 整合公式可以得出电压V_POWER= ADC值 *（3.3f * 25 / 4096）
 */
#define ADC2VBUS    (float)(3.3f * 25 / 4096)

/*****************************************************************************************************/

extern uint16_t g_adc_val[ADC_CH_NUM];                         /*ADC平均值存放数组*/
float get_temp(uint16_t para);          /* 获取温度值 */
void calc_adc_val(uint16_t * p);        /* 计算ADC平均值 */

#endif

#if 9
void my_adc_self_test(void);
#endif

#endif
