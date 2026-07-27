#ifndef __MY_DAC_H
#define __MY_DAC_H

#include "stm32f4xx.h"

/*  DAC普通输出  zat*/
/**
 * DAC -> OUT1 -> PA4
 * DAC -> OUT2 -> PA5
 */
#if 1

void dac_init(uint8_t outx);                        /* DAC初始化 */ 
void dac_set_voltage(uint8_t outx, uint16_t vol);   /* 设置输出电压 */ 

#endif

/*  DAC输出三角波函数  zat*/
#if 2

void dac_triangular_wave(uint16_t max_value, uint16_t dt, uint16_t samples, uint16_t n);

#endif

#if 9
void my_dac_self_test(void);
#endif

#endif
