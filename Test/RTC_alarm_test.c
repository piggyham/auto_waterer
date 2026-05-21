#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/RTC/rtc.h"

static void rtc_alarm_test_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "RTC ALARM", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 34, "NOW:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 62, "ALARM:+10S", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 96, "FLAG:", 16, 0, BLACK, WHITE);
}

static void rtc_alarm_test_show_now(void)
{
    lcd_fill(42, 32, 126, 52, WHITE);
    lcd_show_num(44, 34, calendar.hour, 2, 12, BLUE, WHITE);
    lcd_show_char(58, 34, ':', 12, 0, BLUE, WHITE);
    lcd_show_num(66, 34, calendar.min, 2, 12, BLUE, WHITE);
    lcd_show_char(80, 34, ':', 12, 0, BLUE, WHITE);
    lcd_show_num(88, 34, calendar.sec, 2, 12, BLUE, WHITE);
}

int main(void)
{
    uint32_t alarm_sec;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    spi2_init();
    lcd_init();
    rtc_init();

    rtc_set_time(2026, 5, 21, 12, 0, 0);
    alarm_sec = rtc_date2sec(2026, 5, 21, 12, 0, 10);
    rtc_set_alarm(2026, 5, 21, 12, 0, 10);
    __HAL_RTC_ALARM_ENABLE_IT(&g_rtc_handle, RTC_IT_ALRA);

    rtc_alarm_test_draw_ui();
    (void)alarm_sec;

    while (1)
    {
        rtc_get_time();
        rtc_alarm_test_show_now();

        lcd_fill(62, 94, 126, 114, WHITE);
        lcd_show_num(64, 96, flag_alarm, 1, 16, flag_alarm ? RED : BLUE, WHITE);
        LED_BLUE(flag_alarm ? 1 : 0);

        delay_ms(300);
    }
}
