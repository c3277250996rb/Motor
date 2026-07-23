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











