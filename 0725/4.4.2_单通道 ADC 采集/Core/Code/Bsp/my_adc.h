#ifndef __MY_ADC_H
#define __MY_ADC_H

#include "stm32f4xx.h"

/*  ADC1(独立模式,单通道,中断)测量滑动变阻器电压  zat*/
/** adc的相关计算详见"f407_2026-07-25_ChatGPT_ADC单通道中断配置详解&计算方式.md"
 * ADC1 <- CH8 <- PB0 <- 滑动变阻器
 */
#if 1

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


#endif
