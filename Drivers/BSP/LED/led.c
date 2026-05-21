#include "./BSP/LED/led.h"

//函数功能：初始化LED
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct; //定义结构体变量
    
    __HAL_RCC_GPIOA_CLK_ENABLE(); //GPIOA时钟使能
    __HAL_RCC_GPIOB_CLK_ENABLE(); //GPIOB时钟使能
    __HAL_RCC_AFIO_CLK_ENABLE();  //使能复用功能时钟[3,5](@ref)

    //禁用JTAG功能，保留SWD调试
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4);  // 先复位引脚状态
    __HAL_AFIO_REMAP_SWJ_NOJTAG();  // 禁用JTAG，保留SWD[5,7]
    
    gpio_init_struct.Pin = GPIO_PIN_9; //管脚选择
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP; //模式：推挽输出
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW; //速度：低速
    
    HAL_GPIO_Init(GPIOA, &gpio_init_struct); //GPIOA初始化
    
    gpio_init_struct.Pin = GPIO_PIN_4|GPIO_PIN_3; //管脚选择
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP; //模式：推挽输出
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW; //速度：低速
    
    HAL_GPIO_Init(GPIOB, &gpio_init_struct); //GPIOB初始化
    
    // 关闭LED 
    LED_BLUE(0); 
    LED_RED(0); 
    SWITCH(1); //打开外围电路
}

