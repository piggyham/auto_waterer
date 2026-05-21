#include "./BSP/PWR/pwr.h"


/**
 * @brief       低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式)
 * @param       无
 * @retval      无
 */
void pwr_wkup_key_init(void)
{
//    GPIO_InitTypeDef gpio_init_struct;
//    
//    __HAL_RCC_GPIOA_CLK_ENABLE();     /* WKUP时钟使能 */

//    gpio_init_struct.Pin = GPIO_PIN_0;                 /* WKUP引脚 */
//    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;              /* 中断,上升沿 */
//    gpio_init_struct.Pull = GPIO_PULLDOWN;                    /* 下拉 */
//    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;            /* 高速 */
//    HAL_GPIO_Init(GPIOA, &gpio_init_struct);     /* WKUP引脚初始化 */

    HAL_NVIC_SetPriority(EXTI0_IRQn,2,2); /* 抢占优先级2，子优先级2 */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn); 
}

/**
 * @brief       WK_UP按键 外部中断服务程序
 * @param       无
 * @retval      无
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief       进入CPU睡眠模式
 * @param       无
 * @retval      无
 */
void pwr_enter_sleep(void)
{
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); /* 执行WFI指令, 进入睡眠模式 */
}

/**
 * @brief       外部中断回调函数
 * @param       GPIO_Pin:中断线引脚
 * @note        此函数会被PWR_WKUP_INT_IRQHandler()调用
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        /* HAL_GPIO_EXTI_IRQHandler()函数已经为我们清除了中断标志位，所以我们进了回调函数可以不做任何事 */
    }
}

/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void pwr_enter_standby(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();               /* 使能电源时钟 */

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); /* 使能KEY_UP引脚的唤醒功能 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);        /* 需要清此标记，否则将保持唤醒状态 */
    HAL_PWR_EnterSTANDBYMode();               /* 进入待机模式 */
}

