#include "my_led.h"
#include "my_tim.h"

TIM_HandleTypeDef TIM14_Handler;

//定时器中断服务程序	 
void TIM8_TRG_COM_TIM14_IRQHandler(void){ 		    		  			       
    if(__HAL_TIM_GET_FLAG(&TIM14_Handler, TIM_FLAG_UPDATE) != RESET)//溢出中断
    {
        my_led_self_test();
        __HAL_TIM_CLEAR_IT(&TIM14_Handler, TIM_IT_UPDATE);//清除中断标志位
    }
}


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
void MY_TIM14_Init(void)
{
    __HAL_RCC_TIM14_CLK_ENABLE();
    TIM14_Handler.Instance = TIM14;
    TIM14_Handler.Init.Prescaler = 8400-1;   // T = 5k / ((168M = 168kk)/8.4k) = 1s
    TIM14_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM14_Handler.Init.Period = 5000-1;
    TIM14_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&TIM14_Handler) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn,1,3);
    HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
    HAL_TIM_Base_Start_IT(&TIM14_Handler);
}
