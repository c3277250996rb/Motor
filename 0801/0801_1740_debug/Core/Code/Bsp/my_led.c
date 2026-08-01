#include "my_led.h"


#ifndef __LED__Struct__Reference__
#define __LED__Struct__Reference__

    #include "main.h"
    #if defined(__STM32F1XX_H)
    // F103
    #elif defined(__STM32L1XX_H)
    // L151
    #elif defined(__STM32F4xx_H)
    // F407
    
        #ifndef LED_RED_PF6_Pin
            #define LED_RED_PF6_Pin GPIO_PIN_6
            #define LED_RED_PF6_GPIO_Port GPIOF
        #endif

        LED_Class_Struct LED_Class_RED = {
            .Pin = LED_RED_PE0_Pin,
            .Port = LED_RED_PE0_GPIO_Port,
            .ON_Status = 0,
            .OFF_Status = 1,
        };

    #else
        #error "啊? MCU不是F103, L151或者F407吗? 这..."
    #endif

#endif // __LED__Struct__Reference__



#ifndef __LED__APP__Reference__
#define __LED__APP__Reference__

    GPIO_PinState LED_APP_Read(LED_Class_Struct *input_LED_Class){
        return HAL_GPIO_ReadPin(
            input_LED_Class->Port,
            input_LED_Class->Pin
        );
    }

    void LED_APP_ON(LED_Class_Struct *input_LED_Class){
        HAL_GPIO_WritePin(
            input_LED_Class->Port,
            input_LED_Class->Pin,
            input_LED_Class->ON_Status
        );
    }

    void LED_APP_OFF(LED_Class_Struct *input_LED_Class){
        HAL_GPIO_WritePin(
            input_LED_Class->Port,
            input_LED_Class->Pin,
            input_LED_Class->OFF_Status
        );
    }

    void LED_APP_Toggle(LED_Class_Struct *input_LED_Class){
        HAL_GPIO_TogglePin(
            input_LED_Class->Port,
            input_LED_Class->Pin
        );
    }

#endif // __LED__APP__Reference__



void my_led_self_test(void){
    // LED_APP_OFF(&LED_Class_RED);
    // delay_ms(500);
    // LED_APP_ON(&LED_Class_RED);
    // delay_ms(1000);
    LED_APP_Toggle(&LED_Class_RED);
}





