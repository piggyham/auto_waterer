#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/KEY/key.h"
#include "./BSP/MOTER/moter.h"
#include "./BSP/DWATER/dwater.h"

extern uint16_t adc_value1;
extern uint8_t dwater_mode;

static void dwater_logic_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "DWATER LOGIC", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 28, "KEY1: DRY/WET", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 44, "KEY2: MODE", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 60, "KEY3: STOP", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 88, "MODE:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 106, "FLOWER:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 124, "MOTOR:", 12, 0, BLACK, WHITE);
}

static void dwater_logic_show_state(void)
{
    lcd_fill(58, 86, 126, 140, WHITE);
    lcd_show_num(60, 88, dwater_mode, 1, 12, RED, WHITE);
    lcd_show_num(60, 106, adc_value1, 4, 12, BLUE, WHITE);
    lcd_show_dat(60, 124, motor_handle.sta ? "ON " : "OFF", 12, 0, motor_handle.sta ? RED : BLUE, WHITE);
}

int main(void)
{
    uint8_t dry = 0;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    key_init();
    spi2_init();
    lcd_init();
    motor_init();

    show_ui_index = 0;
    dwater_mode = AUTO;
    adc_value1 = 1000;
    motor_handle.speed = PWM_Period / 2;

    dwater_logic_draw_ui();
    dwater_logic_show_state();

    while (1)
    {
        key_scan();

        if (key_event == 1)
        {
            key_event = 0;
            dry = !dry;
            adc_value1 = dry ? 3000 : 1000;
        }
        else if (key_event == 2)
        {
            key_event = 0;
            if (dwater_mode == AUTO)
                dwater_mode = CLOSE_D;
            else
                dwater_mode = AUTO;
        }
        else if (key_event == 3)
        {
            key_event = 0;
            dwater_mode = CLOSE_D;
            motor_handle.sta = CLOSE;
            motor_speed(0);
        }

        scan_motor_event();
        dwater_logic_show_state();
        delay_ms(100);
    }
}
