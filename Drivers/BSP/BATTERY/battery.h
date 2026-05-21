#ifndef _BATTERY_H
#define _BATTERY_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/FLASH/flash.h"

extern uint16_t adc_value0; //ADC通道0原始值
extern uint16_t adc_value1; //ADC通道1原始值
extern float battery_vol; //电池电压
extern float battery_capcity; //当前电量，mah
extern float battery_max_capcity; //最大容量：mah
extern uint8_t capcity_bfb; //容量百分比
extern float Q; //电荷
extern float I; //电流
extern uint16_t fill_time; //电池充满电预计时间，分
extern uint16_t use_time; //电池可用时间，分

void battery_init(void); //电量初始化
float get_current(void); //计算电流
float get_vol(void); //计算电压
void updat_capcity(void); //更新电量，电量计法
void updat_capcity_vol(void); //更新电量，电压法

#endif
