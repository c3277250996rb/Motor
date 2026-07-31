#include "code.h"


#ifndef __LED__Struct__Definition__
#define __LED__Struct__Definition__
    typedef struct __LED_Class_Struct
    {
        uint16_t Pin;
        GPIO_TypeDef* Port;
        uint8_t ON_Status;
        uint8_t OFF_Status;
    }LED_Class_Struct;

    extern LED_Class_Struct LED_Class_RED;
#endif // __LED__Struct__Definition__


#ifndef __LED__APP__Definition__
#define __LED__APP__Definition__

    GPIO_PinState LED_APP_Read(LED_Class_Struct *input_LED_Class);
    void LED_APP_ON(LED_Class_Struct *input_LED_Class);
    void LED_APP_OFF(LED_Class_Struct *input_LED_Class);
    void LED_APP_Toggle(LED_Class_Struct *input_LED_Class);
    
#endif // __LED__APP__Definition__





