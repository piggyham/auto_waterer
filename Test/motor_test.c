#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/KEY/key.h"
#include "./BSP/MOTER/moter.h"

static uint16_t motor_test_speed = 0;

static void motor_test_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "MOTOR TEST", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 30, "KEY1: SPEED+", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 46, "KEY2: SPEED-", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 62, "KEY3: STOP", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 96, "CCR3:", 16, 0, BLACK, WHITE);
}

static void motor_test_update_speed(uint16_t speed)
{
    motor_test_speed = speed;
    motor_speed(motor_test_speed);
    lcd_fill(56, 94, 126, 114, WHITE);
    lcd_show_num(58, 96, motor_test_speed, 4, 16, RED, WHITE);
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
    motor_init();

    motor_test_draw_ui();
    motor_test_update_speed(0);

    while (1)
    {
        key_scan();

        if (key_event == 1)
        {
            key_event = 0;
            if (motor_test_speed < PWM_Period - 250)
                motor_test_update_speed(motor_test_speed + 250);
            else
                motor_test_update_speed(PWM_Period);
        }
        else if (key_event == 2)
        {
            key_event = 0;
            if (motor_test_speed > 250)
                motor_test_update_speed(motor_test_speed - 250);
            else
                motor_test_update_speed(0);
        }
        else if (key_event == 3)
        {
            key_event = 0;
            motor_test_update_speed(0);
        }

        delay_ms(20);
    }
}
