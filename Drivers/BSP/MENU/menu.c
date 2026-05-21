#include "./BSP/MENU/menu.h"

#define ZERO 0
list* parentlist = NULL;   /* 主链表，响应输出事件 */
list* g_ptr = NULL;    /* 结构体指针， 指向当前光标所在的节点 */
list_parameter list_phandle;    /* 菜单参数结构体句斌 */
slider slider_handle;   /* 滚动条参数结构体句斌 */
flag gflag; /* 标志变量结构体句斌 */
static uint8_t event_key;  /* 按键事件 */
static uint16_t key_pos;   /* 保存按键按下时菜单栏的序号 */

/**
 * @brief       菜单参数初始化
 * @param       无
 * @retval      无
 */
static void mylist_parameter_init(void)
{
    list_phandle.back_color = color_24to16(0xf0f0f0);    /* 菜单区域背景颜色 */
    list_phandle.fram_color = color_24to16(0x9cab9f);    /* 边框颜色 */
    list_phandle.cursor_color = color_24to16(0xcce8ff); /* 光标颜色 */
    list_phandle.title_word_color = BLACK;  /* 菜单标题字体颜色 */    
    list_phandle.title_word_size = 16;  /* 菜单标题字体尺寸 */
    list_phandle.text_word_color = BLACK;   /* 子菜单栏字体颜色 */
    list_phandle.text_word_size = 12;   /* 子菜单栏字体尺寸 */
    list_phandle.heigh = 150;   /* 菜单区域宽度，高度 */
    list_phandle.width = 110; 
    list_phandle.row_dis = 4;   /* 菜单栏行间距 */    
    list_phandle.drump = 8; /* 标题边界线与文本区距离 */
    list_phandle.sx = 0;   /* 菜单区域初始坐标 */
    list_phandle.sy = 0;
    list_phandle.jju = 4;   /* 文本与左边沿距离 */
    list_phandle.heigh_cur = list_phandle.text_word_size + 4;   /* 光标高度 */
    list_phandle.check_color = WHITE;   /* 菜单栏选中颜色 */
}

/**
 * @brief       滚动条参数初始化
 * @param       无
 * @retval      无
 */
static void slider_parameter_init(void)
{
    slider_handle.heigh = list_phandle.heigh - list_phandle.title_word_size;  /* 滚动框高度， 宽度 */
    slider_handle.width = list_phandle.width / 30;    
    slider_handle.sx = list_phandle.width + list_phandle.sx - slider_handle.width;     /* 滚动框起始位置 */
    slider_handle.sy = list_phandle.sy + list_phandle.title_word_size;      
    slider_handle.bs = color_24to16(0xdadada);   /* 滚动框背景颜色 */
    slider_handle.bf = color_24to16(0xdadada);    /* 滚动框颜色 */
    slider_handle.bt = color_24to16(0x858585);     /* 滚动条颜色: 深灰色 */
}

/**
 * @brief       得到当前光标指向的菜单栏位置，注：第一个菜单栏的位置为0...
 * @param       无
 * @retval      得到的位置
 */
uint16_t get_key_index(void)
{
    return key_pos;   /* 减去头节点和标题的位置 */
}

/**
 * @brief       简易菜单界面初始化
 * @param       无
 * @retval      无
 */
void simp_list_init(void)
{   
    mylist_parameter_init();
    slider_parameter_init();
}

/**
 * @brief       创建主菜单
 * @param       plist : 链表结构体指针，指向主链表指针
 * @retval      ptxt : 指向文本内容(只能输入汉字或者字符, 两个不能同时输入, 输入汉字时要在默认加'\n')
 */
void create_mylist(list* plist, char *ptxt)
{   
    clear_linklist(parentlist); /* 清除之前创建的链表 */
    clear_linklist(g_ptr);  /* 清除当前指针链表 */
    gflag.updat = 1;    
    push_node(plist, ptxt, NULL);   /* 跳过头节点，将数据赋值到节点 */   
    parentlist = plist;   /* 将该子链表添加到主链表上 */
}

/**
 * @brief       创建子菜单
 * @param       plist : 链表结构体指针，指向主链表指针
 * @param       ptxt : 指向文本内容(只能输入汉字或者字符, 两个不能同时输入, 输入汉字时要在默认加'\n')
 * @param       pf : 指向自定义的事件函数
 * @retval      无
 */
void create_sonlist(list *plist, char *ptxt, void (*pf)(void))
{
    push_node(plist, ptxt, pf);    /* 链表添加新的节点 */
}

/**
 * @brief       显示滚动条
 * @param       pos : 最顶行的位置
 * @param       maxnum : 最大菜单栏数
 * @param       maxshow : 菜单区最大能显示菜单栏数
 * @retval      无
 */
static void show_slider(uint16_t pos, uint16_t maxnum, uint16_t maxshow)
{
    uint16_t lon;    /* 需要的滚动条长度 */
    static uint16_t py; /* 滚动条每次移动距离 */
    static uint16_t temp = 99;
    uint16_t temp2;
    double temp1;  
    if(temp != pos || gflag.updat) /* 菜单发生滚动或总初始化发生时更新滚动条 */
    {
        /* 滚动框绘制 */
//        lcd_fill((slider_handle.sx), (slider_handle.sy + 3), (slider_handle.sx + slider_handle.width - 2), 
//                (slider_handle.sy + slider_handle.heigh - 3), list_phandle.back_color);   /* 清除滚动框区域 */
//        lcd_draw_frame((slider_handle.sx - 1), (slider_handle.sy + 3), (slider_handle.sx + slider_handle.width - 1), 
//                        (slider_handle.sy + slider_handle.heigh - 2), slider_handle.bf);     /* 添加滚动框 */       
        lcd_fill((slider_handle.sx + 1), (slider_handle.sy + 3), (slider_handle.sx + slider_handle.width - 1), 
                    (slider_handle.sy + slider_handle.heigh - 3), slider_handle.bs);   /* 滚动框添加背景 */ 
        if(maxnum != maxshow)
        {
            temp1 = ((double)maxshow / maxnum) * slider_handle.heigh; /* 计算出需要的滚动条长度 */
            temp2 = temp1;  /* 获取整数部分 */
            if((temp1 - temp2) > 0.5)   /* 小数大于0.5则按四舍五入进位 */
                lon = temp2 + 1;
            else
                lon = temp2;
        }
        else
            lon = slider_handle.heigh - 2;  /* 计算出需要的滚动条长度 */
        temp1  = ((double)slider_handle.heigh - lon) / ((double)maxnum - maxshow); /* 计算出每次需要偏移的距离 */
        temp2 = temp1;  /* 获取整数部分 */
//        if((temp1 - temp2) > 0.5)   /* 小数大于0.5则按四舍五入进位 */
//            py = temp2 + 1;
//        else
            py = temp2;
        
        if(pos == (maxnum - maxshow))   /* 如果到达最低行则从最底行开始绘制滚动条 */
            lcd_fill((slider_handle.sx + 1), (slider_handle.sy + slider_handle.heigh - lon), (slider_handle.sx + slider_handle.width - 1), 
                    (slider_handle.sy + slider_handle.heigh - 3), slider_handle.bt);   /* 在指定区域添加滚动条 */       
        else
            lcd_fill((slider_handle.sx + 1), (slider_handle.sy + 2 + pos * py), (slider_handle.sx + slider_handle.width - 1), 
                    (slider_handle.sy + lon + pos * py - 1), slider_handle.bt);   /* 在指定区域添加滚动条 */
        temp = pos;
    }       
}

/**
 * @brief       设置菜单尺寸
 * @param       heigh : 菜单高度
 * @param       width : 菜单宽度
 * @retval      无
 */
void set_size_list(uint16_t heigh, uint16_t width)
{
    list_phandle.heigh = heigh;   /* 菜单区域宽度，高度 */
    list_phandle.width = width; 
    slider_parameter_init();    /* 滚动条参数更新 */
}

/**
 * @brief       设置菜单起始位置
 * @param       x, y : 起始列,行位置
 * @retval      无
 */
void set_pos_list(uint16_t x, uint16_t y)
{
    list_phandle.sx = x;   /* 菜单区域初始坐标 */
    list_phandle.sy = y;    
    slider_parameter_init();    /* 滚动条参数更新 */
}

/**
 * @brief       设置菜单背景颜色
 * @param       color : 颜色值
 * @retval      无
 */
void set_color_list(uint32_t color)
{
    list_phandle.back_color = color_24to16(color);
    slider_parameter_init();    /* 滚动条参数更新 */
}


/**
 * @brief       响应输出/输入事件, 注：该函数需要不断被调用
 * @param       无
 * @retval      无
 */
void sip_timer_handle(void)
{
    static uint16_t py, px; 
    static bool fist;  /* 标题首次初始化标志 */
    uint16_t tx, ty;    /* 临时坐标，记录菜单栏坐标 */
    static uint16_t bottom, top;    /* 记录当前最底菜单栏和最顶菜单栏,从0开始 */  
    static uint16_t bar_heigh; /* 菜单栏高度 */
    static uint16_t title_heigh;    /* 标题区高度 */
    static uint16_t list_heigh;    /* 菜单区高度 */
    uint8_t count;   /* 当前菜单区域所能显示的最大菜单栏数量 */
    static uint16_t max_show;  /* 当前菜单区域所能显示的最大菜单栏数量 */
    static uint16_t bar_count;   /* 菜单栏数量 */
    static uint16_t barw;   /* 菜单栏实际宽度 */
      
    if(gflag.updat)   /* 首次初始化 */
    {
        /* 参数初始化 */
        py = list_phandle.sy + 1;
        px = list_phandle.sx + list_phandle.jju; 
        barw = list_phandle.width - list_phandle.jju - slider_handle.width - 1;
        title_heigh = list_phandle.title_word_size + list_phandle.drump + 2;
        bar_count = get_linklist_len(parentlist) - 2;
        bar_heigh = list_phandle.text_word_size + list_phandle.row_dis;
        list_heigh = list_phandle.heigh - title_heigh;  /* 文本区高度 */
        count = list_heigh / bar_heigh; /* 计算出当前菜单区域所能显示的最大菜单栏数量 */
        /* 计算出当前菜单区域能显示的最大菜单数量 */
        if(bar_count > count)
            max_show = count;
        else
            max_show = bar_count;
        bottom = max_show - 1;  /* 最底行初始化 */
        top = ZERO; /* 最顶行初始化 */
        fist = 1;   /* 标题首次初始化 */
        g_ptr = parentlist->next;   /* 指向主链表第一个节点 */
        
        /* 菜单界面初始化
          绘制菜单区域背景 */
        lcd_fill(list_phandle.sx, list_phandle.sy, (list_phandle.sx + list_phandle.width), (list_phandle.sy + list_phandle.heigh), list_phandle.back_color);
        lcd_fill(list_phandle.sx, (list_phandle.sy + list_phandle.title_word_size + 2), (list_phandle.sx + list_phandle.width), (list_phandle.sy + list_phandle.title_word_size + 2), list_phandle.fram_color);
        /* 绘制边框 */
        lcd_fill(list_phandle.sx, list_phandle.sy, (list_phandle.sx + list_phandle.width), list_phandle.sy, list_phandle.fram_color);
        lcd_fill(list_phandle.sx, (list_phandle.sy + list_phandle.heigh), (list_phandle.sx + list_phandle.width), (list_phandle.sy + list_phandle.heigh), list_phandle.fram_color);
        lcd_fill(list_phandle.sx, list_phandle.sy, list_phandle.sx, (list_phandle.sy + list_phandle.heigh - 1), list_phandle.fram_color);
        lcd_fill((list_phandle.sx + list_phandle.width), list_phandle.sy, (list_phandle.sx + list_phandle.width), (list_phandle.sy + list_phandle.heigh - 1), list_phandle.fram_color);
        /* 绘制文本内容 */
        while (g_ptr != NULL)
        {
            if(fist)    /* 标题初始化 */
            {
                lcd_show_datx(px, py, g_ptr->data, list_phandle.title_word_size, list_phandle.title_word_size, barw, ZERO, list_phandle.title_word_color, list_phandle.back_color);
                py += title_heigh;
            }
            else    /* 文本初始化 */
            {
                lcd_show_datx(px, py, g_ptr->data, list_phandle.text_word_size, list_phandle.text_word_size, barw, ZERO, list_phandle.text_word_color, list_phandle.back_color);
                py += bar_heigh;
            }
            fist = ZERO;
            if((py + bar_heigh) > (list_phandle.sy + list_phandle.heigh) || px > (list_phandle.sx + list_phandle.width))break;    /* 菜单栏数超出菜单区域范围则停止绘制 */
            g_ptr = g_ptr->next;
        }
        g_ptr = parentlist->next->next;   /* 指向主链表第二个节点 */
        /* 添加光标 */
        lcd_show_datx(px, (list_phandle.sy + title_heigh), g_ptr->data, list_phandle.text_word_size, 
                      list_phandle.heigh_cur, barw, ZERO, list_phandle.check_color, list_phandle.cursor_color);
        show_slider(top, (get_linklist_len(parentlist) - 2), max_show); /* 绘制滚动条 */ 
        gflag.updat = ZERO;  /* 标志置0， 等待下一次更新事件产生 */
    }
    /* 输入事件处理 */
    if(!show_ui_index) //仅在菜单界面时扫描按键
    {
        event_key = key_event; /* 按键扫描 */
        key_event = 0; //清除按键事件
    }
    if(event_key != ZERO)   /* 如果外部更新事件产生，则更新菜单区域 */
    {
        if(event_key == 3)  /* 如果按下的是确认键，则调用回调函数，并将触发源(当前指针指向的节点)传递 */
        {
            void (*event_p)(void) = g_ptr->pf;  /* 指向该节点的事件函数 */ 
            key_pos = g_ptr->index - 2;  /* 更新菜单栏序号 */
            event_p();  /* 调用该节点的事件函数 */ 
            event_key = 0;
            return;
        }
        else    /* 先清除当前节点的光标 */
        {
            uint16_t temp = g_ptr->index - top - 2; /* 计算出当前光标与菜单区最顶行间隔菜单栏数 */

            /* 计算出当前菜单栏的坐标 */
            if(temp == ZERO)    /* 处理第一个菜单栏 */
            {
                ty = list_phandle.sy + title_heigh;        
                tx = list_phandle.sx + list_phandle.jju;
            }
            else
            {
                ty = list_phandle.sy + title_heigh + temp * (list_phandle.text_word_size + list_phandle.row_dis);
                tx = list_phandle.sx + list_phandle.jju;
            }
        
            /* 清除当前结构体指针指向的节点光标 */      
            lcd_show_datx(tx, ty, g_ptr->data, list_phandle.text_word_size, list_phandle.heigh_cur, barw, ZERO, list_phandle.text_word_color, list_phandle.back_color);
        }
        
        /* 根据按键键值为前一个或后一个节点添加光标 */
        if(event_key == 2)  /* 光标前移(向下移动) */
        {
            if(g_ptr->next == NULL) /* 如果当前是最底行菜单栏，且前面没有菜单栏 */
            {
                if(max_show >= bar_count) /* 刷新情况1：不会发生菜单滚动，即maxshow>=len,不更新全屏 */
                {
                    while(g_ptr->prior->prior->prior != NULL)   /* 将结构体指针指向第一个节点 */
                        g_ptr = g_ptr->prior;                  
                }
                else    /* 刷新情况2：会发生菜单滚动，即maxshow<len,刷新全屏 */
                {  
                    while(g_ptr->prior->prior->prior != NULL)   /* 将结构体指针指向第一个节点 */
                        g_ptr = g_ptr->prior;
                    
                    /* 偏移坐标更新 */
                    py = title_heigh + list_phandle.sy;
                    px = list_phandle.sx + list_phandle.jju;
                    
                    /* 更新文本内容 */
                    while (g_ptr != NULL)
                    {
                        lcd_show_datx(px, py, g_ptr->data, list_phandle.text_word_size, list_phandle.text_word_size, barw, ZERO, list_phandle.text_word_color, list_phandle.back_color);
                        py += bar_heigh;
                        if((py + bar_heigh) > (list_phandle.sy + list_phandle.heigh) || px > (list_phandle.sx + list_phandle.width))break;    /* 菜单栏数超出菜单区域范围则停止绘制 */
                        g_ptr = g_ptr->next;
                    }   
                }
                g_ptr = parentlist->next->next; /* 重新指向第一个节点 */  
                bottom = max_show - 1;   /* 最底行重新定义 */
                top = ZERO;    /* 最顶行重新定义 */                
            }
            else if(((g_ptr->index - 2) == bottom) && ((bottom + 1) != bar_count))/* 如果当前是最底行菜单栏，但前面还有菜单栏 */
            {
                g_ptr = parentlist->next->next;   /* 将当前指针指向第二个节点 */
                bottom++;   /* 最底行重新定义 */
                top++;  /* 最顶行重新定义 */
                uint8_t temp = bottom - max_show + 1;
                while(temp--)   /* 将当前指针向前移动 */
                    g_ptr = g_ptr->next;
                /* 偏移坐标更新 */
                py = title_heigh + list_phandle.sy;
                px = list_phandle.sx + list_phandle.jju;
                
                /* 更新文本内容 */
                while (g_ptr != NULL)
                {
                    lcd_show_datx(px, py, g_ptr->data, list_phandle.text_word_size, list_phandle.text_word_size, barw, ZERO, list_phandle.text_word_color, list_phandle.back_color);
                    py += bar_heigh;
                    if((py + bar_heigh) > (list_phandle.sy + list_phandle.heigh) || px > (list_phandle.sx + list_phandle.width))break;    /* 菜单栏数超出菜单区域范围则停止绘制 */
                    g_ptr = g_ptr->next;
                }                
            }
            else{
                g_ptr = g_ptr->next;
            }
        }
        else if(event_key == 1) /* 光标后移(向上移动) */      
        {
            if((top == ZERO) && (top == g_ptr->index - 2)) /* 情况1：如果当前是最顶行菜单栏，且后面没有菜单栏 */
            { 
                if(max_show >= bar_count) /* 刷新情况1：不会发生菜单滚动，即maxshow>=bar_count,不更新全屏 */
                {
                    list* pfront = g_ptr;
                    top = bar_count - max_show;  /* 最顶行重新定义 */
                    bottom = top + max_show - 1; /* 最底行重新定义 */
                    while(pfront)   /* 将当前指针指向最后一个节点 */
                    {
                        g_ptr = pfront;                   
                        pfront = pfront->next;
                    }
                }
                else    /* 刷新情况2：会发生菜单滚动，即maxshow<bar_count,刷新全屏 */
                {                
                    top = bar_count - max_show;  /* 最顶行重新定义 */
                    bottom = top + max_show - 1; /* 最底行重新定义 */                    
                    uint16_t temp = top;
                    while(temp--)   /* 将当前指针向前偏移以得到菜单区滚动后的初始菜单栏 */
                        g_ptr = g_ptr->next;
                    /* 偏移坐标初始化 */
                    py = title_heigh + list_phandle.sy;
                    px = list_phandle.sx + list_phandle.jju;
                    
                    /* 更新文本内容 */
                    while (1)
                    {
                        lcd_show_datx(px, py, g_ptr->data, list_phandle.text_word_size, list_phandle.text_word_size, 
                                     barw, ZERO, list_phandle.text_word_color, list_phandle.back_color);
                        py += bar_heigh;
                        if((py + bar_heigh) > (list_phandle.sy + list_phandle.heigh) || px > (list_phandle.sx + list_phandle.width))break;    /* 菜单栏数超出菜单区域范围则停止绘制 */
                        if(!g_ptr->next)break;  /* 提前观察，避免指针指向空节点 */
                        g_ptr = g_ptr->next;
                    }
                } 
            }
            else if(((g_ptr->index - 2) == top) && top != ZERO)   /* 如果当前指向菜单区最顶行菜单栏，但后面还有菜单栏 */
            {
                g_ptr = g_ptr->prior;   /* 获取后一个菜单栏 */
                bottom--;   /* 最底行重新定义 */
                top--;  /* 最顶行重新定义 */
                
                /* 偏移坐标初始化 */
                py = title_heigh + list_phandle.sy;
                px = list_phandle.sx + list_phandle.jju;
                
                /* 更新文本内容 */
                list* temp_plist = g_ptr;   /* 定义临时结构体指针，省去重新将当前结构体指针指回最顶行节点的步骤 */                                
                while (temp_plist != NULL)
                {
                    lcd_show_datx(px, py, temp_plist->data, list_phandle.text_word_size, list_phandle.text_word_size, 
                                   barw, ZERO, list_phandle.text_word_color, list_phandle.back_color);
                    py += bar_heigh;
                    if((py + bar_heigh) > (list_phandle.sy + list_phandle.heigh) || px > (list_phandle.sx + list_phandle.width))break;    /* 菜单栏数超出菜单区域范围则停止绘制 */
                    temp_plist = temp_plist->next;
                }                                
            }
            else{
                g_ptr = g_ptr->prior;
            }                       
        } 
        event_key = 0;
        /* 坐标重新设置 */
        ty = list_phandle.sy + title_heigh + ((g_ptr->index - 2) - top) * bar_heigh;/* 得到当前位置距离最顶行距离 */
        tx = list_phandle.sx + list_phandle.jju;
        
        /* 添加光标 */   
        lcd_show_datx(tx, ty, g_ptr->data, list_phandle.text_word_size, list_phandle.heigh_cur,
                      barw, ZERO, list_phandle.check_color, list_phandle.cursor_color);
        show_slider(top, (get_linklist_len(parentlist) - 2), max_show); /* 绘制滚动条 */         
    }
}

/**
 * @brief       计算字符串长度
 * @param       pstring : 要计算的字符串
 * @retval      len : 字符串长度
 */
static uint8_t cal_string_len(char *pstring)
{
    uint8_t len = 0;
    while(*pstring != '\0')
    {
        len++;
        pstring++;
    }
    return len;
}

/**
 * @brief       24位RGB转换为16位RGB颜色
 * @param       color : 要转换的颜色
 * @retval      转换的颜色
 */
uint16_t color_24to16(uint32_t color)
{
    return (((color << 8) >> 27) << 11) | (((color << 16) >> 26) << 5) | ((color << 24) >> 27);
}

/**
 * @brief       设置文本字体尺寸
 * @param       size : 字体尺寸
 * @retval      转换的颜色
 */
void set_text_size(uint8_t size)
{
    list_phandle.text_word_size = size;
}

/**
 * @brief       设置文本字体颜色
 * @param       color : 字体颜色
 * @retval      转换的颜色
 */
void set_text_color(uint32_t color)
{
    list_phandle.text_word_color = color_24to16(color);
}

/**
 * @brief       设置标题字体尺寸
 * @param       size : 字体尺寸
 * @retval      转换的颜色
 */
void set_title_size(uint8_t size)
{
    list_phandle.title_word_size = size;
}

/**
 * @brief       设置标题字体颜色
 * @param       color : 字体颜色
 * @retval      转换的颜色
 */
void set_title_color(uint32_t color)
{
    list_phandle.title_word_color = color_24to16(color);
}

/**
 * @brief       重新初始化菜单
 * @param       无
 * @retval      无
 */
void updat_menu(void)
{
    gflag.updat = 1;
    sip_timer_handle(); 
}

/*********************************************标准输出输入函数库************************************************/

/*
 函数功能：得到数字长度
 num：数字
 返回值：数字长度
 */
uint8_t get_len(uint32_t num)
{
	uint8_t len = 1;
	while (num/10 != 0)
	{
		num /= 10;
		len++;
	}
	return len;
}

/*
 函数功能：标准输出函数, 输出的数字最大支持9999999
 注：该函数会将字符串数据拆解为单个字符，如果是%d，则将其对应的数字拆解，并将所有字符保存到数组
 pstr：指向要显示的字符串(包括汉字)
 返回值：指向储存字符的空间的指针
 */
#define BUFFER_SIZE 80 /* 缓冲区大小 */
static char arr[BUFFER_SIZE] = { 0 }; /* 临时缓冲区，保存拆解的字符 */
char* myprintf(const char* pstr, ...)
{
	static uint8_t i = 0;/* 控制数据下标 */
	uint8_t j = i; /* 用于记录本次返回所需要的数组下标值 */
	va_list ap;//定义va_list类型变量
	const char* tpstr = pstr;/* 定义临时指针 */
	va_start(ap, pstr); //将ap指向可变参数列表第一个参数，最右边的一个可变参数

    if(i >= BUFFER_SIZE - 1) /* 如果缓冲区满,则提示错误 */
    {
        lcd_show_string1(0, 10, 12, "Eoor!:buffer is filled!", 1, RED, WHITE);
        while(1); /* 等待处理 */
    }
    
	while (*tpstr != '\0')//统计字符串内%d,%c,%f的数量 
	{
		if(*tpstr == '%')
		{
			tpstr++;
			if(*tpstr == 'd')
			{
				uint32_t a = va_arg(ap, int); //取出可变参数值
				switch (get_len(a)) //得到数值长度,并根据长度选择对应的取模算法
				{
                    /* 个 */
					case 1: arr[i++] = a + 0x30; break;
                    /* 十 */
					case 2: arr[i++] = a / 10 + 0x30; arr[i++] = a % 10 + 0x30; break;
					/* 百 */
                    case 3: arr[i++] = a / 100 + 0x30; arr[i++] = a % 100 / 10 + 0x30; arr[i++] = a % 100 % 10 + 0x30; break;
					/* 千 */
                    case 4: arr[i++] = a / 1000 + 0x30; arr[i++] = a % 1000 / 100 + 0x30; arr[i++] = a % 1000 % 100 / 10 + 0x30; arr[i++] = a % 1000 % 100 % 10 + 0x30; break;
					/* 万 */
                    case 5: 
                    {
                        arr[i++] = a / 10000 + 0x30; arr[i++] = a % 10000 / 1000 + 0x30; arr[i++] = a % 10000 % 1000 / 100 + 0x30; arr[i++] = a % 10000 % 1000 % 100 / 10 + 0x30; 
                        arr[i++] = a % 10000 % 1000 % 100 % 10 + 0x30; 
                        break;
                    }
					/* 十万 */
                    case 6: 
                    {
                        arr[i++] = a / 100000 + 0x30; arr[i++] = a % 100000 / 10000 + 0x30; arr[i++] = a % 100000 % 10000 / 1000 + 0x30;
                        arr[i++] = a % 100000 % 10000 % 1000 / 100 + 0x30; arr[i++] = a % 100000 % 10000 % 1000 % 100 / 10+ 0x30; arr[i++] = a % 100000 % 10000 % 1000 % 100 % 10 + 0x30;
                        break;
                    }
					/* 百万 */
                    case 7: 
                    {
                        arr[i++] = a / 1000000 + 0x30; arr[i++] = a % 1000000 / 100000 + 0x30; arr[i++] = a % 1000000 % 100000 / 10000 + 0x30;
                        arr[i++] = a % 1000000 % 100000 % 10000 / 1000 + 0x30; arr[i++] = a % 1000000 % 100000 % 10000 % 1000 / 100 + 0x30; 
                        arr[i++] = a % 1000000 % 100000 % 10000 % 1000 % 100 / 10 + 0x30; arr[i++] = a % 1000000 % 100000 % 10000 % 1000 % 100 % 10 + 0x30; 
                        break;
                    }
					/* ...... */
                    default:break;
				}
				tpstr++;
			}
		}
		arr[i++] = *tpstr++;//储存数据
	}
//	i = 0;
//	while (arr[i] != '\0')//输出数据
//	{
//		printf("%c", arr[i]);
//		i++;
//	}

	// 销毁可变参数列表
	va_end(ap);
    
    return &arr[j]; /* 返回拆解的字符首地址 */
}

