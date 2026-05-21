#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"

#define KEY_LONG_TIME		50     //长按判定时间：/20ms
#define KEY_DOUBLE_TIME		5      //双击判定间隔时间：/20ms
#define KEY_REPEAT_TIME		5      //长按之后连续按下触发事件间隔：/20ms

uint8_t key_event = 0; //按键事件

/**
 * @brief       按键初始化函数
 * @param       无
 * @retval      无
 */
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    gpio_init_struct.Pin = GPIO_PIN_4;                       /* KEY1引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);                /* KEY1引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = GPIO_PIN_5;                       /* KEY2引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);                /* KEY2引脚模式设置,上拉输入 */

    gpio_init_struct.Pin = GPIO_PIN_0;                       /* KEY3(WKUP)引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;                    /* 中断,上升沿 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                      /* 下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);                /* KEY3引脚初始化 */
}


//函数功能：非阻塞式按键扫描
//注：放在定时器内扫描，推荐时间：20ms/次
void key_scan(void)
{
	static uint8_t s = 0, curr_sta = 0, time = 0, key_index = 0;
	
	if(time > 0)time--;
	
	curr_sta = key_get();
	
	if(s == 0)
	{
		if(curr_sta != 0)
		{
			key_index = curr_sta;
			time = KEY_LONG_TIME;
			s = 1;
		}
	}
	else if(s == 1)
	{
		if(time == 0)
		{
			if(key_index == 1)key_event = 201; //长按
			if(key_index == 2)key_event = 202; //长按
			if(key_index == 3)key_event = 203; //长按
			s = 4;
		}
		else if(curr_sta == 0)
		{
			time = KEY_DOUBLE_TIME;
			s = 2;
		}
	}
	else if(s == 2)
	{
		if(time == 0)
		{
			if(key_index == 1)key_event = 1; //单击
			if(key_index == 2)key_event = 2; //单击
			if(key_index == 3)key_event = 3; //单击
			s = 0;
		}
		else if(curr_sta != 0)
		{
			if(key_index == 1)key_event = 101; //双击
			if(key_index == 2)key_event = 102; //双击
            if(key_index == 3)key_event = 103; //双击
			s = 3;
		}
	}
	else if(s == 3)
	{
		if(curr_sta == 0)
		{
			s = 0;
		}
	}
	else if(s == 4)
	{
		if(curr_sta == 0)
		{
			s = 0;
		}
		
		if(curr_sta != 0 && time == 0)
		{
			time = KEY_REPEAT_TIME;
			if(key_index == 1)key_event = 1; //重复触发
			if(key_index == 2)key_event = 2; //重复触发
            if(key_index == 3)key_event = 3; //重复触发
		}
	}
}

//函数功能：获取按键按下的值
//返回值：按键键值
uint8_t key_get(void)
{
    uint8_t key_value = 0;
    if (KEY1 == 0)key_value = 1; 
    if (KEY2 == 0)key_value = 2; 
    if (KEY3 == 1)key_value = 3; 
    return key_value; 
}


