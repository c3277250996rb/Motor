/*金米妮金米妮金米妮金米妮金米妮金米妮金米妮金米妮金米妮金米妮*/
#include "code.h"

void CODE_MAIN(void){
    CODE_INIT();
    POWER_ON();
    CODE_SELF_TEST();
    CODE_WHILE();
}


void CODE_INIT(void){
	delay_init(di_sysclk);      //初始化延时函数
    uart_init(115200);          //初始化串口
    usmart_init(ui_sysclk);     //初始化USMART
    TIMx_Configuration();
    // atim_timx_cplm_pwm_init(1000 - 1, 168 - 1);/* 1Mhz 的计数频率 1Khz 的周期 */
    // atim_timx_cplm_pwm_set(300, 100); /* 占空比:7:3 死区时间 100*tDTS */

    atim_timx_comp_pwm_init(1000 - 1, 168 - 1); /* 1Mhz 的计数频率，计数 1000 次 */
    ATIM_TIMX_COMP_CH1_CCRX = 250 - 1; /* 通道 1 相位 25% */
    ATIM_TIMX_COMP_CH2_CCRX = 500 - 1; /* 通道 2 相位 50% */
    ATIM_TIMX_COMP_CH3_CCRX = 750 - 1; /* 通道 3 相位 75% */
}

void CODE_SELF_TEST(void){
    while(1){
        // muti_printf_self_test();
        // my_led_self_test();
        my_pwm_self_test();
        // my_test();
        while(1);
    }
}

void CODE_WHILE(void){

    while(1){

    }
}











