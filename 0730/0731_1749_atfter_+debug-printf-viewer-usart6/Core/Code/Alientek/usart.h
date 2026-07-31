/**
 ******************************************************************************
 * @file    usart.h
 * @brief   串口.
 ******************************************************************************
 * 
 *-----------------------------------------------------------------------------
 * @brief   1.初始化串口.
 * @param   uart_init
 *  param   bound: 波特率
 *                 (正点原子一般用 115200).
 *-----------------------------------------------------------------------------
 * 
 */


#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "main.h"


#include "main.h"
#if defined(__STM32F1XX_H)
    // F103
    #include "stm32f1xx_hal_uart.h"
#elif defined(__STM32L1XX_H)
    // L151
    #include "stm32l1xx_hal_uart.h"
#elif defined(__STM32F4xx_H)
    // F407
    #include "stm32f4xx_hal_uart.h"
#else
    #error "啊? MCU不是F103, L151或者F407吗? 这..."
#endif



// 串口
#if 1
#define USART_REC_LEN  			200  		//定义最大接收字节数 200
#define EN_USART1_RX 			1			//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         			//接收状态标记	
extern UART_HandleTypeDef UART1_Handler; 	//UART句柄

#define RXBUFFERSIZE   1 					//缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];			//HAL库USART接收Buffer

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);

#endif
// 串口


#endif

