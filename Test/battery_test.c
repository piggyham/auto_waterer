#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/BATTERY/battery.h"

extern uint16_t adc_value2;

static void battery_test_draw_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "BATTERY TEST", 12, 0, BLACK, GREEN);
    lcd_show_dat(6, 30, "ADC2:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 52, "VOL x100:", 12, 0, BLACK, WHITE);
    lcd_show_dat(6, 74, "PERCENT:", 12, 0, BLACK, WHITE);
}

int main(void)
{
    uint16_t vol_x100 = 0;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    spi2_init();
    lcd_init();
    adc_dma_init((uint32_t)&adc_dma_buff);

    battery_test_draw_ui();

    while (1)
    {
        if (g_adc_dma_sta)
        {
            get_adc_channel_value(&adc_value0, ADC_CHANNEL_0);
            get_adc_channel_value(&adc_value1, ADC_CHANNEL_1);
            get_adc_channel_value(&adc_value2, ADC_CHANNEL_2);
            adc_dma_enable(ADC_BUFFER_SIZE);
            g_adc_dma_sta = 0;

            updat_capcity_vol();
            vol_x100 = (uint16_t)(battery_vol * 100.0f);

            lcd_fill(70, 28, 126, 92, WHITE);
            lcd_show_num(72, 30, adc_value2, 4, 12, RED, WHITE);
            lcd_show_num(72, 52, vol_x100, 3, 12, BLUE, WHITE);
            lcd_show_num(72, 74, capcity_bfb, 2, 12, RED, WHITE);
            lcd_show_char(88, 74, '%', 12, 0, RED, WHITE);
        }
    }
}
