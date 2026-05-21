#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ST7735/st7735.h"
#include "./BSP/MENU/menu.h"
#include "./MALLOC/malloc.h"
#include "./BSP/KEY/key.h"
#include "./BSP/BEER/beer.h"
#include "./BSP/MOTER/moter.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/GTIM/gtim.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/PWR/pwr.h"
#include "./BSP/FLASH/flash.h"
#include "./BSP/BATTERY/battery.h"
#include "./BSP/DWATER/dwater.h"

//函数功能：检测唤醒源
void check_wkup_source(void)
{  
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) || __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) 
    {
        wkup_source = REST;
        __HAL_RCC_CLEAR_RESET_FLAGS(); 
    }
    else if(RTC->CRL & RTC_FLAG_ALRAF)
    {
        wkup_source = ALRF;   
        RTC->CRL &= ~RTC_FLAG_ALRAF;
    }
    else if(PWR->CSR & PWR_FLAG_WU)
    {
        wkup_source = PA0;
        PWR->CSR |= PWR_FLAG_WU;
    }   
}

int main1(void)
{
    check_wkup_source();                        /* 初始化前先检测唤醒源 */
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* 设置时钟,72M */
    delay_init(72);                             /* 初始化延时函数 */
    led_init();                                 /* 初始化LED, 外围电路开关初始化 */
    key_init();                                 /* KEY初始化 */                
    spi2_init();                                /* SPI初始化 */
    lcd_init();                                 /* LCD初始化 */
    pwr_wkup_key_init();                        /* 按键唤醒初始化 */
    motor_init();                               /* 电机初始化, 定时器1初始化 */    
    my_mem_init(SRAMIN);                        /* 内存管理初始化 */  
    rtc_init();                                 /* RTC初始化 */
    adc_dma_init((uint32_t)&adc_dma_buff);      /* ADC DMA初始化 */  
    dht11_init();                               /* DHT11初始化 */
    beep_init();                                /* 蜂鸣器初始化 */
    simp_list_init();                           /* 菜单初始化 */   
    battery_init();                             /* 电量计初始化 */
    gtim_init(10 - 1, 7200 - 1);                /* 定时器3初始化：定时1ms */

    dwater_sys_init();                          /* 浇水系统初始化 */

    while(1)
    {
        
    }
}

