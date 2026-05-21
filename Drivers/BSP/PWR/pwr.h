#ifndef __PWR_H
#define __PWR_H

#include "./SYSTEM/sys/sys.h"

void pwr_wkup_key_init(void);    /* 唤醒按键初始化 */
void pwr_enter_sleep(void);      /* 进入睡眠模式 */
void pwr_enter_standby(void);    /* 进入待机模式 */

#endif
