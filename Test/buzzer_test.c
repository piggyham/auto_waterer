#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/BEER/beer.h"
#include "./BSP/KEY/key.h"

static void buzzer_test_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "BUZZER TEST", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 34, "KEY1/2: TOGGLE", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 52, "KEY3: BEEP ONCE", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 86, "STATE:", 16, 0, BLACK, WHITE);
}

static void buzzer_test_show_state(uint8_t on)
{
    lcd_fill(66, 84, 126, 104, WHITE);
    lcd_show_dat(68, 86, on ? "ON " : "OFF", 16, 0, on ? RED : BLUE, WHITE);
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
    beep_init();

    buzzer_test_draw_ui();
    buzzer_test_show_state(0);

    while (1)
    {
        key_scan();

        if (key_event == 1 || key_event == 2)
        {
            beep_sta = !beep_sta;
            BEEP(beep_sta);
            buzzer_test_show_state(beep_sta);
            key_event = 0;
        }
        else if (key_event == 3)
        {
            key_event = 0;
            buzzer_test_show_state(1);
            BEEP(1);
            delay_ms(250);
            BEEP(0);
            beep_sta = 0;
            buzzer_test_show_state(0);
        }

        delay_ms(20);
    }
}
