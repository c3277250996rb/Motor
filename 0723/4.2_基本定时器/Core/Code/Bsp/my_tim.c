#include "my_led.h"
#include "my_tim.h"

TIM_HandleTypeDef TIM6_Handler;

//定时器6中断服务程序	 
void TIM6_DAC_IRQHandler(void){ 		    		  			       
    if(__HAL_TIM_GET_FLAG(&TIM6_Handler,TIM_FLAG_UPDATE) && __HAL_TIM_GET_IT_SOURCE(&TIM6_Handler,TIM_IT_UPDATE))//溢出中断
    {
        my_led_self_test();
        __HAL_TIM_CLEAR_IT(&TIM6_Handler, TIM_IT_UPDATE);//清除中断标志位
    }
}


/**
  * @brief TIM6 Initialization Function
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

void MY_TIM6_Init(void)
{
    __HAL_RCC_TIM6_CLK_ENABLE();
    TIM6_Handler.Instance = TIM6;
    TIM6_Handler.Init.Prescaler = 8400-1;   // T = 5k / ((84M = 84kk)/8.4k) = 0.5s
    TIM6_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM6_Handler.Init.Period = 5000-1;
    TIM6_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&TIM6_Handler) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(TIM6_DAC_IRQn,1,3);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    HAL_TIM_Base_Start_IT(&TIM6_Handler);
}
