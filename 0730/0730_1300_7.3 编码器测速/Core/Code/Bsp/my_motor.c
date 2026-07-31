#include "my_motor.h"
#include "delay.h"
#include "my_led.h"
#include "my_adc.h"
#include "my_tim.h"

/*  BDC基础驱动  zat*/
#if 1

extern TIM_HandleTypeDef g_atimx_cplm_pwm_handle;                              /* 定时器x句柄 */

/**
 * @brief       电机初始化
 * @param       无
 * @retval      无
 */
void dcmotor_init(void)
{
    SHUTDOWN_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init_struct;
    
    /* SD引脚设置，设置为推挽输出 */
    gpio_init_struct.Pin = SHUTDOWN1_Pin|SHUTDOWN2_Pin;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SHUTDOWN1_GPIO_Port, &gpio_init_struct);
    
    HAL_GPIO_WritePin(GPIOF, SHUTDOWN1_Pin|SHUTDOWN2_Pin, GPIO_PIN_RESET);      /* SD拉低，关闭输出 */
    
    dcmotor_stop();                 /* 停止电机 */
    dcmotor_dir(0);                 /* 设置正转 */
    dcmotor_speed(0);               /* 速度设置为0 */
    dcmotor_start();                /* 开启电机 */
}

/**
 * @brief       电机开启
 * @param       无
 * @retval      无
 */
void dcmotor_start(void)
{
    ENABLE_MOTOR;                                                       /* 拉高SD引脚，开启电机 */
}

/**
 * @brief       电机停止
 * @param       无
 * @retval      无
 */
void dcmotor_stop(void)
{
    HAL_TIM_PWM_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);          /* 关闭主通道输出 */
    HAL_TIMEx_PWMN_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);       /* 关闭互补通道输出 */
    DISABLE_MOTOR;                                                      /* 拉低SD引脚，停止电机 */
}

/**
 * @brief       电机旋转方向设置
 * @param       para:方向 0正转，1反转
 * @note        以电机正面，顺时针方向旋转为正转
 * @retval      无
 */
void dcmotor_dir(uint8_t para)
{
    HAL_TIM_PWM_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);          /* 关闭主通道输出 */
    HAL_TIMEx_PWMN_Stop(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);       /* 关闭互补通道输出 */

    if (para == 0)                /* 正转 */
    {
        HAL_TIM_PWM_Start(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);     /* 开启主通道输出 */
    } 
    else if (para == 1)           /* 反转 */
    {
        HAL_TIMEx_PWMN_Start(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1);  /* 开启互补通道输出 */
    }
}

/**
 * @brief       电机速度设置
 * @param       para:比较寄存器值
 * @retval      无
 */
void dcmotor_speed(uint16_t para)
{
    if (para < (__HAL_TIM_GetAutoreload(&g_atimx_cplm_pwm_handle) - 0x0F))  /* 限速 */
    {  
        __HAL_TIM_SetCompare(&g_atimx_cplm_pwm_handle, TIM_CHANNEL_1, para);
    }
}

/**
 * @brief       电机控制
 * @param       para: pwm比较值 ,正数电机为正转，负数为反转
 * @note        根据传入的参数控制电机的转向和速度
 * @retval      无
 */
void motor_pwm_set(float para)
{
    int val = (int)para;

    if (val >= 0) 
    {
        dcmotor_dir(0);           /* 正转 */
        dcmotor_speed(val);
    } 
    else 
    {
        dcmotor_dir(1);           /* 反转 */
        dcmotor_speed(-val);
    }
}

#endif


#if !9
void my_motor_self_test(void){
    uint32_t time = 0;
    int32_t compare_value = 400;
    int32_t max_value = 4200;
    int32_t step = 400;
    int32_t direction = 1;

    while(1){
        printf("[%d]",time);
        if(compare_value <= -max_value){
            direction = 1;
        }

        if(compare_value >= max_value){
            direction = -1;
        }

        compare_value += direction * step;
        motor_pwm_set(compare_value);

        time++;
        if(time >= 30){
            printf("\r\n");
            dcmotor_stop();
            break;
        }
        my_led_self_test();
        delay_ms(1000);
    }
}
#endif
#if 9

void my_motor_self_test(void){
    uint32_t time = 0;
    int32_t compare_value = 400;
    int32_t max_value = 4200;
    int32_t step = 400;
    int32_t direction = 1;

    uint16_t init_adc_val;  /* init_adc_val存储电流测量对应的参考电压ADC值，这里进行滤波 */
    init_adc_val = g_adc_val[2];            /* 取出第一次得到的值 */
    for(int t=0;t<1000;t++)
    {
        init_adc_val += g_adc_val[2];       /* 现在的值和上一次存储的值相加 */
        init_adc_val /= 2;                  /* 取平均值 */
        delay_ms(1);
    }

    while(1){
        // printf("[%d]",time);
        printf("[%d]  Valtage: %.1fV  Temperature: %.1fC  Ampere: %.1f  RPM: %.1f\r\n",time, g_adc_val[0]*ADC2VBUS, get_temp(g_adc_val[1]), abs(g_adc_val[2]-init_adc_val)*ADC2CURT, g_motor_data.speed);
        
        if((time % 3) == 0){
            if(compare_value <= -max_value){
                direction = 1;
            }
            if(compare_value >= max_value){
                direction = -1;
            }
            compare_value += direction * step;
            motor_pwm_set(compare_value);
        }

        time++;
        if(time >= 30){
            printf("\r\n");
            dcmotor_stop();
            break;
        }
        my_led_self_test();
        delay_ms(1000);
    }
}

#endif
