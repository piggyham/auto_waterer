#ifndef _MENU_H
#define _MENU_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LINKLIST/linklist.h"
#include <stdarg.h>

/* 菜单参数配置结构体 */
typedef struct
{
    uint16_t back_color;    /* 菜单区域背景颜色 */
    uint16_t fram_color;    /* 菜单边框颜色 */
    uint16_t cursor_color;  /* 光标颜色 */
    uint16_t text_word_color;    /* 子菜单栏字体颜色 */
    uint8_t text_word_size;  /* 子菜单栏字体尺寸 */
    uint8_t title_word_size;   /* 菜单标题字体尺寸 */
    uint16_t title_word_color; /* 菜单标题字体颜色 */
    uint16_t width, heigh;  /* 菜单区域宽度， 高度 */
    uint16_t sx, sy;    /* 菜单区域初始坐标 */
    uint8_t row_dis;    /* 菜单栏行间距 */
    uint8_t drump; /* 标题边界线与文本区距离 */
    uint8_t heigh_cur;  /* 光标高度 */
    uint16_t check_color;   /* 光标选中时文本颜色 */
    uint8_t jju;    /* 文本与左边沿距离 */
}list_parameter;

/* 滚动条参数配置结构体 */
typedef struct
{
    uint16_t heigh, width;   /* 滚动框高度， 宽度 */
    uint16_t sx, sy;    /* 滚动框起始位置 */
    uint16_t bs;    /* 滚动框背景颜色 */
    uint16_t bf;    /* 滚动框颜色 */
    uint16_t bt;     /* 滚动条颜色, 深灰色 */    
}slider;

/* 标志变量结构体 */
typedef struct
{
    bool updat; /* 初始输出更新标志 */    
}flag;

/* 用户调用函数 */
void simp_list_init(void);  /* 菜单初始化 */  
void create_mylist(list* plist, char *ptxt);    /* 创建菜单界面 */
void create_sonlist(list *plist, char *ptxt, void (*pf)(void));   /* 创建菜单栏 */
void sip_timer_handle(void);    /* 响应输入/输出事件 */
uint16_t get_key_index(void);   /* 得到当前按下的菜单栏的序号, 注：从0开始 */
void set_size_list(uint16_t heigh, uint16_t width); /* 设置菜单尺寸 */
void set_pos_list(uint16_t x, uint16_t y);  /* 设置菜单位置 */
void set_color_list(uint32_t color);    /* 设置菜单颜色 */
uint16_t color_24to16(uint32_t color);  /* rgb24转16 */
void set_title_color(uint32_t color);   /* 设置标题颜色 */
void set_title_size(uint8_t size);  /* 设置标题尺寸 */
void set_text_color(uint32_t color);    /* 设置文本颜色 */
void set_text_size(uint8_t size);   /* 设置文本尺寸 */
void updat_menu(void);  /* 菜单重新初始化 */
char* myprintf(const char* pstr, ...); /* 标准输出函数 */

/* 以下函数用于文件内调用 */
static uint8_t cal_string_len(char *pstring);
static void show_slider(uint16_t pos, uint16_t maxnum, uint16_t maxshow);
static void slider_parameter_init(void);
static void mylist_parameter_init(void); 
uint8_t get_len(uint32_t num);

#endif

