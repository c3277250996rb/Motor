#include "my_motor.h"

#if 0

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


#if 2
/**
****************************************************************************************************
* @file        bldc.c
* @author      正点原子团队(ALIENTEK)
* @version     V1.0
* @date        2021-10-14
* @brief       BLDC 驱动代码
* @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
****************************************************************************************************
* @attention
*
* 实验平台:正点原子 F407电机开发板
* 在线视频:www.yuanzige.com
* 技术论坛:www.openedv.com
* 公司网址:www.alientek.com
* 购买地址:openedv.taobao.com
*
* 修改说明
* V1.0 20211014
* 第一次发布
*
****************************************************************************************************
*/

#include "my_motor.h"
#include "my_tim.h"
#include "my_adc.h"
#include "math.h"

_bldc_obj g_bldc_motor1 = {STOP,0,0,CCW,0,0,0,0,0,0};   /* 电机结构体 */

const uint8_t hall_table_cw[6] = {6,2,3,1,5,4};         /* 顺时针旋转表 */
const uint8_t hall_table_ccw[6] = {5,1,3,2,6,4};        /* 逆时针旋转表 */

const uint8_t hall_cw_table[12] = {0x62,0x23,0x31,0x15,0x54,0x46,0x63,0x21,0x35,0x14,0x56,0x42};
const uint8_t hall_ccw_table[12] = {0x45,0x51,0x13,0x32,0x26,0x64,0x41,0x53,0x12,0x36,0x24,0x65};
/**
* @brief       无刷电机初始化，包括定时器，编码器以及SD引脚初始化
* @param       arr: 自动重装值
* @param       psc: 时钟预分频数
* @retval      无
*/
void bldc_init(uint16_t arr, uint16_t psc)
{       
   GPIO_InitTypeDef gpio_init_struct;
   
   SHUTDOWN_PIN_GPIO_CLK_ENABLE();
   SHUTDOWN2_PIN_GPIO_CLK_ENABLE();
 
   gpio_init_struct.Pin = SHUTDOWN_PIN;
   gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
   gpio_init_struct.Pull = GPIO_NOPULL;
   gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(SHUTDOWN_PIN_GPIO, &gpio_init_struct);    
   
   gpio_init_struct.Pin = SHUTDOWN2_PIN;
   HAL_GPIO_Init(SHUTDOWN2_PIN_GPIO, &gpio_init_struct);  
   hall_gpio_init();                   /* 霍尔接口初始化 */
   atim_timx_oc_chy_init(arr,  psc);   /* 定时器初始化 */
   btim_timx_int_init(1000 - 1 , 84 - 1);
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
/**
* @brief       方向检测函数
* @param       obj ： 电机控制句柄
* @retval      res ： 旋转方向
*/
uint8_t check_hall_dir(_bldc_obj * obj)
{
   uint8_t temp,res = HALL_ERROR;
   if((obj->step_last <= 6)&&(obj->step_sta <= 6))
   {
       temp = ((obj->step_last & 0x0F) << 4)|(obj->step_sta & 0x0F);
       if((temp == hall_ccw_table[0])||(temp == hall_ccw_table[1])||\
               (temp == hall_ccw_table[2])||(temp == hall_ccw_table[3])||\
               (temp == hall_ccw_table[4])||(temp == hall_ccw_table[5]))
       {
           res  = CCW;
       }
       else if((temp == hall_cw_table[0])||(temp == hall_cw_table[1])||\
               (temp == hall_cw_table[2])||(temp == hall_cw_table[3])||\
               (temp == hall_cw_table[4])||(temp == hall_cw_table[5]))
       {
           res  = CW;
       }
   }
   return res;
}
/******************************************* 霍尔接口初始化 **********************************************/

/**
 * @brief  霍尔传感器接口初始化
 * @param  无
 * @retval 无
 */
void hall_gpio_init(void)
{
   GPIO_InitTypeDef gpio_init_struct;
   HALL1_U_GPIO_CLK_ENABLE();
   HALL1_V_GPIO_CLK_ENABLE();
   HALL1_W_GPIO_CLK_ENABLE();

   HALL2_U_GPIO_CLK_ENABLE();
   HALL2_V_GPIO_CLK_ENABLE();
   HALL2_W_GPIO_CLK_ENABLE();

   /* 霍尔通道 1 引脚初始化 */
   gpio_init_struct.Pin = HALL1_TIM_CH1_PIN;
   gpio_init_struct.Mode = GPIO_MODE_INPUT;
   gpio_init_struct.Pull = GPIO_PULLUP;
   HAL_GPIO_Init(HALL1_TIM_CH1_GPIO, &gpio_init_struct);

   /* 霍尔通道 2 引脚初始化 */
   gpio_init_struct.Pin = HALL1_TIM_CH2_PIN;
   HAL_GPIO_Init(HALL1_TIM_CH2_GPIO, &gpio_init_struct);

   /* 霍尔通道 3 引脚初始化 */
   gpio_init_struct.Pin = HALL1_TIM_CH3_PIN;
   HAL_GPIO_Init(HALL1_TIM_CH3_GPIO, &gpio_init_struct);
}

/**
* @brief       获取霍尔传感器引脚状态
* @param       motor_id ： 无刷接口编号
* @retval      霍尔传感器引脚状态
*/
uint32_t hallsensor_get_state(uint8_t motor_id)
{
   __IO static uint32_t state ;
   state  = 0;
   if(motor_id == MOTOR_1)
   {
       if(HAL_GPIO_ReadPin(HALL1_TIM_CH1_GPIO,HALL1_TIM_CH1_PIN) != GPIO_PIN_RESET)  /* 霍尔传感器状态获取 */
       {
           state |= 0x01U;
       }
       if(HAL_GPIO_ReadPin(HALL1_TIM_CH2_GPIO,HALL1_TIM_CH2_PIN) != GPIO_PIN_RESET)  /* 霍尔传感器状态获取 */
       {
           state |= 0x02U;
       }
       if(HAL_GPIO_ReadPin(HALL1_TIM_CH3_GPIO,HALL1_TIM_CH3_PIN) != GPIO_PIN_RESET)  /* 霍尔传感器状态获取 */
       {
           state |= 0x04U;
           g_bldc_motor1.hall_single_sta = 1;                                        /* 单个霍尔状态，计算速度用到 */
       }
       else
           g_bldc_motor1.hall_single_sta = 0;
   }
   return state;
}

/************************************* BLDC相关函数 *************************************/

/**
 * @brief  关闭电机运转
 * @param  无
 * @retval 无
 */
void stop_motor1(void)
{
   /* 关闭半桥芯片输出 */
   SHUTDOWN_OFF;
   /* 关闭PWM输出 */
   HAL_TIM_PWM_Stop(&g_atimx_handle,TIM_CHANNEL_1);
   HAL_TIM_PWM_Stop(&g_atimx_handle,TIM_CHANNEL_2);
   HAL_TIM_PWM_Stop(&g_atimx_handle,TIM_CHANNEL_3);
   /* 上下桥臂全部关断 */
   g_atimx_handle.Instance->CCR2 = 0;
   g_atimx_handle.Instance->CCR1 = 0;
   g_atimx_handle.Instance->CCR3 = 0;
   HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}

/**
 * @brief  开启电机运转
 * @param  无
 * @retval 无
 */
void start_motor1(void)
{
   SHUTDOWN_EN;
   /* 使能PWM输出 */
   HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_2);
   HAL_TIM_PWM_Start(&g_atimx_handle,TIM_CHANNEL_3);
}

/*************************** 上下桥臂的导通情况，共6种，也称为6步换向（接口一） ****************************/ 

/*  六步换向函数指针 */
pctr pfunclist_m1[6] =
{
   &m1_uhwl, &m1_vhul, &m1_vhwl,
   &m1_whvl, &m1_uhvl, &m1_whul
};

/* 上下桥臂的导通情况，共6种，也称为6步换向 */
void m1_uhvl(void)
{
   g_atimx_handle.Instance->CCR2 = 0;
   g_atimx_handle.Instance->CCR1 = g_bldc_motor1.pwm_duty;                 /* U相上桥臂PWM */
   g_atimx_handle.Instance->CCR3 = 0;
   HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_SET);   /* V相下桥臂导通 */
   HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET); /* U相下桥臂关闭 */
   HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET); /* W相下桥臂关闭 */
}

void m1_uhwl(void)
{
   g_atimx_handle.Instance->CCR2 = 0;
   g_atimx_handle.Instance->CCR1 = g_bldc_motor1.pwm_duty;
   g_atimx_handle.Instance->CCR3 = 0;
   HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_SET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
}

void m1_vhwl(void)
{
   g_atimx_handle.Instance->CCR1=0;
   g_atimx_handle.Instance->CCR2 = g_bldc_motor1.pwm_duty;
   g_atimx_handle.Instance->CCR3=0;
   HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_SET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
}

void m1_vhul(void)
{
   g_atimx_handle.Instance->CCR1 = 0;
   g_atimx_handle.Instance->CCR2 = g_bldc_motor1.pwm_duty;
   g_atimx_handle.Instance->CCR3 = 0;
   HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_SET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}


void m1_whul(void)
{
   g_atimx_handle.Instance->CCR2 = 0;
   g_atimx_handle.Instance->CCR3 = g_bldc_motor1.pwm_duty;
   g_atimx_handle.Instance->CCR1 = 0;
   HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_SET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}

void m1_whvl(void)
{
   g_atimx_handle.Instance->CCR2 = 0;
   g_atimx_handle.Instance->CCR3 = g_bldc_motor1.pwm_duty;
   g_atimx_handle.Instance->CCR1 = 0;
   HAL_GPIO_WritePin(M1_LOW_SIDE_V_PORT,M1_LOW_SIDE_V_PIN,GPIO_PIN_SET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_U_PORT,M1_LOW_SIDE_U_PIN,GPIO_PIN_RESET);
   HAL_GPIO_WritePin(M1_LOW_SIDE_W_PORT,M1_LOW_SIDE_W_PIN,GPIO_PIN_RESET);
}

/**
* @brief       检测输入信号是否发生变化
* @param       val :输入信号
* @note        测量速度使用，获取输入信号状态翻转情况，计算速度
* @retval      0：计算高电平时间，1：计算低电平时间，2：信号未改变
*/
uint8_t uemf_edge(uint8_t val)
{
   /* 主要是检测val信号从0 - 1 在从 1 - 0的过程，即高电平所持续的过程 */
   static uint8_t oldval=0;
   if(oldval != val)
   {
       oldval = val;
       if(val == 0) return 0;
       else return 1;
   }
   return 2;
}

/*************************************    第二部分    电压电流温度采集    **********************************************/
/*
   Rt = Rp *exp(B*(1/T1-1/T2))

   Rt 是热敏电阻在T1温度下的阻值；
   Rp是热敏电阻在T2常温下的标称阻值；
   exp是e的n次方，e是自然常数，就是自然对数的底数，近似等于 2.7182818；
   B值是热敏电阻的重要参数，教程中用到的热敏电阻B值为3380；
   这里T1和T2指的是开尔文温度，T2是常温25℃，即(273.15+25)K
   T1就是所求的温度
*/
const float Rp = 10000.0f;          /* 10K */
const float T2 = (273.15f + 25.0f); /* T2 */
const float Bx = 3380.0f;           /* B */
const float Ka = 273.15f;

/**
* @brief       计算温度值
* @param       para: 温度采集对应ADC通道的值（已滤波）
* @note        计算温度分为两步：
               1.根据ADC采集到的值计算当前对应的Rt
               2.根据Rt计算对应的温度值
* @retval      温度值
*/
float get_temp(uint16_t para)
{
   float Rt;
   float temp;
   Rt = 3.3f / (para * 3.3f / 4096.0f / 4700.0f) - 4700.0f;
   /* like this R=5000, T2=273.15+25,B=3470, RT=5000*EXP(3470*(1/T1-1/(273.15+25)) */
   temp = Rt / Rp;
   temp = log(temp);       /* ln(Rt/Rp) */
   temp /= Bx;             /* ln(Rt/Rp)/B */
   temp += (1.0f / T2);
   temp = 1.0f / (temp);
   temp -= Ka;
   return temp;
}

extern uint16_t g_adc_value[ADC_CH_NUM * ADC_COLL];

/**
* @brief       计算ADC的平均值（滤波）
* @param       * p ：存放ADC值的指针地址
* @note        此函数对电压、温度、电流对应的ADC值进行滤波
* @retval      无
*/
void calc_adc_val(uint16_t * p)
{
   uint32_t temp[ADC_CH_NUM] = {0,0,0};             /* 定义一个缓存数组 */
   int i,j;                                         /* 循环采集ADC_COLL次数 */
   for(i=0;i<ADC_COLL;i++)                          /* 根据ADC通道数循环获取，并累加 */
   {
       for(j=0;j<ADC_CH_NUM;j++)                    /* 将采集到的ADC值，各通道进行累加 */
       {
           temp[j] += g_adc_value[j+i*ADC_CH_NUM];
       }
   }
   for(j=0;j<ADC_CH_NUM;j++)
   {
       temp[j] /= ADC_COLL;                         /* 获取平均值 */
       p[j] = temp[j];                              /* 存到*p */
   }
}


#endif


#if !9
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
#if !9

extern int16_t adc_amp_un[3];
extern float  adc_amp_bus;
#include "my_adc.h"
float current_lpf[4]= {0.0f};
void my_motor_self_test(void)
{
    uint32_t key,t;
    char buf[32];
    int16_t pwm_duty_temp=1000;
    float current[3]= {0.0f};

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
		
		if(t == (30 * 100)){
            stop_motor1();                              /* 停机*/
            g_bldc_motor1.run_flag = STOP;              /* 标记停机*/
            pwm_duty_temp = 0;
            g_bldc_motor1.pwm_duty = 0;
			break;
		}
    }
}


#endif
#if 9

#include "my_pid.h"
float*user_setpoint = (float*)(&g_speed_pid.SetPoint);    /* 设置目标值指针 指向存放目标值地址 */

extern int32_t motor_pwm_s;
extern int32_t temp_pwm1;
extern int16_t adc_amp_un[3];
extern float  adc_amp_bus;
extern uint16_t g_adc_value[ADC_CH_NUM * ADC_COLL];

void bldc_speed_stop(void);

void my_motor_self_test(void)
{
    uint8_t debug_cmd = 0;                   /* 存放上位机指令 */
    float current_lpf[4] = {0.0f};           /* 存放三相电流以及母线电流 */
    uint32_t key,t;
    char buf[32];
    float current[3] = {0.0f};

    while (1)
    {
        t++;
        if(t % 20 == 0)
        {
            current[0] = adc_amp_un[0] * ADC2CURT; /* U */
            current[1] = adc_amp_un[1] * ADC2CURT; /* V */
            current[2] = adc_amp_un[2] * ADC2CURT; /* W */

            /* 一阶数字滤波 滤波系数0.1 用于显示 */
            FirstOrderRC_LPF(current_lpf[0],current[0],0.1f);
            FirstOrderRC_LPF(current_lpf[1],current[1],0.1f);
            FirstOrderRC_LPF(current_lpf[2],current[2],0.1f);
            FirstOrderRC_LPF(current_lpf[3],adc_amp_bus,0.1f);
            if(g_bldc_motor1.run_flag == STOP)                          /* 停机的电流显示 */
            {
                current_lpf[0] = 0;
                current_lpf[1] = 0;
                current_lpf[2] = 0;
            }
        }

        if((t % 100) == 1){
            printf("[%d]PWM_Duty:%.1f%%  ",(t/100), (float)((g_bldc_motor1.pwm_duty/MAX_PWM_DUTY)*100));/* 显示控制PWM占空比 */
            printf("SetSpeed:%4d  ",(int16_t)*user_setpoint);     /* 显示设置速度 */
            printf("M1 Speed:%4d  ",(int16_t)g_bldc_motor1.speed);
            printf("M1 pos:%4d  ",g_bldc_motor1.pos);                
            // printf("Power:%.3fV  ",g_adc_value[0]*ADC2VBUS);
            printf("Temp:%.1fC\r\n",get_temp(g_adc_value[1]));

            printf("[%d]Amp U:%.3fmA  ",(t/100),(float)current_lpf[0]);
            printf("Amp V:%.3fmA  ",(float)current_lpf[1]);
            printf("Amp W:%.3fmA  ",(float)current_lpf[2]);
            printf("Amp Bus:%.3fmA\r\n\r\n",(float)adc_amp_bus);
        }

        if((t == 100) || (t == 500) || (t == 1500) || (t == 2500))
        {
            g_bldc_motor1.run_flag = RUN;   /* 开启运行 */
            start_motor1();                 /* 开启运行 */
            if(*user_setpoint == 0 && g_bldc_motor1.dir == CCW)
            {
                pid_init();                 /* 换向时刻，重新初始化PID，防止速度突变 */
                g_bldc_motor1.dir = CW;
            }

            *user_setpoint += 800;          /* 顺时针旋转下递增 */
            if(*user_setpoint >= 4000)
                *user_setpoint = 4000;
            if(*user_setpoint == 0)
            {
                g_bldc_motor1.run_flag = STOP; 
                stop_motor1();              /* 停机 */
                g_bldc_motor1.speed = 0;
                motor_pwm_s = 0;
                g_bldc_motor1.pwm_duty = 0;
            }
        }
        
        if((t == 1000) || (t == 2000))
        {
            g_bldc_motor1.run_flag = RUN;   /* 开启运行 */
            start_motor1();                 /* 开启运行 */
            if(*user_setpoint == 0 && g_bldc_motor1.dir == CW)
            {     
                pid_init();
                g_bldc_motor1.dir = CCW;
            }
            *user_setpoint -= 400;          /* 逆时针旋转下递增 */
            if(*user_setpoint <= -4000)
                *user_setpoint = -4000;
            if(*user_setpoint == 0)
            {
                g_bldc_motor1.run_flag = STOP;  
                stop_motor1();              /* 停机 */
                g_bldc_motor1.speed = 0;
                motor_pwm_s = 0;
                g_bldc_motor1.pwm_duty = 0;
            }
        }

        if(t == (30*100))
        {
            bldc_speed_stop();              /* 清除电机状态并关闭电机 */
            break;
        }
        
        delay_ms(10);
    }
}
/**
 * @brief       清除电机状态并关闭电机
 * @param       无
 * @retval      无
 */
void bldc_speed_stop(void)
{
    pid_init();                     /* 重新初始化PID，防止积分过大失控 */
    g_bldc_motor1.run_flag = STOP;  /* 标记停机 */
    stop_motor1();                  /* 停机 */
    g_bldc_motor1.speed = 0;
    motor_pwm_s = 0;
    g_bldc_motor1.pwm_duty = 0;
}


#endif