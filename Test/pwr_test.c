#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/KEY/key.h"
#include "./BSP/PWR/pwr.h"

static void pwr_test_draw_ui(uint8_t count)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "PWR TEST", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 34, "KEY1: RESET CNT", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 52, "KEY3: STANDBY", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 86, "AUTO:", 16, 0, BLACK, WHITE);
    lcd_show_num(62, 86, count, 2, 16, RED, WHITE);
}

int main(void)
{
    uint8_t count = 10;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    key_init();
    spi2_init();
    lcd_init();
    pwr_wkup_key_init();

    pwr_test_draw_ui(count);

    while (1)
    {
        key_scan();

        if (key_event == 1)
        {
            key_event = 0;
            count = 10;
            pwr_test_draw_ui(count);
        }
        else if (key_event == 3)
        {
            key_event = 0;
            lcd_show_dat(6, 124, "ENTER STANDBY", 12, 0, RED, WHITE);
            SWITCH(0);
            delay_ms(120);
            pwr_enter_standby();
        }

        delay_ms(1000);
        if (count > 0)
        {
            count--;
            lcd_fill(62, 86, 100, 106, WHITE);
            lcd_show_num(62, 86, count, 2, 16, RED, WHITE);
        }
        else
        {
            lcd_show_dat(6, 124, "ENTER STANDBY", 12, 0, RED, WHITE);
            SWITCH(0);
            delay_ms(120);
            pwr_enter_standby();
        }
    }
}
