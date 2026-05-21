#ifndef __BTIME_H
#define __BTIME_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/KEY/key.h"

extern uint32_t sys_tick; //程序计时
void time_init(uint32_t pre,uint32_t pod);//定时器初始化

#endif

