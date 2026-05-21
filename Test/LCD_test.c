#include "./SYSTEM/delay/delay.h"
#include "./BSP/ST7735/st7735.h"
#include "./SYSTEM/sys/sys.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"

/*
 * LCD standalone test.
 *
 * Call lcd_test_run() after:
 *   HAL_Init();
 *   sys_stm32_clock_init(...);
 *   delay_init(...);
 *   led_init();
 *   spi2_init();
 *   lcd_init();
 *
 * Chinese strings are written with GBK byte escapes because st7735.h sets
 * ENCODE to 1, so lcd_show_dat() expects two-byte GB2312/GBK characters.
 */

#define LCD_GBK_TEST_TEXT "\xC9\xE8\xD6\xC3"       /* GBK: she zhi */
#define LCD_GBK_WATER     "\xBD\xBD\xCB\xAE"       /* GBK: jiao shui */
#define LCD_GBK_POWER     "\xB5\xE7\xC1\xBF"       /* GBK: dian liang */

static void lcd_test_page_basic(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "LCD TEST", 12, 0, BLACK, GREEN);

    lcd_show_dat(8, 28, "ASCII OK", 16, 0, RED, WHITE);
    lcd_show_dat(8, 52, LCD_GBK_TEST_TEXT, 16, 0, BLUE, WHITE);
    lcd_show_dat(8, 76, LCD_GBK_WATER, 16, 0, BLACK, WHITE);

    lcd_show_dat(8, 106, "NUM:", 12, 0, BLACK, WHITE);
    lcd_show_num(42, 106, 1234, 4, 12, RED, WHITE);

    lcd_show_dat(8, 126, LCD_GBK_POWER, 12, 0, BLACK, WHITE);
    lcd_show_char(34, 126, ':', 12, 0, BLACK, WHITE);
    lcd_show_num(44, 126, 88, 2, 12, RED, WHITE);
    lcd_show_char(58, 126, '%', 12, 0, RED, WHITE);
}

static void lcd_test_page_color(void)
{
    lcd_clear(WHITE);
    lcd_show_dat(4, 4, "COLOR", 16, 0, BLACK, WHITE);

    lcd_fill(8, 30, 38, 60, RED);
    lcd_fill(48, 30, 78, 60, GREEN);
    lcd_fill(88, 30, 118, 60, BLUE);

    lcd_fill(8, 75, 38, 105, YELLOW);
    lcd_fill(48, 75, 78, 105, BLACK);
    lcd_fill(88, 75, 118, 105, rgb_24to16(0xead6a3));

    lcd_dram_fram(6, 118, 116, 30, BLACK);
    lcd_show_dat(12, 128, "FRAME OK", 12, 0, RED, WHITE);
}

static void lcd_test_page_dynamic(void)
{
    uint16_t x;
    uint16_t count = 0;

    lcd_clear(WHITE);
    lcd_show_dat(4, 4, "DYNAMIC", 16, 0, BLACK, WHITE);
    lcd_dram_fram(8, 36, 112, 24, BLACK);

    while (1)
    {
        for (x = 10; x < 112; x += 4)
        {
            lcd_fill(9, 37, 119, 59, WHITE);
            lcd_fill(x, 40, x + 10, 56, RED);

            lcd_fill(0, 86, 128, 104, WHITE);
            lcd_show_dat(8, 88, "COUNT:", 12, 0, BLACK, WHITE);
            lcd_show_num(52, 88, count++, 5, 12, BLUE, WHITE);

            delay_ms(80);
        }
    }
}

void lcd_test_run(void)
{
    lcd_test_page_basic();
    delay_ms(2000);

    lcd_test_page_color();
    delay_ms(2000);

    lcd_test_page_dynamic();
}



int main3(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    spi2_init();
    lcd_init();

    lcd_test_run();

    while (1)
    {
    }
}
