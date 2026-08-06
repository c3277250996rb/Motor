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


#if !9

extern int16_t adc_amp_un[3];
extern float  adc_amp_bus;

void my_motor_self_test(void){
    uint32_t time = 0;
    int32_t compare_value = 400;
    int32_t max_value = MAX_PWM_DUTY/2;
    int32_t step = 800;
    int32_t direction = 1;
    int32_t sign = 1;
    
    float current[3]= {0.0f};
    float current_lpf[4]= {0.0f};

	delay_ms(500);
    quick_start_motor1(compare_value);
	delay_ms(500);
    while(1){
        current[0] = adc_amp_un[0]* ADC2CURT;               /* 计算出三相电流值，U */
        current[1] = adc_amp_un[1]* ADC2CURT;               /* 计算出三相电流值，V */
        current[2] = adc_amp_un[2]* ADC2CURT;               /* 计算出三相电流值，W */
        
        /*一阶数字滤波 滤波系数0.1 用于显示*/
        FirstOrderRC_LPF(current_lpf[0],current[0],0.1f);   /* U相电流 */
        FirstOrderRC_LPF(current_lpf[1],current[1],0.1f);   /* V相电流 */
        FirstOrderRC_LPF(current_lpf[2],current[2],0.1f);   /* W相电流 */
        FirstOrderRC_LPF(current_lpf[3],adc_amp_bus,0.1f);  /* 母线电流 */
        
        printf("[%d]  Valtage:%.1fV  Temp:%.1fC  U_A:%.3fmA  V_A:%.3fmA  W_A:%.3fmA  Root_A:%.3f\r\n", \
            time, g_adc_val[0]*ADC2VBUS, get_temp(g_adc_val[1]), current_lpf[0], current_lpf[1], current_lpf[2], current_lpf[3]);
        
        if(compare_value <= -max_value){
            direction = 1;
        }

        if(compare_value >= max_value){
            direction = -1;
        }

        // compare_value += direction * step;
        if(compare_value * sign <= 0){
            quick_stop_motor1();
            delay_ms(10);
        }
        sign = SIGN(compare_value);
        g_bldc_motor1.dir = sign > 0 ? CW : CCW;    /* 通过判断正负号设置旋转方向 */
        quick_start_motor1(compare_value);

        time++;
        if(time >= 600){
            printf("\r\n");
            quick_stop_motor1();
            break;
        }
        my_led_self_test();
        delay_ms(100);
    }
}
#endif

#if 9

extern int16_t adc_amp_un[3];
extern float  adc_amp_bus;
#include "my_adc.h"
void my_motor_self_test(void)
{
    uint8_t key,t;
    char buf[32];
    int16_t pwm_duty_temp=1000;
    float current[3]= {0.0f};
    float current_lpf[4]= {0.0f};

    printf("按下KEY0 增加比较值即正转加速\r\n");
    printf("按下KEY1 减少比较值即反转加速\r\n");
    printf("按下KEY2 停止电机\r\n");

	delay_ms(100);
        {
            // pwm_duty_temp += 500;
            if(pwm_duty_temp >= MAX_PWM_DUTY)
                pwm_duty_temp = MAX_PWM_DUTY;
            if(pwm_duty_temp > 0)
            {
                g_bldc_motor1.pwm_duty = pwm_duty_temp;/* 设置占空比 */
                g_bldc_motor1.dir = CW;                /* 设置方向 */
            }
            else
            {
                g_bldc_motor1.pwm_duty = -pwm_duty_temp;
                g_bldc_motor1.dir = CCW;
            }
            g_bldc_motor1.run_flag = RUN;               /* 运行标志 */
            start_motor1();                             /* 开启运行*/
        }
    while (1)
    {
        t++;
        if(t % 20 == 0)
        {
            current[0] = adc_amp_un[0]* ADC2CURT;               /* 计算出三相电流值，U */
            current[1] = adc_amp_un[1]* ADC2CURT;               /* 计算出三相电流值，V */
            current[2] = adc_amp_un[2]* ADC2CURT;               /* 计算出三相电流值，W */
            
            /*一阶数字滤波 滤波系数0.1 用于显示*/
            FirstOrderRC_LPF(current_lpf[0],current[0],0.1f);   /* U相电流 */
            FirstOrderRC_LPF(current_lpf[1],current[1],0.1f);   /* V相电流 */
            FirstOrderRC_LPF(current_lpf[2],current[2],0.1f);   /* W相电流 */
            FirstOrderRC_LPF(current_lpf[3],adc_amp_bus,0.1f);  /* 母线电流 */
            
            if(g_bldc_motor1.run_flag == STOP)                  /* 停机的电流显示 */
            {
                current_lpf[0] = 0;
                current_lpf[1] = 0;
                current_lpf[2] = 0;
                current_lpf[3] = 0;
            }
            
            /* 串口打印信息 */
            printf("Valtage:%.1fV \r\n", g_adc_val[0]*ADC2VBUS);
            printf("Temp:%.1fC \r\n", get_temp(g_adc_val[1]));
            printf("U相电流为：%.3fmA\r\n", (current_lpf[0]));
            printf("V相电流为：%.3fmA\r\n", (current_lpf[1]));
            printf("W相电流为：%.3fmA\r\n", (current_lpf[2]));
            printf("母线电流为：%.3fmA\r\n", (current_lpf[3]));
            printf("\r\n");
        }

        // key = key_scan(0);
        // if(key == KEY0_PRES)                            /* 按下KEY0占空比++ */
		// if(t % 50 == 0)
        // else if(key == KEY1_PRES)                       /* 按下KEY1占空比-- */
        {
            // pwm_duty_temp -= 500;
            // if(pwm_duty_temp <= -MAX_PWM_DUTY)
            //     pwm_duty_temp = -MAX_PWM_DUTY;
            // if(pwm_duty_temp < 0)
            // {
            //     g_bldc_motor1.pwm_duty = -pwm_duty_temp;
            //     g_bldc_motor1.dir = CCW;
            // }
            // else
            // {
            //     g_bldc_motor1.pwm_duty = pwm_duty_temp;
            //     g_bldc_motor1.dir = CW;
            // }
            // g_bldc_motor1.run_flag = RUN;               /* 开启运行*/
            // start_motor1();                             /* 运行电机*/
        }
        // else if(key == KEY2_PRES)                       /* 按下KEY2关闭电机 */
        {
            // stop_motor1();                              /* 停机*/
            // g_bldc_motor1.run_flag = STOP;              /* 标记停机*/
            // pwm_duty_temp = 0;
            // g_bldc_motor1.pwm_duty = 0;
        }
        delay_ms(10);
    }
}


#endif
