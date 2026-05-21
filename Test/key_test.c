#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/KEY/key.h"

static void key_test_draw_static_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "KEY TEST", 12, 0, BLACK, GREEN);

    lcd_show_dat(6, 28, "KEY1: PA5", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 44, "KEY2: PA4", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 60, "KEY3: PA0", 12, 0, BLACK, WHITE);

    lcd_show_dat(6, 88, "EVENT:", 16, 0, BLACK, WHITE);
    lcd_show_dat(6, 120, "RAW:", 12, 0, BLACK, WHITE);
}

static void key_test_show_event(uint8_t event)
{
    lcd_fill(62, 86, 126, 108, WHITE);
    lcd_show_num(64, 88, event, 3, 16, RED, WHITE);

    if (event == 1)
    {
        lcd_show_dat(6, 140, "KEY1 CLICK ", 12, 0, BLUE, WHITE);
    }
    else if (event == 2)
    {
        lcd_show_dat(6, 140, "KEY2 CLICK ", 12, 0, BLUE, WHITE);
    }
    else if (event == 3)
    {
        lcd_show_dat(6, 140, "KEY3 CLICK ", 12, 0, BLUE, WHITE);
    }
    else if (event >= 101 && event <= 103)
    {
        lcd_show_dat(6, 140, "DOUBLE    ", 12, 0, BLUE, WHITE);
    }
    else if (event >= 201 && event <= 203)
    {
        lcd_show_dat(6, 140, "LONG      ", 12, 0, BLUE, WHITE);
    }
    else
    {
        lcd_show_dat(6, 140, "UNKNOWN   ", 12, 0, BLUE, WHITE);
    }
}

static void key_test_show_raw(uint8_t raw)
{
    lcd_fill(36, 118, 80, 134, WHITE);
    lcd_show_num(38, 120, raw, 1, 12, RED, WHITE);
}

int main5(void)
{
    uint8_t raw = 0;
    uint8_t last_raw = 255;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    key_init();
    spi2_init();
    lcd_init();

    key_test_draw_static_ui();

    while (1)
    {
        key_scan();

        raw = key_get();
        if (raw != last_raw)
        {
            key_test_show_raw(raw);
            last_raw = raw;
        }

        if (key_event != 0)
        {
            key_test_show_event(key_event);
            key_event = 0;
        }

        delay_ms(20);
    }
}
