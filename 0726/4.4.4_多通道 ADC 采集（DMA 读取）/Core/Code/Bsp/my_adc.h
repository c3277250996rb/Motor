#ifndef __MY_ADC_H
#define __MY_ADC_H

#include "stm32f4xx.h"
#include "main.h"

/*  ADC1(独立模式,单通道,中断)测量滑动变阻器电压  zat*/
/** adc的相关计算详见"f407_2026-07-25_ChatGPT_ADC单通道中断配置详解&计算方式.md"
 * ADC1 <- CH8 <- PB0 <- 滑动变阻器
 */
#if 0

// ADC GPIO 宏定义
#define RHEOSTAT_ADC_GPIO_PORT              GPIOB
#define RHEOSTAT_ADC_GPIO_PIN               GPIO_PIN_0
#define RHEOSTAT_ADC_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
    
// ADC 序号宏定义
#define RHEOSTAT_ADC                        ADC1
#define RHEOSTAT_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL                ADC_CHANNEL_8

// ADC 中断宏定义
#define Rheostat_ADC_IRQ                    ADC_IRQn
#define Rheostat_ADC_INT_FUNCTION           ADC_IRQHandler


void Rheostat_Init(void);

void my_adc_self_test(void);

#endif

/*  ADC1(单通道,DMA读取)测量滑动变阻器电压  zat*/
#if 0

// ADC GPIO 宏定义
#define RHEOSTAT_ADC_GPIO_PORT              GPIOB
#define RHEOSTAT_ADC_GPIO_PIN               GPIO_PIN_0
#define RHEOSTAT_ADC_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
    
// ADC 序号宏定义
#define RHEOSTAT_ADC                        ADC1
#define RHEOSTAT_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL                ADC_CHANNEL_8

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR                ((uint32_t)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK_ENABLE()       __DMA2_CLK_ENABLE()
#define RHEOSTAT_ADC_DMA_CHANNEL            DMA_CHANNEL_0
#define RHEOSTAT_ADC_DMA_STREAM             DMA2_Stream0

/* ADC单通道/多通道 DMA采集 DMA通道相关 定义
 * 注意: 这里我们的通道还是使用上面的定义.
 */
#define ADC_ADCX_DMASx_IRQn                 DMA2_Stream0_IRQn
#define ADC_ADCX_DMASx_IRQHandler           DMA2_Stream0_IRQHandler

#define ADC_DMA_BUF_SIZE        50                                  /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */
#define ADC_CH_NUM              1                                   /* ADC 通道数量 */
extern uint16_t g_adc_dma_buf[ADC_CH_NUM * ADC_DMA_BUF_SIZE];       /* ADC DMA BUF */
extern uint16_t g_adc_val[ADC_CH_NUM];                              /* ADC平均值存放数组 */
extern ADC_HandleTypeDef ADC_Handle;                              /* ADC句柄 */

/******************************************************************************************/

void adc_init(void);                                                                                /* ADC初始化 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime);    /* ADC通道设置 */

void adc_dma_init(void);                                                                            /* ADC DMA采集初始化 */
void calc_adc_val(uint16_t * p, uint16_t * buf , uint16_t ch_num, uint16_t len);                    /* 计算ADC平均值 */

#endif

/*  ADC1(3通道,DMA读取)  zat*/
/**
 * ADC1 <- IN4 <- PA4
 * ADC1 <- IN8 <- PB0
 * ADC1 <- IN9 <- PB1
 */
#if 3

#define RHEOSTAT_NOFCHANEL      3

/*=====================通道1 IO======================*/
// PC3 通过调帽接电位器
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT1             ADC1_CH4_PA4_GPIO_Port
#define RHEOSTAT_ADC_GPIO_PIN1              ADC1_CH4_PA4_Pin
#define RHEOSTAT_ADC_GPIO_CLK1_ENABLE()     __GPIOA_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL1               ADC_CHANNEL_4
/*=====================通道2 IO ======================*/
// PA4 通过调帽接光敏电阻
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT2             ADC1_CH8_PB0_GPIO_Port
#define RHEOSTAT_ADC_GPIO_PIN2              ADC1_CH8_PB0_Pin
#define RHEOSTAT_ADC_GPIO_CLK2_ENABLE()     __GPIOB_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL2               ADC_CHANNEL_8
/*=====================通道3 IO ======================*/
// PA6 悬空，可用杜邦线接3V3或者GND来实验
// ADC IO宏定义
#define RHEOSTAT_ADC_GPIO_PORT3             ADC1_CH9_PB1_GPIO_Port
#define RHEOSTAT_ADC_GPIO_PIN3              ADC1_CH9_PB1_Pin
#define RHEOSTAT_ADC_GPIO_CLK3_ENABLE()     __GPIOB_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL3               ADC_CHANNEL_9
   
// ADC 序号宏定义
#define RHEOSTAT_ADC                        ADC1
#define RHEOSTAT_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR                ((uint32_t)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK_ENABLE()       __DMA2_CLK_ENABLE()
#define RHEOSTAT_ADC_DMA_CHANNEL            DMA_CHANNEL_0
#define RHEOSTAT_ADC_DMA_STREAM             DMA2_Stream0

/* ADC单通道/多通道 DMA采集 DMA通道相关 定义
 * 注意: 这里我们的通道还是使用上面的定义.
 */
#define ADC_ADCX_DMASx_IRQn                 DMA2_Stream0_IRQn
#define ADC_ADCX_DMASx_IRQHandler           DMA2_Stream0_IRQHandler

#define ADC_DMA_BUF_SIZE        50                                  /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */
#define ADC_CH_NUM              RHEOSTAT_NOFCHANEL                  /* ADC 通道数量 */
extern uint16_t g_adc_dma_buf[ADC_CH_NUM * ADC_DMA_BUF_SIZE];       /* ADC DMA BUF */
extern uint16_t g_adc_val[ADC_CH_NUM];                              /* ADC平均值存放数组 */
extern ADC_HandleTypeDef ADC_Handle;                                /* ADC句柄 */

/******************************************************************************************/

void adc_init(void);                                                                                /* ADC初始化 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime);    /* ADC通道设置 */

void adc_dma_init(void);                                                                            /* ADC DMA采集初始化 */
void calc_adc_val(uint16_t * p, uint16_t * buf , uint16_t ch_num, uint16_t len);                    /* 计算ADC平均值 */

#endif


#endif
