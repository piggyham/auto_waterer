#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/DHT11/dht11.h"

#define DHT11_TIMEOUT 5000U

static uint8_t dht11_wait_level(GPIO_PinState level)
{
    uint32_t timeout = DHT11_TIMEOUT;

    while (Read_Data == level)
    {
        if (timeout-- == 0)
            return 1;
    }

    return 0;
}

static uint8_t dht11_read_byte_safe(uint8_t *pdata)
{
    uint8_t i;
    uint8_t data = 0;

    for (i = 0; i < 8; i++)
    {
        if (dht11_wait_level(GPIO_PIN_RESET))
            return 1;

        delay_us(30);
        data <<= 1;
        if (Read_Data == GPIO_PIN_SET)
            data |= 1;

        if (dht11_wait_level(GPIO_PIN_SET))
            return 1;
    }

    *pdata = data;
    return 0;
}

static uint8_t dht11_read_safe(uint8_t data[4])
{
    uint8_t rh, rl, th, tl, check;

    DHT11_Start();
    dht11_high;

    if (Read_Data != GPIO_PIN_RESET)
        return 1;

    if (dht11_wait_level(GPIO_PIN_RESET))
        return 2;
    if (dht11_wait_level(GPIO_PIN_SET))
        return 3;

    if (dht11_read_byte_safe(&rh)) return 4;
    if (dht11_read_byte_safe(&rl)) return 5;
    if (dht11_read_byte_safe(&th)) return 6;
    if (dht11_read_byte_safe(&tl)) return 7;
    if (dht11_read_byte_safe(&check)) return 8;

    if ((uint8_t)(rh + rl + th + tl) != check)
        return 9;

    data[0] = rh;
    data[1] = rl;
    data[2] = th;
    data[3] = tl;
    return 0;
}

static void dht11_test_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "DHT11 TEST", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 34, "HUM:", 16, 0, BLACK, WHITE);
    lcd_show_dat(6, 60, "TEMP:", 16, 0, BLACK, WHITE);
    lcd_show_dat(6, 94, "ERR:", 16, 0, BLACK, WHITE);
}

int main(void)
{
    uint8_t data[4] = {0};
    uint8_t err;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    spi2_init();
    lcd_init();
    dht11_init();

    dht11_test_draw_ui();

    while (1)
    {
        err = dht11_read_safe(data);

        lcd_fill(58, 32, 126, 112, WHITE);
        lcd_show_num(60, 34, data[0], 2, 16, BLUE, WHITE);
        lcd_show_num(60, 60, data[2], 2, 16, RED, WHITE);
        lcd_show_num(60, 94, err, 2, 16, err ? RED : BLUE, WHITE);

        LED_BLUE(err ? 1 : 0);
        delay_ms(1000);
    }
}
