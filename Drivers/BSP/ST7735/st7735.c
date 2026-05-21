#include "./SYSTEM/delay/delay.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/ST7735/lcdfont.h"
#include "./BSP/ST7735/picture.h"

#define SPICE_COLOR 0x6666      /* 标记色， 标记颜色视为无效颜色，读取到标记颜色时不会绘制该颜色，而是旋转直接跳过 */

lcd_dev lcddev;

/*
驱动芯片：st7735
屏幕尺寸：1.8
屏幕分辨率：128*160
*/

extern SPI_HandleTypeDef g_spi2_handler; /* SPI2句柄 */

                                        /*软件模拟SPI通信*/
//void st7735_write_cmd(uint8_t cmd)//写命令8位
//{
//    uint8_t i;
//    ST7735_CS(0);
//    ST7735_DC(0);

//    for(i=0;i<8;i++)
//    {
//        if(cmd&0x80)
//        {
//            ST7735_SDA(1);
//        }
//        else
//        {
//            ST7735_SDA(0);
//        }
//        ST7735_SCK(0);
//        ST7735_SCK(1);
//        cmd=cmd<<1;
//    }
//    ST7735_CS(1);
//}

//void st7735_write_dat(uint8_t dat)//写数据8位
//{
//    uint8_t i;
//    ST7735_CS(0);
//    ST7735_DC(1);

//    for(i=0;i<8;i++)
//    {
//        if(dat&0x80)
//        {
//            ST7735_SDA(1);
//        }
//        else
//        {
//            ST7735_SDA(0);
//        }
//        ST7735_SCK(0);
//        ST7735_SCK(1);
//        dat=dat<<1;
//    }
//    ST7735_CS(1);
//}

//void st7735_write_datu16(uint16_t dat)//写数据16位
//{
//    st7735_write_dat((uint8_t)(dat>>8));
//    st7735_write_dat((uint8_t)dat);
//}

                                        /*硬件实现SPI通信*/
void st7735_write_cmd(uint8_t cmd)//写命令8位
{
    ST7735_CS(0);
    ST7735_DC(0);
    spi2_write_byte(cmd);
    ST7735_CS(1);
}

void st7735_write_dat(uint8_t dat)//写数据8位
{
    ST7735_CS(0);
    ST7735_DC(1);
    spi2_write_byte(dat);
    ST7735_CS(1);   
}

void st7735_write_datu16(uint16_t dat)//写数据16位
{
    ST7735_CS(0);
    ST7735_DC(1);
    spi2_write_byte(dat>>8);
    spi2_write_byte(dat);
    ST7735_CS(1);    
}

void lcd_set_cursorx(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end)//坐标设置
{
    st7735_write_cmd(0x2a);
	st7735_write_dat(x_start >> 8);
    st7735_write_dat(x_start);
	st7735_write_dat(x_end >> 8);
    st7735_write_dat(x_end);
	
	st7735_write_cmd(0x2b);
	st7735_write_dat(y_start >> 8);
	st7735_write_dat(y_start);
	st7735_write_dat(y_end >> 8);
	st7735_write_dat(y_end);	
	
	st7735_write_cmd(0x2c);//开始写缓存   
}

/* 
函数功能：lcd设置光标位置
*/
void lcd_set_cursor(uint16_t x, uint16_t y)
{
    lcd_set_cursorx(x, y, x, y);
}

void lcd_init(void)//st7735初始化
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE();  /* ST7735 CS,DC,RST脚时钟使能 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    gpio_init_struct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;//RST,DC,CS
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);
    
    gpio_init_struct.Pin = GPIO_PIN_8; //BLK
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);
    
    lcddev.height = 160;
    lcddev.width = 128;
    
    ST7735_BLK(0); // 点亮背光 
    
    ST7735_RST(0);
    delay_us(20);
    ST7735_RST(1);
    delay_ms(120);
    st7735_write_cmd(0x11);//睡眠关
    delay_ms(120); 
    
    st7735_write_cmd(0xB1);
    st7735_write_dat(0x01);
    st7735_write_dat(0x2C);
    st7735_write_dat(0x2D);
    
    st7735_write_cmd(0xB2); 
	st7735_write_dat(0x01); 
	st7735_write_dat(0x2C); 
	st7735_write_dat(0x2D); 

	st7735_write_cmd(0xB3); 
	st7735_write_dat(0x01); 
	st7735_write_dat(0x2C); 
	st7735_write_dat(0x2D); 
	st7735_write_dat(0x01); 
	st7735_write_dat(0x2C); 
	st7735_write_dat(0x2D); 
	
	st7735_write_cmd(0xB4); //Column inversion 
	st7735_write_dat(0x07); 
	
	//ST7735R Power Sequence
	st7735_write_cmd(0xC0); 
	st7735_write_dat(0xA2); 
	st7735_write_dat(0x02); 
	st7735_write_dat(0x84); 
	st7735_write_cmd(0xC1); 
	st7735_write_dat(0xC5); 

	st7735_write_cmd(0xC2); 
	st7735_write_dat(0x0A); 
	st7735_write_dat(0x00); 

	st7735_write_cmd(0xC3); 
	st7735_write_dat(0x8A); 
	st7735_write_dat(0x2A); 
	st7735_write_cmd(0xC4); 
	st7735_write_dat(0x8A); 
	st7735_write_dat(0xEE); 
	
	st7735_write_cmd(0xC5); //VCOM 
	st7735_write_dat(0x0E); 
	
	st7735_write_cmd(0x36); //设置缓存的读写方向及RGB格式
	st7735_write_dat(0x00); //上到下，左到右,RGB格式传输颜色
	//0x08:上到下，左到右,BGR格式
    
	//ST7735R Gamma Sequence
	st7735_write_cmd(0xe0); 
	st7735_write_dat(0x0f); 
	st7735_write_dat(0x1a); 
	st7735_write_dat(0x0f); 
	st7735_write_dat(0x18); 
	st7735_write_dat(0x2f); 
	st7735_write_dat(0x28); 
	st7735_write_dat(0x20); 
	st7735_write_dat(0x22); 
	st7735_write_dat(0x1f); 
	st7735_write_dat(0x1b); 
	st7735_write_dat(0x23); 
	st7735_write_dat(0x37); 
	st7735_write_dat(0x00); 	
	st7735_write_dat(0x07); 
	st7735_write_dat(0x02); 
	st7735_write_dat(0x10); 

	st7735_write_cmd(0xe1); 
	st7735_write_dat(0x0f); 
	st7735_write_dat(0x1b); 
	st7735_write_dat(0x0f); 
	st7735_write_dat(0x17); 
	st7735_write_dat(0x33); 
	st7735_write_dat(0x2c); 
	st7735_write_dat(0x29); 
	st7735_write_dat(0x2e); 
	st7735_write_dat(0x30); 
	st7735_write_dat(0x30); 
	st7735_write_dat(0x39); 
	st7735_write_dat(0x3f); 
	st7735_write_dat(0x00); 
	st7735_write_dat(0x07); 
	st7735_write_dat(0x03); 
	st7735_write_dat(0x10);  
	
	st7735_write_cmd(0x2a);
	st7735_write_dat(0x00);
	st7735_write_dat(0x00+2);
	st7735_write_dat(0x00);
	st7735_write_dat(0x80+2);

	st7735_write_cmd(0x2b);
	st7735_write_dat(0x00);
	st7735_write_dat(0x00+3);
	st7735_write_dat(0x00);
	st7735_write_dat(0x80+3);
	
	st7735_write_cmd(0xF0); //Enable test command  
	st7735_write_dat(0x01); 
	st7735_write_cmd(0xF6); //Disable ram power save mode 
	st7735_write_dat(0x00); 
	
	st7735_write_cmd(0x3A); //设置RGB模式
	st7735_write_dat(0x05); //16位模式
	
	st7735_write_cmd(0x29);//开启显示
    lcd_clear(WHITE); // 清屏
}

/*************************************************数据显示函数**************************************************/

/* 
函数功能：lcd画点 
*/
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    lcd_set_cursor(x, y);       /* 设置光标位置 */
    st7735_write_datu16(color);
}

/* 
函数功能：lcd写RAM
*/
void lcd_write_ram(uint16_t dat)
{
    st7735_write_datu16(dat);
}

/*
 函数功能：清屏函数
 color： 要清屏的颜色
 返回：无
 */
void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = 0;
    totalpoint = lcddev.height * lcddev.width ;    /* 得到总点数 */
    
    lcd_set_cursorx(0, 0, lcddev.width, lcddev.height); /* 设置区域 */
    for (index = 0; index < totalpoint; index++)
    {
        lcd_write_ram(color);
    }
}


/*
 函数功能：在指定位置显示一个字符
 x, y  ： 列，行坐标
 chr  ：要显示的字符:" "--->"~"
 size ： 字体大小 12/16/24/32
 mode ：显示模式，重叠显示(1), 覆盖显示(0)
 color： 字体颜色
 bcolor： 背景颜色
 返回：无
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color, uint16_t bcolor)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';    /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* 调用1206字体 */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* 调用1608字体 */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];  /* 调用2412字体 */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];  /* 调用3216字体 */
            break;

        default:
            return ;
    } 
    
    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* 获取字符的点阵数据 */
        
        for (t1 = 0; t1 < 8; t1++)   /* 一个字节8个点 */
        {   
            if (temp & 0x80)        /* 有效点,需要显示 */
            {
                lcd_draw_point(x, y, color);        /* 画点出来,要显示这个点 */
            }
            else if(mode == 1)      /* 重叠显示 */
            {
                /* 不显示，即跳过该点 */
            }
            else if(mode == 0)      /* 覆盖显示 */
            {
                lcd_draw_point(x, y, bcolor); /* 画字体背景色 */
            }

            temp <<= 1; /* 移位, 以便获取下一个位的状态 */
            y++;

            if (y > lcddev.height)return;  /* 超区域了 */

            if ((y - y0) == size)   /* 显示完一列了? */
            {
                y = y0; /* y坐标复位 */
                x++;    /* x坐标递增 */
                if (x >= lcddev.width)return;   /* x坐标超区域了 */
                break;
            }
        }
    }
}

/**
 * @brief       平方函数, m^n
 * @param       m: 底数
 * @param       n: 指数
 * @retval      m的n次方
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * @brief       扩展显示len个数字(高位是0也显示)
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
 * @param       mode: 显示模式
 *              [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 *              color：字体颜色
 * @retval      无
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color, uint16_t bcolor)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* 高位需要填充0 */
                {
                    lcd_show_char(x + (size / 2)*t, y, '0', size, 0, color, bcolor);  /* 用0占位 */
                }
                else
                {
                    lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color, bcolor);  /* 用空格占位 */
                }

                continue;
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color, bcolor);
    }
}


/**
 * @brief       显示len个数字
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
                clor：字体颜色
 * @retval      无
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color, uint16_t bcolor)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;   /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color, bcolor);/* 显示空格,占位 */
                continue;   /* 继续下个一位 */
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color, bcolor); /* 显示字符 */
    }
}

/**
 * @brief       显示字符串
 * @param       x,y         : 起始坐标
 * @param       width,height: 区域大小
 * @param       size        : 选择字体 12/16/24/32
 * @param       p           : 字符串首地址
 * @retval      无
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint16_t bcolor)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    //front_are.flag_lon_dat = 0;     /* 标志置0， 以告诉程序要开始传输长数据了 */
    while ((*p <= '~') && (*p >= ' '))   /* 判断是不是非法字符! */
    {
        if (x >= width)     /* 写完一行了， 则换到下一行写 */
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* 退出 */
        lcd_show_char(x, y, *p, size, 0, color, bcolor);
        x += size / 2;
        p++;
    }
}

/*
 函数功能：显示字符串
 x,y         : 起始坐标
 size        : 选择字体 12/16/24/32
 p           : 字符串首地址
 mode        : 叠加显示(1), 覆盖显示(0)
 color       : 字体颜色
 bcolor      : 背景颜色
 返回值：无
 */
void lcd_show_string1(uint16_t x, uint16_t y, uint8_t size, char *p, uint8_t mode, uint16_t color, uint16_t bcolor)
{
    uint8_t x0 = x;
    char *tp = p;
    uint16_t len = 0;
    while(*tp != '\0')      /* 计算出字符串长度 */
    {
        len ++;
        tp ++;
    }
    
    while (len != 0 && *p != '\0')   /* 数据量不为0 */
    {
//        if(*p == '\n')/* 如果读取到换行符'\n', 则换行 */
//        {
//            x = x0;
//            y += size;
//        }
        lcd_show_char(x, y, *p, size, mode, color, bcolor);
        x += size / 2;
        if(x >= lcddev.width) return;   /* 列坐标超出屏幕则退出 */
        p++;
        len --;     /* 剩余数据量减1 */
    }
}

///*
// 函数功能：画一个矩形框
// sx: 起始x坐标
// sy: 起始y坐标
// ex: 终点x坐标
// ey: 终点y坐标
// color：颜色
// */
//void lcd_draw_frame(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
//{
//    uint16_t i, j;
//    if(sx > 240 || sy > 320) return;//坐标范围限制
//        
//    for(j = sy; j < ey + 1; j = j + (ey - sy))//画宽
//    {
//        for(i = sx; i < ex; i++)
//        {
//           lcd_draw_point(i,j,color);
//        }
//    }
//    
//    for(i = sx; i < ex + 1; i = i + (ex - sx))//画高
//    {
//        for(j = sy; j < ey; j++)
//        {
//            lcd_draw_point(i,j,color);
//        }
//    }
//}

/**
 * @brief       24位RGB转换为16位RGB颜色
 * @param       color : 要转换的颜色
 * @retval      转换得到的颜色
 */
uint16_t rgb_24to16(uint32_t color)
{
    return (((color << 8) >> 27) << 11) | (((color << 16) >> 26) << 5) | ((color << 24) >> 27);
}

/**
 * @brief       在指定位置显示一个汉字
 * @param       x,y  : 列，行 坐标
 * @param       chi  : 要显示的汉字:只能传入数字
 * @param       size : 字体大小: 12*12, 16*16, 24*24, 只有 24*24, 其他需要自己添加
 * @param       mode : 叠加方式(1); 非叠加方式(0);
 * @param       color: 字体颜色
 * @param       bcolor: 背景颜色
 */
void lcd_show_chinese(uint16_t x, uint16_t y, uint8_t chi, uint8_t size,  uint8_t mode, uint16_t color, uint16_t bcolor)
{    
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t *pfont = 0;
    uint8_t csize;
    uint8_t temp_size;
    if( x > 240 || y > 320) return;//坐标范围限制
    if(size == 12)
    {
        pfont = (uint8_t *)LCD_CF12x12[chi].dat;
        csize = 24;//得到显示汉字所需要的字节数
        temp_size = 12;
    }
    else if(size == 16)
    {
        pfont = (uint8_t *)LCD_CF16x16[chi].dat;
        csize = 32;//得到显示汉字所需要的字节数
        temp_size = 16;
    }
    else if(size == 17)
    {
        pfont = (uint8_t *)LCD_CF16x16add[chi].dat;
        csize = 32;//得到显示汉字所需要的字节数 
        temp_size = 16;
    }
    else if(size == 24)
    {
        pfont=(uint8_t *)LCD_CF24x24[chi].dat;
        csize = 72;//得到显示汉字所需要的字节数
        temp_size = 24;
    }
    else if(size == 25)
    {
        pfont = (uint8_t *)LCD_CF24x24add[chi].dat;
        csize = 72;
        temp_size = 24;
    }
    else if(size == 32)
    {
        pfont = (uint8_t *)LCD_CF32x32[chi].dat;
        csize = 128;//得到显示汉字所需要的字节数 
        temp_size = 32;
    }
    else return;
    
    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* 获取字符的点阵数据 */

        for (t1 = 0; t1 < 8; t1++)   /* 一个字节8个点 */
        {
            if (temp & 0x80)        /* 有效点,需要显示 */
            {
                lcd_draw_point(x, y, color);        /* 画点出来,要显示这个点 */
            }
            else if(mode == 1)      /* 重叠显示 */
            {
                /* 不显示，即跳过这个点 */
            }
            else if(mode == 0)      /* 覆盖显示 */
            {
                lcd_draw_point(x, y, bcolor); /* 画字体背景色 */
            }

            temp <<= 1; /* 移位, 以便获取下一个位的状态 */
            y++;

            if (y > lcddev.height)return;  /* 超区域了 */

            if ((y - y0) == temp_size)   /* 显示完一列了? */
            {
                y = y0; /* y坐标复位 */
                x++;    /* x坐标递增 */

                if (x >= lcddev.width)return;   /* x坐标超区域了 */

                break;
            }
        }
    } 
}

/**
 * @brief       显示汉字，汉字编码格式：GB2132
 * @param       x,y  : 列，行 坐标
 * @param       Chinese  : 要显示的汉字
 * @param       size : 字体大小: 12x12, 16x16, 24x24, 24x32 
 * @param       mode : 叠加方式(1); 非叠加方式(0);
 * @param       color:字体颜色
*/
void lcd_show_chineseGB(uint16_t x, uint16_t y, char* Chinese, uint16_t size, uint8_t mode, uint16_t color, uint16_t bcolor)
{
    char SigleChinese[3] = {0};//存放汉字的编码
    uint8_t pChinese = 0;
    uint8_t pIndex;//存放汉字索引
    uint8_t i;
    uint8_t temp = 2;
    uint8_t temp1;
    #if (ENCODE)    /* GB2132编码 */
    temp = 2;
    #else   /* UTF8编码 */
    temp = 3;
    #endif
    for(i = 0; Chinese[i] != '\0'; i++)
    {
        SigleChinese[pChinese] = Chinese[i];//储存汉字编码
        pChinese++;
        if(pChinese >= temp)//取得一个汉字的编码
        {
            pChinese = 0;
            switch(size)
            {
                case 32:for(pIndex = 0; strcmp(LCD_CF32x32[pIndex].index,"") != 0; pIndex++)//遍历数组
                        {
                            if(strcmp(LCD_CF32x32[pIndex].index,SigleChinese) == 0)//找到对应的汉字
                            {
                                lcd_show_chinese(x, y, pIndex, size, mode, color, bcolor);
                                break;
                            }
                            if(strcmp(LCD_CF32x32[pIndex + 1].index,"") == 0)   /* 没有找到则显示 '?' */
                            {
                                lcd_show_chinese(x, y, (pIndex + 1), size, mode, color, bcolor);
                                break;
                            }
                        }break;
                case 24:for(pIndex = 0; strcmp(LCD_CF24x24[pIndex].index,"") != 0; pIndex++)//遍历数组
                        {
                            if(strcmp(LCD_CF24x24[pIndex].index,SigleChinese) == 0)//找到对应的汉字
                            {
                                lcd_show_chinese(x, y, pIndex, size, mode, color, bcolor);
                                break;
                            }
                            if(strcmp(LCD_CF24x24[pIndex + 1].index,"") == 0)   /* 没有找到则显示 '?' */
                            {
                                lcd_show_chinese(x, y, (pIndex + 1), size, mode, color, bcolor);
                                break;
                            }
                        }break;
                case 25:for(pIndex = 0; strcmp(LCD_CF24x24add[pIndex].index,"") != 0; pIndex++)//遍历数组
                        {
                            if(strcmp(LCD_CF24x24add[pIndex].index,SigleChinese) == 0)//找到对应的汉字
                            {
                                lcd_show_chinese(x, y, pIndex, size, mode, color, bcolor);
                                break;
                            }
                            if(strcmp(LCD_CF24x24[pIndex + 1].index,"") == 0)   /* 没有找到则显示 '?' */
                            {
                                lcd_show_chinese(x, y, (pIndex + 1), size, mode, color, bcolor);
                                break;
                            }
                        }break;
                case 16:for(pIndex = 0; strcmp(LCD_CF16x16[pIndex].index,"") != 0; pIndex++)//遍历数组
                        {
                            if(strcmp(LCD_CF16x16[pIndex].index,SigleChinese) == 0)//找到对应的汉字
                            {
                                lcd_show_chinese(x, y, pIndex, size, mode, color, bcolor);
                                break;
                            }
                            if(strcmp(LCD_CF16x16[pIndex + 1].index,"") == 0)   /* 没有找到则显示 '?' */
                            {
                                lcd_show_chinese(x, y, (pIndex + 1), size, mode, color, bcolor);
                                break;
                            }
                        }break;
                case 17:for(pIndex = 0; strcmp(LCD_CF16x16add[pIndex].index,"") != 0; pIndex++)//遍历数组
                        {
                            if(strcmp(LCD_CF16x16add[pIndex].index,SigleChinese) == 0)//找到对应的汉字
                            {
                                lcd_show_chinese(x, y, pIndex, size, mode, color, bcolor);
                                break;
                            }
                            if(strcmp(LCD_CF16x16[pIndex + 1].index,"") == 0)   /* 没有找到则显示 '?' */
                            {
                                lcd_show_chinese(x, y, (pIndex + 1), size, mode, color, bcolor);
                                break;
                            }
                        }break;
                case 12:for(pIndex = 0; strcmp(LCD_CF12x12[pIndex].index,"") != 0; pIndex++)//遍历数组
                        {
                            if(strcmp(LCD_CF12x12[pIndex].index,SigleChinese) == 0)//找到对应的汉字
                            {
                                lcd_show_chinese(x, y, pIndex, size, mode, color, bcolor);
                                break;
                            }
                            if(strcmp(LCD_CF12x12[pIndex + 1].index,"") == 0)   /* 没有找到则显示 '?' */
                            {
                                lcd_show_chinese(x, y, (pIndex + 1), size, mode, color, bcolor);
                                break;
                            }
                        }break;                       
                default : return;
            }
            x += size;  /* 列地址往后偏移 */
        }
    }
}

/**
 * @brief       判断传入的是不是字符
 * @param       ch : 传入的数据
 * @retval      1,是字符(包括空格)。 0,不是字符
 */
static bool is_cnorch(char ch)
{
    return (ch < '~' && ch >= ' ') ? 1 : 0;
}

/**
 * @brief       在指定区域内填充单个颜色
 * @param       (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex - sx + 1) * (ey - sy + 1)
 * @param       color:  要填充的颜色
 * @retval      无
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i;
    uint32_t totalpoint = 0;
    
    totalpoint = (ex - sx + 1) * (ey - sy + 1);
    if(ey > 320 || ex > 240)return;
    lcd_set_cursorx(sx, sy, ex, ey);      /* 设置光标位置 */
    for (i = 0; i <= totalpoint; i++)
    {
        lcd_write_ram(color);
    }
}

/**
 * @brief       显示汉字,字符,数字(显示数字时需要用%d声明)
 * @param       x,y  : 列，行坐标
 * @param       chi  : 要显示的汉字:只能传入数字
 * @param       size : 字体大小: 12x12, 16x16, 24x24, 24x32 
 * @param       mode : 叠加方式(1); 非叠加方式(0);
 * @param       color:字体颜色
 * @param       bcolor: 背景颜色
 */
void lcd_show_dat(uint16_t x, uint16_t y, char* pdat, uint16_t size, uint8_t mode, uint16_t color, uint16_t bcolor)
{
    char temp_buff[4];
    uint16_t sx = x;
    while(*pdat != '\0')
    {
        if(*pdat == '\n') /* 如果是换行符，则在下一行开始显示 */
        {
            x = sx; /* 列坐标重置 */
            y += size; /* 下一行开始显示 */
            pdat++;
        }
        if(is_cnorch(*pdat))    /* 判断是不是字符 */
        {
            lcd_show_char(x, y, *pdat, size, mode, color, bcolor);
            x += size / 2;
        }
        else    /* 否则是汉字 */
        {
            #if (ENCODE)    /* GB2132编码 */
            temp_buff[0] = *pdat++;   /* 读取后面两个字节 */
            temp_buff[1] = *pdat;
            temp_buff[2] = '\0';    /* 添加结束符 */
            #else   /* UTF8编码 */
            temp_buff[0] = *pdat++;   /* 读取后面三个字节 */
            temp_buff[1] = *pdat++;
            temp_buff[2] = *pdat;
            temp_buff[3] = '\0';    /* 添加结束符 */            
            #endif
            
            lcd_show_chineseGB(x, y, temp_buff, size, mode, color, bcolor);
            x += size;
        }
        pdat++;
    }
}

/**
 * @brief       显示汉字与字符(指定区域，添加光标用)
 * @param       x,y  : 列，行坐标
 * @param       chi  : 要显示的汉字:只能传入数字
 * @param       size : 字体大小: 12x12, 16x16, 24x24, 24x32 
 * @param       mode : 叠加方式(1); 非叠加方式(0);
 * @param       color: 字体颜色
 * @param       bcolor: 背景颜色
 */
void lcd_show_datx(uint16_t x, uint16_t y, char* pdat, uint16_t size, uint16_t heigh, uint16_t width, uint8_t mode, uint16_t color, uint16_t bcolor)
{
    char temp_buff[4];
    uint16_t sx = x;
    while(*pdat != '\0')
    {
        if(*pdat == '\n') /* 如果是换行符，则在下一行开始显示 */
        {
            x = sx; /* 列坐标重置 */
            y += size; /* 下一行开始显示 */
            pdat++;
        }
        if(is_cnorch(*pdat))    /* 判断是不是字符 */
        {
            lcd_show_char(x, y, *pdat, size, mode, color, bcolor);
            x += size / 2;
        }
        else    /* 否则是汉字 */
        {
            #if (ENCODE)    /* GB2132编码 */
            temp_buff[0] = *pdat++;   /* 读取后面两个字节 */
            temp_buff[1] = *pdat;
            temp_buff[2] = '\0';    /* 添加结束符 */
            #else   /* UTF8编码 */
            temp_buff[0] = *pdat++;   /* 读取后面三个字节 */
            temp_buff[1] = *pdat++;
            temp_buff[2] = *pdat;
            temp_buff[3] = '\0';    /* 添加结束符 */            
            #endif
            
            lcd_show_chineseGB(x, y, temp_buff, size, mode, color, bcolor);
            x += size;
        }
        pdat++;
    }

    /* 填充光标 */
    lcd_fill(sx - 2, y, sx - 1, (y + size), bcolor); /* 填充数据前面的部分 */
    if(x > sx + width)return;
    lcd_fill(x, y, sx + width, (y + size), bcolor);    /* 填充数据后面的部分 */
    lcd_fill(sx - 2, (y - (heigh - size) / 2), sx + width, (y - 1), bcolor);   /* 填充上面部分 */
    lcd_fill(sx - 2, (y + size), sx + width, (y + size + (heigh - size) / 2), bcolor);    /* 填充下面部分 */    
}

/**
 * @brief       在指定区域内填充指定颜色块
 * @param       (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex - sx + 1) * (ey - sy + 1)
 * @param       color: 要填充的颜色数组首地址
 * @retval      无
 */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t i;
    uint32_t totalpoint = 0;
    
    totalpoint = (ex - sx + 1) * (ey - sy + 1);
    if(ey > 320 || ex > 240)return;
    lcd_set_cursorx(sx, sy, ex, ey);      /* 设置光标位置 */
    for (i = 0; i <= totalpoint; i++)
    {
        lcd_write_ram(color[i]);
    }
}

/*
函数功能：显示全屏和非全屏图片
x：起始列坐标
y：起始行坐标
px：图片向右偏移量，注：偏移量不为0时，x,y都会被设置为0!
py：图片向上偏移量，注：偏移量不为0时，x,y都会被设置为0!
photo：指向图片数组指针
*/
void lcd_show_picture(uint16_t x, uint16_t y, uint8_t *photo)
{
    uint16_t heigh, width;
    uint16_t i, j;
    uint32_t k = 8;
    uint16_t color;
    uint16_t temp = 0;
    
    if(x > 240 || y > 320) return;
    
    temp = photo[3];
    width = photo[2] | (temp << 8);     /* 获取图片宽度 */
    temp = photo[5];
    heigh = photo[4] | (temp << 8);     /* 获取图片高度 */
    
    lcd_set_cursorx(x, y, x + width, y + heigh);   /* 设置显示区域 */
    for(i = y; i < heigh * width; i++)      /* 填充颜色 */
    {     
        temp = photo[k+1];
        color = photo[k] | (temp << 8);
        lcd_write_ram(color);
        k = k + 2; 
    }
}

/*
 函数功能：绘制一个边框
 x：初始列坐标
 y：初始行坐标
 width：宽
 heigh：高
 color：边框颜色
 */
void lcd_dram_fram(uint16_t x, uint16_t y, uint16_t width, uint16_t heigh, uint16_t color)
{
    lcd_fill(x, y, x+width+1, y, color); /* 画上边 */
    lcd_fill(x, y+heigh, x+width+1, y+heigh, color); /* 画下边 */
    lcd_fill(x, y, x, y+heigh+1, color); /* 画左边 */
    lcd_fill(x+width, y, x+width, y+heigh+1, color); /* 画右边 */
}

/*
 函数功能：绘制一个有边框的矩形
 x：初始列坐标
 y：初始行坐标
 width：宽
 heigh：高
 fram：边框颜色
 back：背景颜色
 */
void lcd_draw_menu_ui(uint16_t x, uint16_t y, uint16_t width, uint16_t heigh, uint16_t fram, uint16_t back)
{
    lcd_dram_fram(x, y, width, heigh, fram); /* 绘制边框 */
    lcd_fill(x+1, y+1, x+width-1, y+heigh-1, back); /* 绘制背景 */    
}

//void lcd_show_longmao(uint8_t x, uint8_t y)//显示龙猫
//{
//    lcd_show_picture(x, y, (uint8_t *)gImage_longmaox);
//}
