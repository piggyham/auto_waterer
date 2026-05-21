#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"

DMA_HandleTypeDef g_dma_adc_handle = {0};                                   /* 定义要搬运ADC数据的DMA句柄 */
ADC_HandleTypeDef g_adc_dma_handle = {0};                                   /* 定义ADC（DMA读取）句柄 */
uint8_t g_adc_dma_sta = 0;                     /* DMA传输状态标志, 0,未完成; 1, 已完成 */
uint16_t adc_dma_buff[ADC_BUFFER_SIZE] = {0};  //dma缓冲区

/**
 * @brief       ADC DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 * @param       mar         : 存储器地址
 * @retval      无
 */
void adc_dma_init(uint32_t mar)
{
    GPIO_InitTypeDef gpio_init_struct;
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
    ADC_ChannelConfTypeDef adc_ch_conf = {0};

    __HAL_RCC_ADC1_CLK_ENABLE();                                              /* 使能ADC1时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                                         /* 开启GPIO时钟 */
    __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1时钟使能 */

    /* 设置ADC时钟 */
    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                  /* ADC外设时钟 */
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                     /* 分频因子6时钟为72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                               /* 设置ADC时钟 */

    /* 设置AD采集通道对应IO引脚工作模式 */
    gpio_init_struct.Pin = GPIO_PIN_3 | GPIO_PIN_1 | GPIO_PIN_2;  /* ADC通道对应的IO引脚 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                               /* 模拟 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);    

    /* 初始化DMA */
    g_dma_adc_handle.Instance = DMA1_Channel1;                             /* 设置DMA通道 */
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* 从外设到存储器模式 */
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* 外设流控模式: 不循环 */
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_adc_handle);

    //ADC访问内部DMA结构体指针，并将其指向DMA结构体
    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);         /* 将DMA与adc联系起来 */

    g_adc_dma_handle.Instance = ADC1;                                   /* 选择哪个ADC */
    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* 数据对齐方式：右对齐 */
    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;                   /* 使能扫描模式 */
    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;                      /* 使能连续转换模式 */
    g_adc_dma_handle.Init.NbrOfConversion = 3;                              /* 赋值范围是1~16，3规则通道序列 */
    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* 禁止规则通道组间断模式 */
    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                          /* 配置间断模式的规则通道个数，禁止规则通道组间断模式后，此参数忽略 */
    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* 触发转换方式：软件触发 */
    HAL_ADC_Init(&g_adc_dma_handle);                                        /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle);                         /* 校准ADC */

    /* 配置ADC通道 */
    adc_ch_conf.Channel = ADC_CHANNEL_3;                                     /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                  /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* 采样时间，设置最大采样周期:239.5个ADC周期 */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* 通道配置 */

    adc_ch_conf.Channel = ADC_CHANNEL_1;                                     /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_2;                                  /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* 采样时间，设置最大采样周期:239.5个ADC周期 */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* 通道配置 */
    
    adc_ch_conf.Channel = ADC_CHANNEL_2;                                     /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_3;                                  /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* 采样时间，设置最大采样周期:239.5个ADC周期 */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* 通道配置 */
    
    /* 配置DMA数据流请求中断优先级 */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    HAL_DMA_Start_IT(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);       /* 启动DMA，并开启中断 */
    HAL_ADC_Start_DMA(&g_adc_dma_handle, &mar, 0);                          /* 开启ADC，通过DMA传输结果 */
    adc_dma_enable(ADC_BUFFER_SIZE); /* 使能一次dma传输 */
}

/**
 * @brief       使能一次ADC DMA传输
 *   @note      该函数用寄存器来操作，防止用HAL库操作对其他参数有修改,也为了兼容性
 * @param       ndtr: DMA传输的次数
 * @retval      无
 */
void adc_dma_enable(uint16_t cndtr)
{
    ADC1->CR2 &= ~(1 << 0);                 /* 先关闭ADC */

    DMA1_Channel1->CCR &= ~(1 << 0);           /* 关闭DMA传输 */
    while (DMA1_Channel1->CCR & (1 << 0));     /* 确保DMA可以被设置 */
    DMA1_Channel1->CNDTR = cndtr;              /* DMA传输数据量 */
    DMA1_Channel1->CCR |= 1 << 0;              /* 开启DMA传输 */

    ADC1->CR2 |= 1 << 0;                    /* 重新启动ADC */
    ADC1->CR2 |= 1 << 22;                   /* 启动规则转换通道 */
}

// 函数功能：计算adc_dma缓存区内的原始adc平均值，并将平均值通过指针传递给要被赋值的变量
// pdat：指针指向要赋值的变量
// channel：哪个adc通道
// 注：扩充通道数时，如果扩充为两个通道，则最后一个通道取值的下标为：adc_dma_buff[i*pre+1]; 
// 3个通道，最后一个通道取值的下标为：adc_dma_buff[i*pre+2]; 
// 4个，则加3，依次类推。
#define ADC_CHANNLE_NUM     3 //ADC通道数
#define ADC_PSC_FACTOR      (ADC_BUFFER_SIZE/ADC_CHANNLE_NUM)     //用于计算平均值
void get_adc_channel_value(uint16_t *pdat, uint8_t channel)
{
    uint16_t i; //数组下标
    uint32_t sum = 0; /* 累加值 */
    if(channel == ADC_CHANNEL_0) /* 通道0 */
    {
        for(i = 0; i < ADC_PSC_FACTOR; i++) /* 对数据累加求和 */
        {
            sum += adc_dma_buff[i*ADC_CHANNLE_NUM+channel]; 
        }
    }
    else if(channel == ADC_CHANNEL_1) /* 通道1 */
    {
        for(i = 0; i < ADC_PSC_FACTOR; i++) /* 对数据累加求和 */
        {
            sum += adc_dma_buff[i*ADC_CHANNLE_NUM+channel]; 
        }
    }
    else if(channel == ADC_CHANNEL_2) /* 通道2 */
    {
        for(i = 0; i < ADC_PSC_FACTOR; i++) /* 对数据累加求和 */
        {
            sum += adc_dma_buff[i*ADC_CHANNLE_NUM+channel]; 
        }
    }
    else if(1) /* 通道x */
    {
        //......
    }
    else return;
    *pdat = (uint16_t)(sum / ADC_PSC_FACTOR); /* 返回平均值 */ 
}

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无 
 * @retval      无
 */
void DMA1_Channel1_IRQHandler(void)
{
    if (DMA1->ISR & (1 << 1))
    {
        g_adc_dma_sta = 1;                      /* 标记DMA传输完成 */
        DMA1->IFCR |= 1 << 1;                /* 清除DMA1 数据流7 传输完成中断 */
    }
}

