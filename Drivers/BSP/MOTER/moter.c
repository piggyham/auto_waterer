#include "./BSP/MOTER/moter.h"

moter motor_handle; // 电机结构体句斌 

TIM_HandleTypeDef g_timx_npwm_chy_handle; // 定时器1句柄

//电机低通滤波
float l_moter_q = 0.05;  //滤波系数：值越小，电机提速越慢
float l_moter_fre = 0; //旧值

//函数功能：电机初始化
void motor_init(void)
{
    atim_init(PWM_Period, 1); //定时器1初始化，频率20Khz,周期：50us。
    motor_handle.sta = 0;
    motor_handle.max_speed = PWM_Period;
    motor_speed(0); /* 关闭电机 */
}


//函数功能：设置电机速度
//speed：电机速度值，为0时关闭电机
void motor_speed(float speed)
{
    if(speed > PWM_Period)speed = PWM_Period; //最大值限制
    if(speed) 
    {
        //低通滤波
        speed = speed*l_moter_q+(1-l_moter_q)*l_moter_fre;
        l_moter_fre = speed; //更新旧值
    }
    else //如果电机关机，则清除滤波器旧值，避免下次电机快速启动
    {
        l_moter_fre = 0;
    }
    TIM1->CCR3 = (uint16_t)speed;
}

// 函数功能：定时器初始化，PWM模式配置 
void atim_init(uint16_t arr, uint16_t psc)
{
    g_timx_npwm_chy_handle.Instance = TIM1;                            /* 定时器1 */
    g_timx_npwm_chy_handle.Init.Prescaler = psc;                       /* 定时器分频 */
    g_timx_npwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;      /* 递增计数模式 */
    g_timx_npwm_chy_handle.Init.Period = arr;                          /* 自动重装载值 */
    
    HAL_TIM_OC_Init(&g_timx_npwm_chy_handle);/* 输出比较模式初始化，该函数会调用HAL_TIM_OC_MspInit函数 */
    
    TIM_OC_InitTypeDef timx_oc_npwm_chy = {0};            /* 定时器输出 ,用于配置定时器通道的模式 */
   
    timx_oc_npwm_chy.OCMode = TIM_OCMODE_PWM1;                         /* PWM1模式 */   
    timx_oc_npwm_chy.Pulse = 0;                                  /* 设置比较值,此值用来确定占空比 */                                                                     /* 这里默认设置比较值为自动重装载值的一半,即占空比为50% */
    timx_oc_npwm_chy.OCPolarity = TIM_OCPOLARITY_HIGH;                 /* 输出比较极性为低 */
    
    HAL_TIM_OC_ConfigChannel(&g_timx_npwm_chy_handle, &timx_oc_npwm_chy, TIM_CHANNEL_3); /* 通道3输出初始化 */

    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_npwm_chy_handle, TIM_CHANNEL_3); /* 使能通道3预装载 */

    HAL_TIM_OC_Start(&g_timx_npwm_chy_handle, TIM_CHANNEL_3); /* 开启通道3 */  
}

// 函数功能：GPIO初始化
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1)
    {
        GPIO_InitTypeDef gpio_init_struct; /* GPIO句柄 */
        
        __HAL_RCC_TIM1_CLK_ENABLE(); /* TIM1时钟使能 */        
        __HAL_RCC_GPIOA_CLK_ENABLE(); /* GPIOA时钟使能 */
        
        gpio_init_struct.Pin = GPIO_PIN_10; /* 管脚选择 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_NOPULL; /* 无上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
        
        HAL_GPIO_Init(GPIOA, &gpio_init_struct); /* 寄存器A初始化 */
    }
}

