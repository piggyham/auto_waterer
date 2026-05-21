#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/DWATER/dwater.h"
#include "./BSP/PWR/pwr.h"
#include "./BSP/MOTER/moter.h"
#include "./BSP/KEY/key.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/BEER/beer.h"

extern uint32_t sys_tick;
extern _Bool flag_500ms;
extern _Bool flag_100ms;
extern _Bool flag_400ms;
extern _Bool flag_1s;

void gtim_init(uint16_t arr, uint16_t psc);        /* 通用定时器 定时中断初始化函数 */

#endif




