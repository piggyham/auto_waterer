#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"

/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


void spi2_init(void);
void spi2_set_speed(uint8_t speed);
void spi2_write_byte(uint8_t txdata);//SPI发送数据

#endif
























