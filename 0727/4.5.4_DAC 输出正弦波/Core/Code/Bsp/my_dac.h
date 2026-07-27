#ifndef __MY_DAC_H
#define __MY_DAC_H

#include "stm32f4xx.h"

/*  DAC普通输出  zat*/
/**
 * DAC -> OUT1 -> PA4
 * DAC -> OUT2 -> PA5
 */
#if 0

void dac_init(uint8_t outx);                        /* DAC初始化 */ 
void dac_set_voltage(uint8_t outx, uint16_t vol);   /* 设置输出电压 */ 

#endif

/*  DAC输出三角波函数  zat*/
#if 0

void dac_triangular_wave(uint16_t max_value, uint16_t dt, uint16_t samples, uint16_t n);

#endif

/*  DAC(TIM2更新触发输出,DMA搬运数据)输出正弦波  zat*/
#if 3

void dac_init(uint8_t outx,uint16_t cndtr);                      /* DAC、DMA初始化 */
void dac_creat_sin_buf(uint16_t maxval, uint16_t samples);       /* 产生正弦波序列 */

#endif


#if 9
void my_dac_self_test(void);
#endif

#endif
