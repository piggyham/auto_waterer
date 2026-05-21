#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/GTIM/gtim.h"
#include "./BSP/KEY/key.h"

static void tim3_test_draw_static_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "TIM3 TEST", 12, 0, BLACK, GREEN);

    lcd_show_dat(6, 30, "100ms:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 48, "400ms:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 66, "500ms:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 84, "1s:", 12, 0, BLACK, WHITE);

    lcd_show_dat(6, 112, "KEY:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 132, "TICK:", 12, 0, BLACK, WHITE);
}

static void tim3_test_show_counter(uint16_t x, uint16_t y, uint32_t value, uint16_t color)
{
    lcd_fill(x, y, 126, y + 14, WHITE);
    lcd_show_num(x, y, value, 6, 12, color, WHITE);
}

int main6(void)
{
    uint32_t cnt_100ms = 0;
    uint32_t cnt_400ms = 0;
    uint32_t cnt_500ms = 0;
    uint32_t cnt_1s = 0;
    uint8_t led_state = 0;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    key_init();
    spi2_init();
    lcd_init();

    tim3_test_draw_static_ui();

    /*
     * TIM3 interrupt only starts useful work after flag_sys_init is true.
     * The normal dwater system sets this flag after all modules are ready.
     */
    flag_sys_init = 1;
    gtim_init(10 - 1, 7200 - 1);

    while (1)
    {
        if (flag_100ms)
        {
            flag_100ms = 0;
            cnt_100ms++;
            tim3_test_show_counter(52, 30, cnt_100ms, RED);
        }

        if (flag_400ms)
        {
            flag_400ms = 0;
            cnt_400ms++;
            tim3_test_show_counter(52, 48, cnt_400ms, BLUE);
        }

        if (flag_500ms)
        {
            flag_500ms = 0;
            cnt_500ms++;
            tim3_test_show_counter(52, 66, cnt_500ms, RED);
        }

        if (flag_1s)
        {
            flag_1s = 0;
            cnt_1s++;
            led_state = !led_state;
            LED_BLUE(led_state);
            tim3_test_show_counter(52, 84, cnt_1s, BLUE);
        }

        if (key_event)
        {
            lcd_fill(42, 110, 126, 126, WHITE);
            lcd_show_num(44, 112, key_event, 3, 12, RED, WHITE);
            key_event = 0;
        }

        lcd_fill(48, 130, 126, 146, WHITE);
        lcd_show_num(50, 132, sys_tick, 6, 12, BLUE, WHITE);
        delay_ms(80);
    }
}
