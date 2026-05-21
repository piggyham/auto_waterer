#ifndef __BEER_H
#define __BEER_H

#include "./SYSTEM/sys/sys.h"

extern _Bool beep_sta;

//蜂鸣器控制： 1打开，0关闭
#define BEEP(x)   do{ x ? \
                      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); \
                  }while(0)      

void beep_init(void); //蜂鸣器初始化

#endif


                  