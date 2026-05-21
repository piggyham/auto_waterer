#ifndef __RTC_H
#define __RTC_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/LED/led.h"
#include "./BSP/DWATER/dwater.h"

/* 时间结构体, 包括年月日周时分秒等信息 */
typedef struct
{
    uint8_t hour;       /* 时 */
    uint8_t min;        /* 分 */
    uint8_t sec;        /* 秒 */
    /* 公历年月日周 */
    uint16_t year;      /* 年 */
    uint8_t  month;     /* 月 */
    uint8_t  date;      /* 日 */
    uint8_t  week;      /* 周 */
} _calendar_obj;
extern _calendar_obj calendar;                      /* 时间结构体 */
extern uint16_t bkpflag; /* 备份域标志,用来判断RTC备份域有没有设置过 */
extern RTC_HandleTypeDef g_rtc_handle;
extern _Bool flag_alarm;

uint8_t rtc_get_time(void);
uint8_t rtc_init(void);
uint8_t rtc_set_time(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t rtc_set_alarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
long rtc_date2sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);
static uint8_t rtc_is_leap_year(uint16_t year);
void sec_to_time(uint32_t cnt); /* 将秒钟数转为年月日时分秒 */

#endif

