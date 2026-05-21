#ifndef __ST7735_H
#define __ST7735_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/SPI/spi.h"
#include "stdbool.h"

#define ENCODE 1   /* 汉字编码格式 : GB2132(1), UTF8(0)*/

/* 引脚定义 */
//CS
#define ST7735_CS(x) do{x?\
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10, GPIO_PIN_SET):\
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);\
    }while(0)//片选: 0有效

//DC
#define ST7735_DC(x) do{x?\
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_SET):\
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, GPIO_PIN_RESET);\
    }while(0)//命令/数据选择: 0命令，1数据

//RST
#define ST7735_RST(x) do{x?\
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_SET):\
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_RESET);\
    }while(0)//复位: 0有效,时间大于10us，复位后等待120ms

//BLK
#define ST7735_BLK(x) do{x?\
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_RESET):\
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_SET);\
}while(0) //低电平点亮背光，高电平或高阻态熄灭

///* 软件模拟SPI时使用以下引脚作为SDA和SCK引脚 */
//#define ST7735_SDA(x) do{x?\
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET):\
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET);\
//}while(0)

//#define ST7735_SCK(x) do{x?\
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET):\
//    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);\
//}while(0)


typedef struct
{
    uint16_t height;
    uint16_t width;
}lcd_dev;
extern lcd_dev lcddev;

//常用的颜色数据
#define RED  		0xf800
#define GREEN		0x07e0
#define BLUE 		0x001f
#define WHITE		0xffff
#define BLACK		0x0000
#define YELLOW      0xFFE0

void lcd_init(void);//st7735初始化
void st7735_write_dat(uint8_t dat);//st7735写数据
void lcd_set_cursorx(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end);//坐标设置
void lcd_set_cursor(uint16_t x, uint16_t y);
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);
void lcd_write_ram(uint16_t dat);
static uint32_t lcd_pow(uint8_t m, uint8_t n);
uint16_t rgb_24to16(uint32_t color);
static bool is_cnorch(char ch);

/* 数据显示函数 */
void lcd_clear(uint16_t color);
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color, uint16_t bcolor);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color, uint16_t bcolor);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color, uint16_t bcolor);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint16_t bcolor);
void lcd_show_string1(uint16_t x, uint16_t y, uint8_t size, char *p, uint8_t mode, uint16_t color, uint16_t bcolor);
//void lcd_draw_frame(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void lcd_show_chinese(uint16_t x, uint16_t y, uint8_t chi, uint8_t size,  uint8_t mode, uint16_t color, uint16_t bcolor);
void lcd_show_chineseGB(uint16_t x, uint16_t y, char* Chinese, uint16_t size, uint8_t mode, uint16_t color, uint16_t bcolor);
void lcd_show_dat(uint16_t x, uint16_t y, char* pdat, uint16_t size, uint8_t mode, uint16_t color, uint16_t bcolor);
void lcd_show_datx(uint16_t x, uint16_t y, char* pdat, uint16_t size, uint16_t heigh, uint16_t width, uint8_t mode, uint16_t color, uint16_t bcolor);
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
void lcd_show_picture(uint16_t x, uint16_t y, uint8_t *photo);
void lcd_dram_fram(uint16_t x, uint16_t y, uint16_t width, uint16_t heigh, uint16_t color);
void lcd_draw_menu_ui(uint16_t x, uint16_t y, uint16_t width, uint16_t heigh, uint16_t fram, uint16_t back);

/* 显示图片 */
//void lcd_show_longmao(uint8_t x, uint8_t y);

#endif

