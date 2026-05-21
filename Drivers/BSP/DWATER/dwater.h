#ifndef __DWATER_H
#define __DWATER_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/MENU/menu.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/BATTERY/battery.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/GTIM/gtim.h"
#include "./BSP/MOTER/moter.h"
#include "./BSP/BEER/beer.h"

//唤醒源
enum wkup{
    ALRF = 1,   //闹钟
    PA0,    //PA0
    REST,   //复位
};

//外设开关
enum device{
    CLOSE,  //关闭
    OPEN,   //打开
};

//浇水模式
enum dwater_mode{
    ALARM,  //闹钟
    AUTO,   //自动
    TIMER,  //定时
    AL_ATO,  //闹钟+自动
    TI_ATO,  //定时+自动
    CLOSE_D,  //关闭    
};

//变量外部声明
extern uint8_t show_ui_index;
extern _Bool lowpower_is;
extern uint8_t lowpower_time;
extern uint8_t lowpower_time_pod;
extern _Bool flag_sys_init;
extern uint8_t wkup_source;;
extern _Bool flag_alarm_lable;
extern uint8_t wkup_source;

void dwater_sys_init(void); //系统初始化


//以下函数为文件内调用
void set_btn_event(void); //设置菜单事件
void moter_btn_event(void); //电机菜单事件
void set_menu_init(void); //设置菜单初始化
void moter_menu_init(); //电机菜单初始化
void dwater_menu_init(void); //浇水模式菜单初始化
    
void show_time(void); //显示时间
void show_batery(void); //显示电量
void show_parmater_ui(void); //显示参数
void motor_sta_ui(void); //电机状态设置
void dwater_alarm_ui(void); //浇水闹钟设置
void dwater_al_ato_ui(void); //浇水闹钟+自动设置
void dwater_tim_menu(void); //浇水定时设置
void dwater_auto_menu(void); //自动浇水设置
void low_power_menu(void); //低功耗设置
void sound_set_menu(void); //声音设置
void clock_set_menu(void); //时钟设置
void motor_speed_menu(void); //电机速度设置
void lock_menu_show(void); //锁屏界面

void adc_updat(void); //ADC更新
void alarm_updat(void); //闹钟更新
void scan_motor_event(void); //处理电机事件

#endif




