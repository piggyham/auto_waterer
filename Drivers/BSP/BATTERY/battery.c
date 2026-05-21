#include "./BSP/BATTERY/battery.h"
#include "stdio.h"

//注;ADC更新需要单独实现

//以下参数根据IP5407芯片设置
#define TERMINATION_CURRENT 0.05f   //电池充满电时的截止电流
#define TERMINATION_MAX_VOLTAGE 4.2f    //电池充满电时的截止电压
#define TERMINATION_MIN_VOLTAGE 2.95f    //电池放完电时的截止电压

#define CAPCITY 666 //容量：mah
#define ADC_REF_VOL 3.25f     //ADC参考电压
#define INA240_MID_VOL (ADC_REF_VOL/2.0f)    //电流采样芯片中点电压
#define INA240_GAIN  50      //电流采样芯片电压放大倍数
#define VOL_GAIN    2.0f   //电压分频
#define INA240_R    0.02f   //电流采样电阻值：Ω
#define ADC_FBL  (ADC_REF_VOL/4096)    //ADC分辨率=ADC_REF_VOL/4096

extern uint16_t adc_value2;

//预先计算常量
#define INA240_SCALE_FACTOR  (1.0f / (INA240_GAIN * INA240_R))  
// 合并所有常量
static const float INA240_K = (ADC_FBL) * INA240_SCALE_FACTOR;
static const float INA240_B = (-INA240_MID_VOL) * INA240_SCALE_FACTOR;

float battery_vol = 0.0; //电池电压
float battery_capcity = 0; //当前电量，mah
float battery_max_capcity = CAPCITY; //最大容量：mah
uint8_t capcity_bfb = 0; //容量百分比
float Q = 0.0; //电荷
float I = 0.0; //电流
uint16_t fill_time = 0; //电池充满电预计时间，分
uint16_t use_time = 0; //电池可用时间，分

void battery_init(void) //初始化电量计
{
    uint16_t temp = 0, i = 0;
    uint32_t sum = 0; //ADC累加值
    uint16_t adc_value2 = 0; //ADC原始值
    //关闭所有负载, 更新一次电压
    while(i < 1000) //连续记录多次求平均值
    {
        if(g_adc_dma_sta)
        {
            get_adc_channel_value(&adc_value2, ADC_CHANNEL_2);
            sum += adc_value2;
            adc_dma_enable(ADC_BUFFER_SIZE);
            g_adc_dma_sta = 0;
            i++;
        }  
    }
    battery_vol = (sum/1000.0f)*ADC_FBL*VOL_GAIN; //计算电池电压
    if(battery_vol > TERMINATION_MAX_VOLTAGE)battery_vol = TERMINATION_MAX_VOLTAGE; 
    
    //1.根据电池电压估算初始电量
    //battery_capcity = ((battery_vol-TERMINATION_MIN_VOLTAGE)*0.8333333f)*battery_max_capcity; //电量计法
    capcity_bfb = (battery_vol/TERMINATION_MAX_VOLTAGE)*100; //电压法
    
//    //2.从FLASH读取上一次的电量
//    stmflash_read(FLASH_START_ADDR, &temp, 1); 
//    battery_capcity = temp;
//    if(battery_capcity < 0)battery_capcity=0;
}

//函数功能：更新电量，电压法
//返回值：无
void updat_capcity_vol(void)
{
    get_vol();
    capcity_bfb = (battery_vol/TERMINATION_MAX_VOLTAGE)*100;
    if(capcity_bfb > 99)capcity_bfb = 99;
}

//函数功能：更新电量，电量计法
//返回值：无
//注：每0.1秒调用一次
void updat_capcity(void)
{
    static uint8_t i = 0;
    uint16_t temp = 0;
    I = get_current();
    if(((I>0?I:-I) <= TERMINATION_CURRENT) && (battery_vol > TERMINATION_MAX_VOLTAGE)) //充电电流低于阈值,且电池电压大于阈值时认为电池充满电
    {
        battery_max_capcity = battery_capcity; //当前电量更新为最大容量
        //battery_capcity = battery_max_capcity;
    }
    else if(battery_vol < TERMINATION_MIN_VOLTAGE) //电压过低时认为电池放完电
    {
        battery_capcity = 0;
        stmflash_write(FLASH_START_ADDR, &temp, 1); //更新到FLASH，以便下次开机时使用
    }
    else
    {
        Q = (0.1f*I)/3.6f; //计算每0.1秒转移的电荷量：mah
        battery_capcity += Q; //累加到电量
        i++;
        if(i%10==0) //每隔1秒保存一次FLASH
        {
            i = 0;
            temp = (uint16_t)battery_capcity;
            stmflash_write(FLASH_START_ADDR, &temp, 1); //更新到FLASH，以便下次开机时使用
        }
        if(battery_capcity < 0)battery_capcity=0; //最低电量限制，避免容量变成负
        if(battery_capcity > CAPCITY)
        {
            battery_capcity=CAPCITY;
        }
    }        
    if(I>0)
    {
        fill_time = (battery_max_capcity-battery_capcity)/(Q*600.0f); //计算充满时间
        if(fill_time > 40000)fill_time = 0;
    }
    else 
    {
        use_time = battery_capcity/(-Q*600.0f); //计算可用时间
    }
    capcity_bfb = battery_capcity/battery_max_capcity*100; //计算百分比
}

//函数功能：计算电压
//返回值：电压：V
float get_vol(void)
{
    //低通滤波
    float L_q = 0.4; //系数
    static float L_Yn = 0; //旧值
    battery_vol = adc_value2*ADC_FBL*VOL_GAIN; //计算电池电压
    battery_vol = battery_vol*L_q + (1-L_q)*L_Yn; //滤波
    L_Yn = battery_vol; //更新旧值
    
    return battery_vol;
}

//函数功能：计算电流
//返回值：电流：安培(A)（负号代表电流反向）
float get_current(void)
{
    //低通滤波
    float q = 0.2; //系数
    static float Yn0 = 2048.0; //旧值
    adc_value2 = q*Yn0+(1-q)*adc_value2; //对ADC滤波
    Yn0 = adc_value2;

    return adc_value2 * INA240_K + INA240_B;
}
