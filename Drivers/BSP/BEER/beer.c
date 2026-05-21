#include "./BSP/BEER/beer.h"

_Bool beep_sta = 0; //蜂鸣器状态，1打开，0关闭

void beep_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE(); //寄存器A时钟使能
    
    gpio_init_struct.Pin = GPIO_PIN_7; //管脚选择
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP; //模式：推挽输出
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; //速度：高速
    
    HAL_GPIO_Init(GPIOA, &gpio_init_struct); //寄存器B初始化
    
    BEEP(0); //关闭蜂鸣器
}


