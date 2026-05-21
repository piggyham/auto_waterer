#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/ADC/adc.h"

static void adc_test_draw_static_ui(void)
{
    lcd_clear(WHITE);
    lcd_fill(0, 0, 128, 16, GREEN);
    lcd_show_dat(4, 2, "ADC DMA TEST", 12, 0, BLACK, GREEN);

    lcd_show_dat(4, 26, "SLOT0:", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 44, "SLOT1:", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 62, "SLOT2:", 12, 0, BLACK, WHITE);

    lcd_show_dat(4, 92, "BUF0:", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 110, "BUF1:", 12, 0, BLACK, WHITE);
    lcd_show_dat(4, 128, "BUF2:", 12, 0, BLACK, WHITE);
}

static void adc_test_show_value(uint16_t x, uint16_t y, uint16_t value, uint16_t color)
{
    lcd_fill(x, y, 126, y + 14, WHITE);
    lcd_show_num(x, y, value, 4, 12, color, WHITE);
}

int main7(void)
{
    uint16_t slot0 = 0;
    uint16_t slot1 = 0;
    uint16_t slot2 = 0;
    uint32_t sample_count = 0;

    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);

    led_init();
    spi2_init();
    lcd_init();
    adc_dma_init((uint32_t)&adc_dma_buff);

    adc_test_draw_static_ui();

    while (1)
    {
        if (g_adc_dma_sta)
        {
            /*
             * get_adc_channel_value() uses 0/1/2 as positions in each scanned
             * ADC group. In this project the real ADC ranks are CH3, CH1, CH2.
             */
            get_adc_channel_value(&slot0, ADC_CHANNEL_0);
            get_adc_channel_value(&slot1, ADC_CHANNEL_1);
            get_adc_channel_value(&slot2, ADC_CHANNEL_2);

            adc_test_show_value(52, 26, slot0, RED);
            adc_test_show_value(52, 44, slot1, BLUE);
            adc_test_show_value(52, 62, slot2, RED);

            adc_test_show_value(52, 92, adc_dma_buff[0], BLUE);
            adc_test_show_value(52, 110, adc_dma_buff[1], RED);
            adc_test_show_value(52, 128, adc_dma_buff[2], BLUE);

            lcd_fill(4, 146, 126, 159, WHITE);
            lcd_show_dat(4, 146, "CNT:", 12, 0, BLACK, WHITE);
            lcd_show_num(32, 146, sample_count++, 5, 12, RED, WHITE);

            LED_BLUE(sample_count & 0x01);

            adc_dma_enable(ADC_BUFFER_SIZE);
            g_adc_dma_sta = 0;
        }
    }
}
