#include "my_motor.h"

#if 1

_bldc_obj g_bldc_motor1 = {STOP,0,0,CCW,0,0,0,0,0,0};   /* 电机结构体初始值 */
/**
 * @brief       无刷电机初始化，包括定时器，霍尔接口以及SD引脚初始化
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void bldc_init(uint16_t arr, uint16_t psc)
{       
    GPIO_InitTypeDef gpio_init_struct;
    
    SHUTDOWN_PIN_GPIO_CLK_ENABLE();
  
    gpio_init_struct.Pin = SHUTDOWN_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SHUTDOWN_PIN_GPIO, &gpio_init_struct);    
    
    hall_gpio_init();                       /* 霍尔接口初始化 */
    atim_timx_oc_chy_init(arr,  psc);       /* 定时器初始化 */
}

/**
 * @brief       BLDC控制函数
 * @param       dir :电机方向, Duty:PWM占空比
 * @retval      无
 */
void bldc_ctrl(uint8_t motor_id,int32_t dir,float duty)
{
    if(motor_id == MOTOR_1)
    {
        g_bldc_motor1.dir = dir;            /* 方向 */
        g_bldc_motor1.pwm_duty = duty;      /* 占空比 */
    }
}

#endif


#if 9
volatile uint32_t time = 0;
volatile int32_t compare_value = 400;
volatile int32_t max_value = MAX_PWM_DUTY/2;
volatile int32_t step = 800;
volatile int32_t direction = 1;
volatile int32_t sign = 0;
void my_motor_self_test(void){

    quick_start_motor1(compare_value);
    while(1){
        printf("[%d]",time);
        if(compare_value <= -max_value){
            direction = 1;
        }

        if(compare_value >= max_value){
            direction = -1;
        }

        compare_value += direction * step;
        if(compare_value * sign <= 0){
            quick_stop_motor1();
            delay_ms(10);
        }
        sign = SIGN(compare_value);
        g_bldc_motor1.dir = sign > 0 ? CW : CCW;    /* 通过判断正负号设置旋转方向 */
        quick_start_motor1(sign * compare_value);

        time++;
        if(time >= 30){
            printf("\r\n");
            stop_motor1();                          /* 停机 */
            g_bldc_motor1.run_flag = STOP;          /* 标记停机 */
            compare_value = 0;                      /* 数据清0 */
            g_bldc_motor1.pwm_duty = 0;
            break;
        }
        my_led_self_test();
        delay_ms(1000);
    }
}
#endif
