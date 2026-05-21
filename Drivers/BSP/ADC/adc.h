#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"

#define ADC_BUFFER_SIZE 60      //dma缓冲区容量

extern uint16_t adc_dma_buff[ADC_BUFFER_SIZE];
extern uint8_t g_adc_dma_sta;                     /* DMA传输状态标志, 0,未完成; 1, 已完成 */

void adc_dma_init(uint32_t mar);
void adc_dma_enable(uint16_t cndtr);
void get_adc_channel_value(uint16_t *pdat, uint8_t channel);


#endif 















