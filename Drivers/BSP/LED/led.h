#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"
#define LED_BLUE(x)   do{ x ? \
                      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET) : \
                      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); \
                  }while(0)      // LED_BLUE翻转,1亮，0灭

#define LED_RED(x)   do{ x ? \
                      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET) : \
                      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET); \
                  }while(0)      // LED_RED翻转,1亮，0灭

#define SWITCH(x)   do{ x ? \
                      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); \
                  }while(0)      //外围电路开关

#define LED0_Toggle()     HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3)
#define LED1_Toggle()     HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4)
                  
void led_init(void); //初始化led

#endif


                  