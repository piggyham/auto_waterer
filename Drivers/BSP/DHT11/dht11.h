#ifndef __DHT11_H
#define __DHT11_H

#include "./SYSTEM/sys/sys.h"

#define dht11_high HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define dht11_low HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define Read_Data HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)

extern uint8_t rec_data[4]; // DHT11ÎÂÊª¶ÈÊý¾Ý 

void DHT11_Start(void);
unsigned char DHT11_REC_Byte(void);
void DHT11_REC_Data(void);
void dht11_init(void);

#endif
                  
                  