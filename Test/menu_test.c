#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/KEY/key.h"
#include "./BSP/MENU/menu.h"
#include "./MALLOC/malloc.h"

static list *ptest_menu = NULL;

#define MENU_GBK_TITLE      "\xB2\xCB\xB5\xA5\xB2\xE2\xCA\xD4"             /* GBK: cai dan ce shi */
#define MENU_GBK_PARAM      "\xB2\xCE\xCA\xFD"                             /* GBK: can shu */
#define MENU_GBK_SOUND      "\xC9\xF9\xD2\xF4"                             /* GBK: sheng yin */
#define MENU_GBK_CLOCK      "\xCA\xB1\xD6\xD3"                             /* GBK: shi zhong */
#define MENU_GBK_WATER_MODE "\xBD\xBD\xCB\xAE\xC4\xA3\xCA\xBD"             /* GBK: jiao shui mo shi */
#define MENU_GBK_MOTOR      "\xB5\xE7\xBB\xFA"                             /* GBK: dian ji */
#define MENU_GBK_LOW_POWER  "\xB5\xCD\xB9\xA6\xBA\xC4"                     /* GBK: di gong hao */
#define MENU_GBK_RETURN     "\xB7\xB5\xBB\xD8"                             /* GBK: fan hui */

static void menu_test_show_event(uint8_t index)
{
    lcd_fill(0, 140, 127, 159, WHITE);
    lcd_show_dat(4, 144, "SELECT:", 12, 0, BLACK, WHITE);
    lcd_show_num(54, 144, index, 2, 12, RED, WHITE);
}

static void menu_test_event(void)
{
    menu_test_show_event((uint8_t)get_key_index());
}

static void menu_test_build_menu(void)
{
    ptest_menu = linklist_init();
    create_mylist(ptest_menu, MENU_GBK_TITLE);
    set_size_list(120, 118);
    set_pos_list(5, 18);
    set_color_list(0xead6a3);
    set_title_size(24);

    create_sonlist(ptest_menu, MENU_GBK_PARAM, menu_test_event);
    create_sonlist(ptest_menu, MENU_GBK_SOUND, menu_test_event);
    create_sonlist(ptest_menu, MENU_GBK_CLOCK, menu_test_event);
    create_sonlist(ptest_menu, MENU_GBK_WATER_MODE, menu_test_event);
    create_sonlist(ptest_menu, MENU_GBK_MOTOR, menu_test_event);
    create_sonlist(ptest_menu, MENU_GBK_LOW_POWER, menu_test_event);
    create_sonlist(ptest_menu, MENU_GBK_RETURN, menu_test_event);
}

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    key_init();
    spi2_init();
    lcd_init();
    my_mem_init(SRAMIN);
    simp_list_init();

    show_ui_index = 0;
    lcd_clear(WHITE);
    menu_test_build_menu();

    while (1)
    {
        key_scan();
        sip_timer_handle();
        delay_ms(20);
    }
}
