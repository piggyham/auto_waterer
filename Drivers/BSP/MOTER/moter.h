#ifndef __MOTER_H
#define __MOTER_H

#include "./SYSTEM/sys/sys.h"

#define PWM_Period   2500    //定时器1重装载值

// 电机状态管理结构体 
typedef struct
{
    _Bool sta; // 电机状态：0关闭，1打开
    uint16_t speed; // 设置电机速度 
    uint16_t max_speed; // 电机允许的最大速度
    uint16_t cnt_time; // 电机剩余开启时间 
    uint16_t pod_time; //电机开启时间重装载值
}moter;
extern moter motor_handle;

void atim_init(uint16_t arr, uint16_t psc); //定时器初始化  
void motor_init(void); // 电机初始化
void motor_speed(float speed); //电机速度设置

#endif


