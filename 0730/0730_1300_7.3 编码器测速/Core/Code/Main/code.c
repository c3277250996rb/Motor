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
    atim_timx_cplm_pwm_init(8400 - 1, 0);    /* 168 000 000 / 1 = 168 000 000 168Mhz的计数频率，计数8400次为50us */
    dcmotor_init();                          /* 初始化电机 */
    adc_nch_dma_init();                     /* ADC DMA传输初始化 */
    gtim_timx_encoder_chy_init(0XFFFF, 0);  /* 编码器定时器初始化，不分频直接84M的计数频率 */
    btim_timx_int_init(1000 - 1 , 84 - 1);  /* 基本定时器初始化，1ms计数周期 */
}

void CODE_SELF_TEST(void){
    while(1){
        // muti_printf_self_test();
        // my_led_self_test();
        my_motor_self_test();
        while(1);
    }
}

void CODE_WHILE(void){

    while(1){

    }
}











