#include "my_dac.h"
#include "my_adc.h"

/*  DAC普通输出  zat*/
#if 1

DAC_HandleTypeDef g_dac_handle;         /* DAC句柄 */

/**
 * @brief       DAC初始化函数
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void dac_init(uint8_t outx)
{
    GPIO_InitTypeDef gpio_init_struct;
    DAC_ChannelConfTypeDef dac_ch_conf;
    
    __HAL_RCC_DAC_CLK_ENABLE();                                     /* 使能DAC1的时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                                   /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    
    gpio_init_struct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;      /* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG; 
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    g_dac_handle.Instance = DAC;
    HAL_DAC_Init(&g_dac_handle);                                    /* 初始化DAC */

    dac_ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;                     /* 不使用触发功能 */
    dac_ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;        /* DAC1输出缓冲关闭 */
    
    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&g_dac_handle,&dac_ch_conf,DAC_CHANNEL_1);    /* DAC通道1配置 */
            HAL_DAC_Start(&g_dac_handle,DAC_CHANNEL_1);                         /* 开启DAC通道1 */
            break;
        case 2:
            HAL_DAC_ConfigChannel(&g_dac_handle,&dac_ch_conf,DAC_CHANNEL_2);    /* DAC通道2配置 */
            HAL_DAC_Start(&g_dac_handle,DAC_CHANNEL_2);                         /* 开启DAC通道2 */
            break;
        default:break;
    }

}

/**
 * @brief       设置通道1/2输出电压
 * @param       outx: 1,通道1; 2,通道2
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;                                               /* 如果值大于等于4096, 则取4095 */

    if (outx == 1)      /* 通道1 */
    {
        HAL_DAC_SetValue(&g_dac_handle,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);     /* 12位右对齐数据格式设置DAC值 */
    }
    else                /* 通道2 */
    {
        HAL_DAC_SetValue(&g_dac_handle,DAC_CHANNEL_2,DAC_ALIGN_12B_R,temp);     /* 12位右对齐数据格式设置DAC值 */
    }
}

#endif

#if 9
void my_dac_self_test(void){
    uint16_t milli_voltage = 1000;
    int direction = 1;
    uint16_t step = 100;
    uint8_t time = 0;

    while(1){
        if((time % 10) == 0){
            my_adc_self_test(); /* 使用VOFA+查看三角波 */
        }

        if((time % 20) == 0){
            if(milli_voltage >= 3000){
                direction = -1;
            }
            if(milli_voltage <= 1000){
                direction = 1;
            }
            milli_voltage += direction * step;
            dac_set_voltage(1, milli_voltage);
        }

        time++;
        delay_ms(10);
    }
}
#endif


