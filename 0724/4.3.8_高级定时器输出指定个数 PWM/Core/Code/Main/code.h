#ifndef __CODE_H
#define __CODE_H
#include "main.h"

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usmart.h"

#include "my_led.h"
#include "my_tim.h"

// template
#if 0

    #include "main.h"
    #if defined(__STM32F1XX_H)
        // F103
    #elif defined(__STM32L1XX_H)
        // L151
    #elif defined(__STM32F4xx_H)
        // F407
    #else
        #error "啊? MCU不是F103, L151或者F407吗? 这..."
    #endif

#endif
// template


// configure
#if 1

    #if defined(__STM32F1XX_H)
        #define DEVICE_STRING "XX-F103-XX"
    #elif defined(__STM32L1XX_H)
        #define DEVICE_STRING "四季-L151-核心"
    #elif defined(__STM32F4xx_H)
        /* power on */
        #define DEVICE_STRING "野火-F407-霸天虎"
        /* delay & usmart */
            #define di_sysclk 168
            #define ui_sysclk 84
    #else
        #error "啊? MCU不是F103, L151或者F407吗? 这..."
    #endif

    #define POWER_ON() { printf("\r\n启动成功!\r\n"); printf("%s\r\n", DEVICE_STRING); }

#endif
// configure


// ikkun
#if 1
#define ikkun() { \
    printf("\r\nHardFault_Handler: 硬件错误辣.\r\n"); \
    printf("尝试重启...\r\n"); \
    char kun[] = "\
                                 &BGG#&     \r\n\
                                P55PPP5G&   \r\n\
                      &##BB5?B#BGBBG5GGGP&  \r\n\
                     #BBBBBG^PBBBGB57J5PP&  \r\n\
                   #B##BBBB#!J#BBBBBYJPGB   \r\n\
                 &###&#BBBB#J!BB###BB#      \r\n\
                 ###&5GBBBB#P!PBPB###B&     \r\n\
                 &###GBBB###75##BY?5B##     \r\n\
                  &#######&P~#####GJ7YB     \r\n\
                    ######&?Y&#####&GJP     \r\n\
                  &#PY5GBBBJB######&&#&     \r\n\
                  PJ??JJJYYY55PGB####       \r\n\
               &GY??JJJYYYYYYYYYY5G&        \r\n\
             &PJ???JJJYYYYYYYYYYYYJ5&       \r\n\
            BJ?JJJJYYY55PPP555555YYJB       \r\n\
          &Y?JYYYY5PGB#&    #G555YYYJ#      \r\n\
          Y7J55PG#&           &G5555YY&     \r\n\
         #7?JYG                 #GPPY?J&    \r\n\
         G7?JY#                   #YJ??P    \r\n\
         5?JJ5                     GJ??J&   \r\n\
         Y?JY&                     &Y???G   \r\n\
        &J?JG                       &Y??5   \r\n\
        &PGB                         #55P   \r\n\
        BGPB                          #GPB  \r\n\
      &BGBGPB                         #PGBB&\r\n\
    "; \
    printf("%s", &kun); \
    HAL_NVIC_SystemReset();\
}

#endif
// ikkun


void CODE_MAIN(void);
void CODE_INIT(void);
void CODE_SELF_TEST(void);
void CODE_WHILE(void);


#endif

