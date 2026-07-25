#include "my_led.h"
#include "my_tim.h"

TIM_HandleTypeDef TIM1_Handler;
TIM_OC_InitTypeDef sConfigOC;
GPIO_InitTypeDef gpio_init_struct;

/**
  * @brief TIM Initialization Function
  * 1. 开启时钟
  * 
  * 2.1 实例
  * 2.2 预分频系数
  * 2.3 计数方向
  * 2.4 重装载值
  * 2.5 启用自动重装载
  * 
  * 3. Base_Init
  * 4. 设置中断优先级
  * 5. 启用中断
  * 6. Base_Start_IT
  */
void MY_TIM1_Init(void)
{
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    TIM1_Handler.Instance = TIM1;
    TIM1_Handler.Init.Prescaler = 84-1;   // T = 
    TIM1_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM1_Handler.Init.Period = 500-1;
    TIM1_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_PWM_Init(&TIM1_Handler);
    sConfigOC.OCMode = TIM_OCMODE_PWM2;
    sConfigOC.Pulse = (TIM1_Handler.Init.Period + 1) / 2;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&TIM1_Handler, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM1_Handler, TIM_CHANNEL_1);

    gpio_init_struct.Pin = TIM1_PWM_PE9_Pin;/* GPIO 口 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP; /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF1_TIM1; /* 端口复用 */
    HAL_GPIO_Init(TIM1_PWM_PE9_GPIO_Port, &gpio_init_struct);

}

uint8_t time = 0;
uint16_t direction = 1;
uint16_t compare_value = 0;
void my_pwm_self_test(){

    while(1){
        compare_value += direction;

        if(compare_value > 300){
            direction = -1; //超过比较值时递减
        }
        if(compare_value == 0){
            direction = 1;  //减到0时递增
        }
        
        /* 修改比较值控制占空比 */
        __HAL_TIM_SET_COMPARE(&TIM1_Handler, TIM_CHANNEL_1, compare_value);

        time++;
        if(time > 50){
            time = 0;
            my_led_self_test(); /* 控制 LED0 闪烁, 提示程序运行状态 */
        }
        delay_ms(10);
    }

}
