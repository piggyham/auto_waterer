#include "./BSP/GTIM/gtim.h"


TIM_HandleTypeDef g_timx_handle; // 定时器3句柄 
uint32_t sys_tick = 0; //程序计时
_Bool flag_500ms = 0; //500ms标志
_Bool flag_100ms = 0; //100ms标志
_Bool flag_1s = 0; //1s标志
_Bool flag_400ms = 0; //400ms标志

/**
 * @brief       通用定时器TIM3定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_init(uint16_t arr, uint16_t psc)
{
    __HAL_RCC_TIM3_CLK_ENABLE();                                 /* 使能TIM3时钟 */

    g_timx_handle.Instance = TIM3;                              /* 通用定时器3 */
    g_timx_handle.Init.Prescaler = psc;                         /* 预分频系数 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* 递增计数模式 */
    g_timx_handle.Init.Period = arr;                            /* 自动装载值 */
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);             /* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(TIM3_IRQn);                     /* 开启ITM3中断 */

    HAL_TIM_Base_Start_IT(&g_timx_handle);                      /* 使能定时器3和定时器3更新中断 */
}

/**
 * @brief       定时器3中断服务函数
 * @param       无
 * @retval      无
 */
void TIM3_IRQHandler(void) 
{
    if(__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
    {
        if(flag_sys_init) //仅在系统初始化完成后才处理事件
        {
            sys_tick++;
            if(sys_tick%20 == 0)key_scan(); //按键扫描
            if(sys_tick%500 == 0)flag_500ms = 1; 
            if(sys_tick%400 == 0)flag_400ms = 1;
            if(sys_tick%100 == 0)flag_100ms = 1;
            if(sys_tick%1000 == 0)flag_1s = 1;
            
            if(key_event != 0)lowpower_time = lowpower_time_pod; //有按键事件，重置休眠时间
            if(lowpower_is) //如果低功耗模式打开
            {
                if(sys_tick%1000 == 0)
                {
                    if(!lowpower_time)
                    {
                        SWITCH(0); //关闭外围电路
                        delay_ms(120);  //等待稳定
                        pwr_enter_standby(); // 进入待机模式
                    }                        
                    else lowpower_time--; 
                }                    
            }
            if(flag_alarm_lable) //如果下一个闹钟标签在显示
            {
                if(key_event)//如果有按键事件
                {
                    set_menu_init(); //则重新初始化一次菜单界面
                    flag_alarm_lable = 0;
                }
            }
        }
        __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE); // 清除定时器溢出中断标志位
    }
}

