#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/DWATER/dwater.h"

#define KEY1        HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)     /* 读取KEY1引脚 */
#define KEY2        HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)     /* 读取KEY2引脚 */
#define KEY3        HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)     /* 读取KEY3引脚 */

extern uint8_t key_event;

void key_init(void);    //按键初始化函数 
uint8_t key_get(void);  //获取键值
void key_scan(void);    //按键扫描

#endif


















