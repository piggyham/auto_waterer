#include "./BSP/BTIME/btime.h"

uint32_t sys_tick = 0; //程序计时
TIM_HandleTypeDef g_time_handler; //定义定时器句柄

void time_init(uint32_t pre, uint32_t pod)//定时器初始化
{
    g_time_handler.Instance = TIM1; //定时器1地址
    g_time_handler.Init.Prescaler = pre; //分频
    g_time_handler.Init.Period = pod; //重新装载值
    HAL_TIM_Base_Init(&g_time_handler); //定时器初始化
    HAL_TIM_Base_Start_IT(&g_time_handler); //开启定时器中断
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) //MSP初始化,该函数被定时器初始化HAL_TIM_Base_Init函数调用
{
    __HAL_RCC_TIM6_CLK_ENABLE(); //时钟使能
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 3); //优先级设置
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn); //总中断使能
}

void TIM6_DAC_IRQHandler() //定时器6中断服务函数,中断到来时被执行
{
    HAL_TIM_IRQHandler(&g_time_handler); //定时器中断处理函数，该函数会调用回调函数
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //定时器中断回调函数
{
    sys_tick++;
    if(sys_tick%20 == 0)key_scan(); //每隔20ms扫描一次按键

}

