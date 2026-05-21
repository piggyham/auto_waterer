#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/FLASH/flash.h"
#include "./BSP/KEY/key.h"

static uint16_t write_buf[4] = {0x1234, 0xABCD, 0x2026, 0x0521};
static uint16_t read_buf[4] = {0};

static void flash_test_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "FLASH TEST", 12, 0, BLACK, GREEN);
    lcd_show_dat(4, 28, "KEY3: WRITE", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 52, "R0:", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 70, "R1:", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 88, "R2:", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 106, "R3:", 12, 0, BLACK, WHITE);
}

static void flash_test_read_show(void)
{
    stmflash_read(FLASH_START_ADDR, read_buf, 4);

    lcd_fill(34, 50, 126, 122, WHITE);
    lcd_show_num(36, 52, read_buf[0], 5, 12, RED, WHITE);
    lcd_show_num(36, 70, read_buf[1], 5, 12, BLUE, WHITE);
    lcd_show_num(36, 88, read_buf[2], 5, 12, RED, WHITE);
    lcd_show_num(36, 106, read_buf[3], 5, 12, BLUE, WHITE);
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

    flash_test_draw_ui();
    flash_test_read_show();

    while (1)
    {
        key_scan();
        if (key_event == 3)
        {
            key_event = 0;
            stmflash_write(FLASH_START_ADDR, write_buf, 4);
            flash_test_read_show();
            LED_BLUE(1);
            delay_ms(200);
            LED_BLUE(0);
        }
        delay_ms(20);
    }
}
