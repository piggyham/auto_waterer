#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/RTC/rtc.h"

static void rtc_test_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "RTC TEST", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 34, "DATE:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 58, "TIME:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 96, "ALARM:", 12, 0, BLACK, WHITE);
}

static void rtc_test_show_time(void)
{
    lcd_fill(44, 32, 126, 74, WHITE);

    lcd_show_num(44, 34, calendar.year, 4, 12, RED, WHITE);
    lcd_show_char(70, 34, '-', 12, 0, RED, WHITE);
    lcd_show_num(78, 34, calendar.month, 2, 12, RED, WHITE);
    lcd_show_char(92, 34, '-', 12, 0, RED, WHITE);
    lcd_show_num(100, 34, calendar.date, 2, 12, RED, WHITE);

    lcd_show_num(44, 58, calendar.hour, 2, 16, BLUE, WHITE);
    lcd_show_char(62, 58, ':', 16, 0, BLUE, WHITE);
    lcd_show_num(72, 58, calendar.min, 2, 16, BLUE, WHITE);
    lcd_show_char(90, 58, ':', 16, 0, BLUE, WHITE);
    lcd_show_num(100, 58, calendar.sec, 2, 16, BLUE, WHITE);
}

int main(void)
{
    uint8_t led_state = 0;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    spi2_init();
    lcd_init();
    rtc_init();

    rtc_set_time(2026, 5, 21, 12, 0, 0);
    rtc_test_draw_ui();

    while (1)
    {
        rtc_get_time();
        rtc_test_show_time();

        led_state = !led_state;
        LED_BLUE(led_state);
        delay_ms(1000);
    }
}
