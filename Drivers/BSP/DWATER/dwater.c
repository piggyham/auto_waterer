#include "./BSP/DWATER/dwater.h"

#define FLOWER_T    2000    //花盆检测阈值
#define GROOVE_T    2000    //水槽检测阈值
#define LOCK_SWITCH     0   //锁屏密码，1开启，0关闭

uint8_t wkup_source; //唤醒源检测，1闹钟，2PA0，3复位

//ADC原始值
uint16_t adc_value0; //水槽    
uint16_t adc_value1; //花盆
uint16_t adc_value2; //电池

uint8_t show_ui_index; //界面索引
_Bool flag_updat_ui; //界面初始化标志
const uint8_t lock_code[6] = {1, 2, 3, 1, 2, 3}; // 锁屏密码
_Bool lock_code_is; //判断输入的密码是不是正确，1正确
uint16_t alarm[5] = {2025, 6, 17, 16, 24}; //闹钟：年，月，日，时，分
_Bool flag_alarm_lable; //闹钟显示标签，1标签显示，0不显示

//浇水模式dwater_mode：
//0闹钟：闹钟时间到了，启动电机一段时间，之后自动设置下一个闹钟
//1自动：自动检测花盆内还有没有水，如果没有水了，就启动电机
//2定时：定时到了，启动电机一段时间
//3闹钟+自动：闹钟时间到了，自动检测花盆内还有没有水，如果没有水了，就启动电机
//4定时+自动：定时到了，自动检测花盆内还有没有水，如果没有水了，就启动电机
uint8_t dwater_mode; //浇水模式
uint8_t timer_pod_value; //定时模式重装载值：/时
uint32_t timer_cnt_value;   //定时模式递减值：/s
_Bool flag_sur_timer; //定时标志，1定时到

_Bool lowpower_is; //低功耗是否打开，注：闹钟事件可以将单片机从低功耗唤醒
uint8_t lowpower_time; //单片机进入低功耗等待时间
uint8_t lowpower_time_pod; //等待时间重装载值
_Bool flag_sys_init;  //系统初始化完成标志 

//电量界面参数
const uint8_t bat_x = 75; /* 初始列位置 */
const uint8_t bat_y = 0; /* 初始行位置 */
const uint16_t bat_word_color = BLACK; /* 字体颜色 */
const uint16_t bat_back_color = GREEN; /* 背景颜色 */
const uint8_t bat_size = 12; //字体尺寸
//时间界面参数
const uint8_t time_x = 0;  /* 初始列位置 */
const uint8_t time_y = 0; /* 初始行位置 */
const uint16_t time_word_color = BLACK; /* 字体颜色 */
const uint16_t time_back_color = GREEN; /* 背景颜色 */
const uint8_t time_size = 12; /* 字体尺寸 */
//参数界面参数
uint16_t parmater_color_fram = 0; /* 边框颜色：浅灰色 */
uint16_t parmater_color_back = 0; /* 背景区颜色，深灰色色 */
const uint16_t parmater_color_word = BLACK; /* 文字颜色 */
const uint16_t parmater_color_num = RED; /* 数字颜色 */
const uint8_t parmater_size = 12; /* 字体尺寸 */

//多级菜单结构体句柄
list* pmoter; // 电机菜单
list* pset; // 设置菜单
list*pdwater; //浇水模式

/************************************************主函数*****************************************************/
//函数功能：浇水系统初始化
void dwater_sys_init(void)
{
    //设置参数
    LED_BLUE(CLOSE); //关闭LED 
    LED_RED(CLOSE);
    motor_handle.sta = CLOSE; //电机状态：关闭电机
    motor_handle.speed = PWM_Period; //电机速度设置
    motor_handle.pod_time = 60; //闹钟与定时模式下电机开启时间：60s
    motor_handle.max_speed = PWM_Period; //电机限速设置
    beep_sta = CLOSE; //关闭蜂鸣器
    dwater_mode = ALARM; //浇水模式设置
    lowpower_is = OPEN; //低功耗模式设置
    lowpower_time_pod = 60; //进入低功耗时间：60s
    timer_pod_value = 10; //定时模式定时时间，时
    //根据设置的参数，初始化其他参数
    lowpower_time = lowpower_time_pod; //装载时间
    if((dwater_mode == ALARM || dwater_mode == AL_ATO)) //如果是闹钟模式
    { 
        __HAL_RTC_ALARM_ENABLE_IT(&g_rtc_handle, RTC_IT_ALRA); //打开闹钟中断
        if(wkup_source == ALRF) //且单片机被闹钟唤醒
        {
            flag_alarm = 1; //手动将闹钟标志置1，触发闹钟事件来打开电机
        }            
    }
    timer_cnt_value = timer_pod_value*3600; //装填递减值
    //初始化背景颜色
    parmater_color_fram = rgb_24to16(0xb5b5b5);
    parmater_color_back = rgb_24to16(0xe0e0e0);
    lcd_fill(0, 0, 128, 16, GREEN); //填充状态栏
    #if(LOCK_SWITCH) //如果开启锁屏，先进入锁屏界面
    flag_updat_ui = 1;
    show_ui_index = 10;
    #else //否则初始化设置菜单
    set_menu_init(); //设置菜单初始化
    #endif
    //stmflash_write(FLASH_START_ADDR, (uint16_t*)alarm, 5); // 闹钟写入FLASH
    flag_sys_init = 1; //系统初始化完成，定时器开启
    adc_updat(); //更新ADC
    updat_capcity_vol(); //更新电量
    DHT11_REC_Data(); //更新温湿度    
    while(1)
    {
        //更新输入事件
        if(flag_400ms) //400ms间隔
        {
            DHT11_REC_Data(); //更新温湿度
            adc_updat(); //更新ADC
            updat_capcity_vol(); //更新电量 
            rtc_get_time(); // 更新时间 
            flag_400ms = 0;
        }   
        
        //更新输出事件
        if(flag_100ms)
        {
            // 蜂鸣器
            if(!beep_sta)BEEP(CLOSE); //关闭蜂鸣器
            else if(beep_sta)BEEP(OPEN); //打开蜂鸣器
            // 指示灯
            if(battery_vol < 3.0f)LED_BLUE(OPEN); // 电池电压过低时，红灯亮 
            else LED_BLUE(CLOSE);  
            // 检测水槽里面还有没有水
            if(adc_value0 > GROOVE_T)
            {
                //
            }
            scan_motor_event(); //电机事件
            show_batery();  //显示电量
            show_time();    //显示时间 
            lcd_show_num(50, 0, lowpower_time, 2, 12, RED, GREEN);
            
            flag_100ms = 0;
        }
        
        //界面扫描
        switch(show_ui_index)
        {
            case 1: show_parmater_ui(); break; //参数界面
            case 2: sound_set_menu(); break; //声音设置
            case 3: clock_set_menu(); break; //时间设置
            case 4: motor_sta_ui(); break; //电机状态
            case 5: motor_speed_menu(); break; //电机速度设置
            case 6: dwater_alarm_ui(); break; //闹钟设置
            case 7: dwater_tim_menu(); break; //定时时间设置
            case 8: dwater_auto_menu(); break; //自动模式设置
            case 9: low_power_menu(); break; //低功耗设置
            case 10: lock_menu_show(); break; //锁屏界面
            case 11: dwater_al_ato_ui(); break; //闹钟+自动设置
            default: break;
        }
        #if(LOCK_SWITCH)
        if(lock_code_is)sip_timer_handle(); //响应菜单事件 //如果锁屏开启且密码正确
        #else 
        sip_timer_handle(); //响应菜单事件
        #endif
    }
}

/*********************************************功能函数**************************************************/
//函数功能：扫描电机事件
void scan_motor_event(void)
{
    switch(dwater_mode) //根据当前模式处理电机事件
    {
        case ALARM:   
            if(flag_alarm) //处理闹钟事件  
            {
                alarm_updat(); // 更新一次闹钟,下一个闹钟为当前时间加一天
                motor_handle.sta = OPEN;
                motor_handle.cnt_time = motor_handle.pod_time*10; //设置开启时间
                flag_alarm = 0;
            }
            break;
        case AUTO: 
            if(adc_value1 > FLOWER_T) motor_handle.sta = OPEN; //如果花盆里面没有水了                      
            else motor_handle.sta = CLOSE; 
            break;
        case TIMER: 
            if(timer_cnt_value > 0 && show_ui_index != 7)
            {
                if(flag_1s)
                {
                    flag_1s = 0;
                    timer_cnt_value--;  
                }
            }
            if(!timer_cnt_value && !flag_sur_timer) //定时时间到了打开电机
            {
                motor_handle.sta = OPEN;
                motor_handle.cnt_time = motor_handle.pod_time*10; //设置开启时间
                flag_sur_timer = 1;
            }
            break;
        case AL_ATO: 
            if(flag_alarm) //处理闹钟事件  
            {
                alarm_updat(); // 更新一次闹钟,下一个闹钟为当前时间加一天
                flag_alarm = 0;
            }
            if(adc_value1 > FLOWER_T) motor_handle.sta = OPEN; //如果花盆里面没有水了                      
            else motor_handle.sta = CLOSE; 
            break;
        case TI_ATO: break;
        default: break;
    }
    // 电机控制
    if(show_ui_index == 4) //电机控制界面下，电机只能通过设置控制
    {
        if(motor_handle.sta)motor_speed(motor_handle.speed);
        else motor_speed(CLOSE);     
    }
    else //否则通过其他事件控制
    {
        if(motor_handle.cnt_time > 0 && (dwater_mode == ALARM || dwater_mode == TIMER))
        {
            motor_handle.cnt_time--;
            motor_speed(motor_handle.speed);
        }
        else if(dwater_mode == ALARM || dwater_mode == TIMER)motor_speed(CLOSE);
        if((dwater_mode == AUTO || dwater_mode == AL_ATO)&& motor_handle.sta)motor_speed(motor_handle.speed);
        else if((dwater_mode == AUTO || dwater_mode == AL_ATO) && !motor_handle.sta)motor_speed(CLOSE);
    }
}

// 函数功能：自动设置下一个闹钟时间, 闹钟为当前时间的下一天
void alarm_updat(void)
{   
    uint32_t seccount = 0; //秒
    stmflash_read(FLASH_START_ADDR, (uint16_t*)alarm, 5); /* 读FLASH内的闹钟数据 */
    seccount = rtc_date2sec(calendar.year, calendar.month, calendar.date, alarm[3], alarm[4], 0); /* 将其转为秒数 */
    seccount += 24*60*60; /* 将当前时间加一天，设置为当前时间 */
    sec_to_time(seccount); /* 更新当前时间(这里先将当前时间设置为下一个闹钟，之后会重新还原) */
    rtc_set_alarm(calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, 0); /* 将当前时间设置为闹钟 */
    /* 更新闹钟 */
    alarm[0] = calendar.year;
    alarm[1] = calendar.month;
    alarm[2] = calendar.date;
    alarm[3] = alarm[3]; /* 时不改变 */
    alarm[4] = alarm[4]; /* 分不改变 */
    stmflash_write(FLASH_START_ADDR, (uint16_t*)alarm, 5); /* 新的闹钟写入FLASH，以备下一次用 */
    seccount -= 24*60*60; /* 将当前时间减一天，设置为当前时间 */
    sec_to_time(seccount); /* 重新设置好时间 */
    
    // 显示下一个闹钟 
    lcd_fill(5, 115, 115, 154, rgb_24to16(0xd9d9d9));
    lcd_show_dat(10, 120, "下一个闹钟:", 12, 0, BLACK, rgb_24to16(0xd9d9d9));
    lcd_show_num(10, 140, alarm[0], 4, 12, RED, rgb_24to16(0xd9d9d9));
    lcd_show_char(35, 140, '-', 12, 0, RED, rgb_24to16(0xd9d9d9));
    lcd_show_num(41, 140, alarm[1], 2, 12, RED, rgb_24to16(0xd9d9d9));
    lcd_show_char(53, 140, '-', 12, 0, RED, rgb_24to16(0xd9d9d9));
    lcd_show_num(59, 140, alarm[2], 2, 12, RED, rgb_24to16(0xd9d9d9));
    lcd_show_char(71, 140, '-', 12, 0, RED, rgb_24to16(0xd9d9d9));
    lcd_show_num(77, 140, alarm[3], 2, 12, RED, rgb_24to16(0xd9d9d9));
    lcd_show_char(89,140, '-', 12, 0, RED, rgb_24to16(0xd9d9d9));
    lcd_show_num(95, 140, alarm[4], 2, 12, RED, rgb_24to16(0xd9d9d9));
    flag_alarm_lable = 1; 
}

// 函数功能：更新ADC
void adc_updat(void)
{
    if(g_adc_dma_sta)
    {
        get_adc_channel_value(&adc_value0, ADC_CHANNEL_0);
        get_adc_channel_value(&adc_value1, ADC_CHANNEL_1);
        get_adc_channel_value(&adc_value2, ADC_CHANNEL_2);
        adc_dma_enable(ADC_BUFFER_SIZE);
        g_adc_dma_sta = 0;
    }
}

/*******************************************界面管理*******************************************/

// 函数功能：输入密码界面
void lock_menu_show(void)
{
    static uint8_t code[6] = {0}; /* 储存输入的密码 */
    static uint8_t code_v = 0; /* 密码匹配值 */
    static uint8_t i = 0; /* 控制数组下标 */
    static _Bool dir = 0; /* 光标闪烁状态 */
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;
        i = 0;
        code_v = 0;
        lcd_clear(BLACK); /* 绘制背景 */
        lcd_fill(8, 58, 120, 102, rgb_24to16(0xd9d9d9)); /* 绘制选择界面,深灰 */
        lcd_fill(10, 60, 118, 100, rgb_24to16(0xdadada)); /* 绘制选择界面，浅灰 */
        lcd_fill(20, 93, 108, 93, BLACK); /* 绘制 '--------' */
        lcd_show_dat(20, 60, "输入密码:123123", 12, 0, RED, rgb_24to16(0xdadada));
    }
    if(key_event)
    {
        code[i] = key_event;
        key_event = 0;
        lcd_show_num(20+i*12+2, 80, code[i], 1, 12, BLACK, rgb_24to16(0xdadada));
        i++;
    }
    if(i >= 6) /* 输入完后，进行密码验证 */
    {
        for(uint8_t j = 0; j < 6; j++)
        {
            if(code[j] == lock_code[j]) /* 逐个匹配 */
            {
                show_ui_index = 0;
                code_v++;
                if(code_v == 6)
                {
                    lock_code_is = 1; /* 密码正确 */
                    set_menu_init();
                }
            }
            else /* 只要有一个不匹配，则认为密码错误 */
            {
                i = 0;
                lcd_show_dat(30, 30, "密码错误!", 12, 0, RED, WHITE); 
                delay_ms(3000); /* 维持显示 */
                lcd_clear(BLACK); /* 清屏 */
                lcd_fill(8, 58, 120, 102, rgb_24to16(0xd9d9d9)); /* 绘制选择界面,深灰 */
                lcd_fill(10, 60, 118, 100, rgb_24to16(0xdadada)); /* 绘制选择界面，浅灰 */
                lcd_fill(20, 93, 108, 93, BLACK); /* 绘制 '--------' */
                lcd_show_dat(20, 60, "输入密码:123123", 12, 0, RED, rgb_24to16(0xdadada));
            }
        }
    }
    if(flag_500ms) /* 光标闪烁时间到 */
    {
        flag_500ms = 0;
        dir = !dir;
        if(dir)lcd_show_dat(20+i*12+2, 80, "|", 12, 0, BLACK, rgb_24to16(0xdadada));         
        else  lcd_show_dat(20+i*12+2, 80, " ", 12, 0, rgb_24to16(0xdadada), rgb_24to16(0xdadada));
    }
}

// 函数功能：低功耗模式设置界面
void low_power_menu(void)
{
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;
        lcd_fill(50, 60, 110, 90, rgb_24to16(0xc5d4f2)); /* 绘制选择界面，浅蓝色 */
    }
    if(key_event == 1||key_event == 2)
    {
        key_event = 0;
        lowpower_is = !lowpower_is;
    }
    else if(key_event == 3) /* 退出设置 */
    {
        key_event = 0;
        show_ui_index = 0;
        set_menu_init();
    }
    if(lowpower_is) lcd_show_dat(70, 70, "开", 12, 0, RED, rgb_24to16(0xc5d4f2));      
    else lcd_show_dat(70, 70, "关", 12, 0, RED, rgb_24to16(0xc5d4f2));
}

// 函数功能：自动模式设置界面
void dwater_auto_menu(void)
{
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;
        lcd_fill(50, 60, 110, 90, rgb_24to16(0xc5d4f2)); /* 绘制选择界面，浅蓝色 */
    }
    if(key_event == 1||key_event == 2) /* 状态翻转 */
    {
        key_event = 0;
        if(dwater_mode == AUTO)dwater_mode = CLOSE_D; /* 关闭浇水功能 */
        else dwater_mode = AUTO; /* 设置为自动浇水 */                 
    }
    else if(key_event == 3) /* 退出设置 */
    {
        key_event = 0;
        show_ui_index = 0;
        dwater_menu_init();
    }    
    if(dwater_mode == AUTO)lcd_show_dat(70, 70, "开", 12, 0, RED, rgb_24to16(0xc5d4f2));
    else lcd_show_dat(70, 70, "关", 12, 0, RED, rgb_24to16(0xc5d4f2));        
}

// 函数功能：定时模式设置界面
void dwater_tim_menu(void)
{
    static _Bool dir = 0;
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;
        lcd_fill(50, 60, 110, 90, rgb_24to16(0xc5d4f2)); /* 绘制选择界面，浅蓝色 */
        lcd_show_dat(80, 70, "时", 12, 0, RED, rgb_24to16(0xc5d4f2));
    }
    if(key_event == 1) /* 加 */
    {
        key_event = 0;
        flag_sur_timer = 0; 
        dwater_mode = TIMER; /* 发生时间设置时将模式设置为定时模式 */
        if(timer_pod_value < 255)timer_pod_value++;
        else timer_pod_value = 0;      
    }
    else if(key_event == 2) /* 减 */
    {
        key_event = 0;
        flag_sur_timer = 0;
        dwater_mode = TIMER; /* 发生时间设置时将模式设置为定时模式 */
        if(timer_pod_value > 0)timer_pod_value--;        
        else timer_pod_value = 255;
    }
    else if(key_event == 3) /* 退出设置 */
    {
        key_event = 0;
        show_ui_index = 0;
        timer_cnt_value = timer_pod_value*3600; //更新递减值
        dwater_menu_init();
    }
    if(flag_500ms)
    {
        flag_500ms = 0; 
        dir = !dir;
    }        
    if(dir)lcd_show_num(60, 70, timer_pod_value, 3, 12, RED, BLUE);
    else lcd_show_num(60, 70, timer_pod_value, 3, 12, RED, color_24to16(0xc5d4f2));
}

// 函数功能：闹钟模式设置界面
// 注：设置的闹钟日期从当前日期开始
void dwater_alarm_ui(void)
{
    static uint8_t chi = 0; //时间单位选择：0时，1分
    static _Bool dir = 0; /* 闪烁方向选择 */
    if(flag_updat_ui)
    {
        chi = 0; dir = 0;
        flag_updat_ui = 0;
        stmflash_read(FLASH_START_ADDR, (uint16_t*)alarm, 5); //读取闹钟
        lcd_fill(50, 60, 110, 90, rgb_24to16(0xc5d4f2)); /* 绘制背景，浅蓝色 */
        lcd_show_char(78, 76, ':', 12, 0, RED, color_24to16(0xc5d4f2));
        lcd_show_dat(60, 64, "时", 12, 0, RED, color_24to16(0xc5d4f2));
        lcd_show_num(56, 76, alarm[3], 3, 12, RED, color_24to16(0xc5d4f2));
        lcd_show_dat(84, 64, "分", 12, 0, RED, color_24to16(0xc5d4f2));
        lcd_show_num(80, 76, alarm[4], 3, 12, RED, color_24to16(0xc5d4f2));
    }
    if(key_event == 1) /* 加 */
    {
        key_event = 0;
        dwater_mode = ALARM; /* 发生时间设置时将模式设置为闹钟模式 */
        if(chi == 0) /* 时 */
        {
            if(alarm[3] < 23) alarm[3]++;
            else alarm[3] = 0;      
        }
        else if(chi == 1) /* 分 */
        {
            if(alarm[4] < 59)alarm[4]++;
            else alarm[4] = 0;
        }
    }
    else if(key_event == 2) /* 减 */
    {
        key_event = 0;
        dwater_mode = ALARM; /* 发生时间设置时将模式设置为闹钟模式 */
        if(chi == 0) /* 时 */
        {
            if(alarm[3] > 0)alarm[3]--;
            else alarm[3] = 23;     
        }
        else if(chi == 1) /* 分 */
        {
            if(alarm[4] > 0)alarm[4]--;
            else alarm[4] = 59;    
        }                       
    }
    else if(key_event == 3) /* 确定 */
    {
        key_event = 0;
        if(chi == 2) /* 退出 */
        {                
            if(dwater_mode == 0) /* 设置闹钟 */
            {
                stmflash_write(FLASH_START_ADDR, (uint16_t*)alarm, 5); /* 将新的闹钟写入FLASH */
                rtc_set_alarm(calendar.year, calendar.month, calendar.date, alarm[3], alarm[4], 0);  /* 设置闹钟 */     
                __HAL_RTC_ALARM_ENABLE_IT(&g_rtc_handle, RTC_IT_ALRA); //打开闹钟中断
            }
            show_ui_index = 0;
            dwater_menu_init();
        }
        chi++;
    }

    if(flag_500ms) /* 闪烁时间到 */
    {          
        flag_500ms = 0;
        dir = !dir;
    }    
    if(dir) /* 显示光标 */ 
    {
        if(chi == 0)
        {
            lcd_show_num(80, 76, alarm[4], 3, 12, RED, color_24to16(0xc5d4f2));
            lcd_show_num(56, 76, alarm[3], 3, 12, RED, BLUE); 
        }
        else if(chi == 1)
        {
            lcd_show_num(56, 76, alarm[3], 3, 12, RED, color_24to16(0xc5d4f2)); 
            lcd_show_num(80, 76, alarm[4], 3, 12, RED, BLUE);
        }
    }
    else /* 不显示光标 */
    {
        if(chi == 0)lcd_show_num(56, 76, alarm[3], 3, 12, RED, color_24to16(0xc5d4f2));           
        else if(chi == 1)lcd_show_num(80, 76, alarm[4], 3, 12, RED, color_24to16(0xc5d4f2)); 
    } 
}

// 函数功能：闹钟+自动模式设置界面
// 注：设置的闹钟日期从当前日期开始
void dwater_al_ato_ui(void)
{
    static uint8_t chi = 0; //时间单位选择：0时，1分
    static _Bool dir = 0; /* 闪烁方向选择 */
    if(flag_updat_ui)
    {
        chi = 0; dir = 0;
        flag_updat_ui = 0;
        stmflash_read(FLASH_START_ADDR, (uint16_t*)alarm, 5);
        lcd_fill(50, 50, 110, 90, rgb_24to16(0xc5d4f2)); /* 绘制背景，浅蓝色 */
        lcd_show_dat(55, 52, "自动+闹钟", 12, 0, RED, color_24to16(0xc5d4f2));
        lcd_show_char(78, 76, ':', 12, 0, RED, color_24to16(0xc5d4f2));
        lcd_show_dat(60, 64, "时", 12, 0, RED, color_24to16(0xc5d4f2));
        lcd_show_num(56, 76, alarm[3], 3, 12, RED, color_24to16(0xc5d4f2));
        lcd_show_dat(84, 64, "分", 12, 0, RED, color_24to16(0xc5d4f2));
        lcd_show_num(80, 76, alarm[4], 3, 12, RED, color_24to16(0xc5d4f2));
    }
    if(key_event == 1) /* 加 */
    {
        key_event = 0;
        dwater_mode = AL_ATO; /* 发生时间设置时将模式设置为闹钟+自动模式 */
        if(chi == 0) /* 时 */
        {
            if(alarm[3] < 23) alarm[3]++;
            else alarm[3] = 0;      
        }
        else if(chi == 1) /* 分 */
        {
            if(alarm[4] < 59)alarm[4]++;
            else alarm[4] = 0;
        }
    }
    else if(key_event == 2) /* 减 */
    {
        key_event = 0;
        dwater_mode = AL_ATO; /* 发生时间设置时将模式设置为闹钟+自动模式 */
        if(chi == 0) /* 时 */
        {
            if(alarm[3] > 0)alarm[3]--;
            else alarm[3] = 23;     
        }
        else if(chi == 1) /* 分 */
        {
            if(alarm[4] > 0)alarm[4]--;
            else alarm[4] = 59;    
        }                       
    }
    else if(key_event == 3) /* 确定 */
    {
        key_event = 0;
        if(chi == 2) /* 退出 */
        {                
            if(dwater_mode == AL_ATO) /* 设置闹钟 */
            {
                stmflash_write(FLASH_START_ADDR, (uint16_t*)alarm, 5); /* 将新的闹钟写入FLASH */
                rtc_set_alarm(calendar.year, calendar.month, calendar.date, alarm[3], alarm[4], 0);  /* 设置闹钟 */     
                __HAL_RTC_ALARM_ENABLE_IT(&g_rtc_handle, RTC_IT_ALRA); //打开闹钟中断
            }
            show_ui_index = 0;
            dwater_menu_init();
        }
        chi++;
    }

    if(flag_500ms) /* 闪烁时间到 */
    {          
        flag_500ms = 0;
        dir = !dir;
    }         
    if(dir) /* 显示光标 */ 
    {
        if(chi == 0)
        {
            lcd_show_num(80, 76, alarm[4], 3, 12, RED, color_24to16(0xc5d4f2));
            lcd_show_num(56, 76, alarm[3], 3, 12, RED, BLUE); 
        }
        else if(chi == 1)
        {
            lcd_show_num(56, 76, alarm[3], 3, 12, RED, color_24to16(0xc5d4f2)); 
            lcd_show_num(80, 76, alarm[4], 3, 12, RED, BLUE);
        }
    }
    else /* 不显示光标 */
    {
        if(chi == 0)lcd_show_num(56, 76, alarm[3], 3, 12, RED, color_24to16(0xc5d4f2));           
        else if(chi == 1)lcd_show_num(80, 76, alarm[4], 3, 12, RED, color_24to16(0xc5d4f2)); 
    } 
}

// 函数功能：电机速度设置界面
void motor_speed_menu(void)
{
    static _Bool dir = 0; //闪烁
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;  
        lcd_fill(30, 60, 100, 90, rgb_24to16(0xc5d4f2)); /* 绘制选择界面，浅蓝色 */
        lcd_show_char(62, 70, '/', 12, 0, BLACK, rgb_24to16(0xc5d4f2));
        lcd_show_num(68, 70, motor_handle.max_speed, 4, 12, BLACK, color_24to16(0xc5d4f2)); /* 显示速度最大值 */
    }
    if(key_event == 1) /* 速度加 */
    {
        key_event = 0;
        if(motor_handle.speed < PWM_Period)motor_handle.speed += 10;
        else motor_handle.speed = 0;
    }
    else if(key_event == 2) /* 速度减 */
    {
        key_event = 0;
        if(motor_handle.speed > 10)motor_handle.speed -= 10;
        else motor_handle.speed = PWM_Period;
    }
    else if(key_event == 3) /* 退出电机速度设置 */
    {
        show_ui_index = 0;
        moter_menu_init();
        key_event = 0;
    }     
    if(flag_500ms)
    {
        flag_500ms = 0; 
        dir = !dir;
    }        
    if(dir)lcd_show_num(36, 70, motor_handle.speed, 4, 12, RED, BLUE);
    else lcd_show_num(36, 70, motor_handle.speed, 4, 12, RED, color_24to16(0xc5d4f2));
}

// 函数功能：时钟设置界面
void clock_set_menu(void)
{
    static uint8_t chi = 0; //时间单位选择：0年，1月，2日，3时，4分
    static uint16_t tim_buf[5] = {0}; /* 临时保存日期时间 */
    static _Bool dir = 0; /* 闪烁方向选择 */
    uint16_t color_back = color_24to16(0xe1e1e1); /* 背景颜色: 浅灰 */
    
    if(flag_updat_ui)
    {
        chi = 0; dir = 0;
        flag_updat_ui = 0;       
        tim_buf[0] = calendar.year;
        tim_buf[1] = calendar.month;
        tim_buf[2] = calendar.date;
        tim_buf[3] = calendar.hour;
        tim_buf[4] = calendar.min;
        lcd_draw_menu_ui(20, 50, 90, 60, rgb_24to16(0xb6b6b6), color_back);
        lcd_show_num(32, 62, tim_buf[0], 4, 12, RED, color_back);
        lcd_show_dat(56, 62, "-", 12, 0, RED, color_back);
        lcd_show_num(62, 62, tim_buf[1], 2, 12, RED, color_back);
        lcd_show_dat(76, 62, "-", 12, 0, RED, color_back);
        lcd_show_num(82, 62, tim_buf[2], 2, 12, RED, color_back);
        lcd_show_num(32, 80, tim_buf[3], 2, 12, RED, color_back);
        lcd_show_dat(44, 80, ":", 12, 0, RED, color_back);
        lcd_show_num(50, 80, tim_buf[4], 2, 12, RED, color_back);
    }

    if(key_event == 1) /* 加 */
    {
        switch(chi)
        {
            case 0: if(tim_buf[0] < 2100)tim_buf[0]++;   
                    else tim_buf[0] = 2100;      
                    lcd_show_num(32, 62, tim_buf[0], 4, 12, RED, BLUE); 
            break;
            case 1: if(tim_buf[1] < 12)tim_buf[1]++;               
                    else tim_buf[1] = 12;      
                    lcd_show_num(62, 62, tim_buf[1], 2, 12, RED, BLUE);            
            break;
            case 2: if(tim_buf[2] < 31)tim_buf[2]++;         
                    else tim_buf[2] = 31;   
                    lcd_show_num(82, 62, tim_buf[2], 2, 12, RED, BLUE); 
            break;
            case 3: if(tim_buf[3] < 23)tim_buf[3]++;
                    else tim_buf[3] = 0;  
                    lcd_show_num(32, 80, tim_buf[3], 2, 12, RED, BLUE); 
            break;
            case 4: if(tim_buf[4] < 59)tim_buf[4]++;
                    else tim_buf[4] = 0;         
                    lcd_show_num(50, 80, tim_buf[4], 2, 12, RED, BLUE); 
            break;           
        }
        key_event = 0;
    }
    else if(key_event == 2) /* 减 */
    {
        switch(chi)
        {
            case 0: if(tim_buf[0] > 2024)tim_buf[0]--;   
                    else tim_buf[0] = 2024;      
                    lcd_show_num(32, 62, tim_buf[0], 4, 12, RED, BLUE); 
            break;
            case 1: if(tim_buf[1] > 1)tim_buf[1]--;               
                    else tim_buf[1] = 1;      
                    lcd_show_num(62, 62, tim_buf[1], 2, 12, RED, BLUE);            
            break;
            case 2: if(tim_buf[2] > 1)tim_buf[2]--;         
                    else tim_buf[2] = 1;   
                    lcd_show_num(82, 62, tim_buf[2], 2, 12, RED, BLUE); 
            break;
            case 3: if(tim_buf[3] > 0)tim_buf[3]--;
                    else tim_buf[3] = 23;  
                    lcd_show_num(32, 80, tim_buf[3], 2, 12, RED, BLUE); 
            break;
            case 4: if(tim_buf[4] > 0)tim_buf[4]--;
                    else tim_buf[4] = 59;         
                    lcd_show_num(50, 80, tim_buf[4], 2, 12, RED, BLUE); 
            break;           
        }
        key_event = 0;
    }
    else if(key_event == 3) /* 确定 */
    {
        key_event = 0;
        if(chi == 5) /* 退出 */
        {
            rtc_set_time(tim_buf[0], tim_buf[1], tim_buf[2], tim_buf[3], tim_buf[4], 0); /* 设置时间*/
            set_menu_init();
            show_ui_index = 0;
        }
        chi++;
    }

    if(flag_500ms) /* 闪烁时间到 */
    {         
        flag_500ms = 0;
        dir = !dir;
        
    }  
    if(dir) /* 显示光标 */ 
    {
        switch(chi)
        {
            case 0: lcd_show_num(50, 80, tim_buf[4], 2, 12, RED, color_back);
                lcd_show_num(32, 62, tim_buf[0], 4, 12, RED, BLUE); break;
            case 1: lcd_show_num(32, 62, tim_buf[0], 4, 12, RED, color_back);
                lcd_show_num(62, 62, tim_buf[1], 2, 12, RED, BLUE); break;
            case 2: lcd_show_num(62, 62, tim_buf[1], 2, 12, RED, color_back);
                lcd_show_num(82, 62, tim_buf[2], 2, 12, RED, BLUE); break;
            case 3:lcd_show_num(82, 62, tim_buf[2], 2, 12, RED, color_back); 
                lcd_show_num(32, 80, tim_buf[3], 2, 12, RED, BLUE); break;
            case 4: lcd_show_num(32, 80, tim_buf[3], 2, 12, RED, color_back);
                lcd_show_num(50, 80, tim_buf[4], 2, 12, RED, BLUE); break;
        }
    }
    else /* 不显示光标 */
    {
        switch(chi)
        {
            case 0: lcd_show_num(32, 62, tim_buf[0], 4, 12, RED, color_back); break;
            case 1: lcd_show_num(62, 62, tim_buf[1], 2, 12, RED, color_back); break;
            case 2: lcd_show_num(82, 62, tim_buf[2], 2, 12, RED, color_back); break;
            case 3: lcd_show_num(32, 80, tim_buf[3], 2, 12, RED, color_back); break;
            case 4: lcd_show_num(50, 80, tim_buf[4], 2, 12, RED, color_back); break;
        }
    }
}

// 函数功能：声音设置界面
void sound_set_menu(void)
{
    //静态界面初始化
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;
        lcd_fill(50, 60, 110, 90, rgb_24to16(0xc5d4f2)); /* 绘制背景，浅蓝色 */ 
    }
    if(key_event == 1 || key_event == 2)
    {
        beep_sta = !beep_sta;
        key_event = 0;
    }
    else if(key_event == 3)
    { 
        key_event = 0;
        show_ui_index = 0;
        set_menu_init();
    }
    //更新动态界面
    if(beep_sta)lcd_show_dat(70, 70, "开", 12, 0, RED, rgb_24to16(0xc5d4f2));
    else lcd_show_dat(70, 70, "关", 12, 0, RED, rgb_24to16(0xc5d4f2));
}

// 函数功能：显示参数界面
void show_parmater_ui(void)
{   
    //静态界面初始化
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;
        lcd_fill(20, 30, 120, 154, parmater_color_back); /* 绘制背景 */
        lcd_fill(19, 29, 121, 29, parmater_color_fram); /* 画上边 */
        lcd_fill(19, 155, 121, 155, parmater_color_fram); /* 下边 */
        lcd_fill(19, 30, 19, 154, parmater_color_fram); /* 左边 */
        lcd_fill(121, 30, 121, 154, parmater_color_fram); /* 右边 */

        stmflash_read(FLASH_START_ADDR, (uint16_t*)alarm, 5); /* 读FLASH内的闹钟数据 */   
        /* 显示温湿度 */
        lcd_show_dat(22, 32, "湿度:", parmater_size, 0, parmater_color_word, parmater_color_back);   
        lcd_show_dat(22, 44, "温度:", parmater_size, 0, parmater_color_word, parmater_color_back);
        lcd_show_char(72, 44, '.', parmater_size, 0, parmater_color_word, parmater_color_back); 
        /* 显示工作模式 */
        lcd_show_dat(22, 56, "模式:", parmater_size, 0, parmater_color_word, parmater_color_back);
        if(dwater_mode == AUTO)lcd_show_dat(54, 56, "自动", parmater_size, 0, parmater_color_word, parmater_color_back);
        else if(dwater_mode == TIMER)
        {
            lcd_show_dat(54, 56, "定时     分", parmater_size, 0, parmater_color_word, parmater_color_back);
            lcd_show_num(78, 56, timer_cnt_value/60, 4, parmater_size, parmater_color_num, parmater_color_back);
        }
        else if(dwater_mode == ALARM)
        {        
            lcd_show_dat(54, 56, "闹钟", parmater_size, 0, parmater_color_word, parmater_color_back);
            lcd_show_num(80, 56, alarm[3], 2, parmater_size, parmater_color_num, parmater_color_back);
            lcd_show_char(98, 56, ':', 12, 0, parmater_color_num, parmater_color_back);
            lcd_show_num(104, 56, alarm[4], 2, parmater_size, parmater_color_num, parmater_color_back);
        } 
        else if(dwater_mode == AL_ATO)
        {        
            lcd_show_dat(54, 56, "自闹", parmater_size, 0, parmater_color_word, parmater_color_back);
            lcd_show_num(80, 56, alarm[3], 2, parmater_size, parmater_color_num, parmater_color_back);
            lcd_show_char(98, 56, ':', 12, 0, parmater_color_num, parmater_color_back);
            lcd_show_num(104, 56, alarm[4], 2, parmater_size, parmater_color_num, parmater_color_back);
        }         
        /* 显示当前密码 */
        lcd_show_dat(22, 68, "密码:", parmater_size, 0, parmater_color_word, parmater_color_back);
        for(uint8_t i = 0; i < 6; i++)
            lcd_show_num(54+i*6, 68, lock_code[i], 1, parmater_size, parmater_color_num, parmater_color_back);
        /* 显示声音 */
        lcd_show_dat(22, 80, "声音:", parmater_size, 0, parmater_color_word, parmater_color_back);    
        if(beep_sta)lcd_show_dat(54, 80, "开", parmater_size, 0, parmater_color_word, parmater_color_back);      
        else lcd_show_dat(54, 80, "关", parmater_size, 0, parmater_color_word, parmater_color_back);                
        /* 显示日期 */
        lcd_show_dat(22, 104, "日期:", parmater_size, 0, parmater_color_word, parmater_color_back);   
        lcd_show_num(50, 104, calendar.year, 4, parmater_size, parmater_color_num, parmater_color_back);   
        lcd_show_char(76, 104, '-', parmater_size, 0, parmater_color_num, parmater_color_back);   
        lcd_show_num(80, 104, calendar.month, 2, parmater_size, RED, parmater_color_back); 
        lcd_show_char(96, 104, '-', parmater_size, 0, parmater_color_num, parmater_color_back);   
        lcd_show_num(102, 104, calendar.date, 2, parmater_size, parmater_color_num, parmater_color_back);     
        /* 显示adc三个通道的原始值 */
        lcd_show_dat(22, 140, "ch1:", parmater_size, 0, parmater_color_word, parmater_color_back);                   
        lcd_show_dat(22, 128, "ch2:", parmater_size, 0, parmater_color_word, parmater_color_back);               
        lcd_show_dat(22, 116, "ch3:", parmater_size, 0, parmater_color_word, parmater_color_back);
        lcd_show_dat(22, 92, "电机速度:", parmater_size, 0, parmater_color_word, parmater_color_back);   
        lcd_show_num(76, 92, motor_handle.speed, 4, parmater_size, parmater_color_num, parmater_color_back);         
    }        
    
    if(key_event == 3)
    { 
        key_event = 0;
        show_ui_index = 0;
        set_menu_init();
    }
    //更新动态界面
    //温湿度
    lcd_show_num(54, 32, rec_data[0], 2, parmater_size, parmater_color_num, parmater_color_back); 
    lcd_show_num(54, 44, rec_data[2], 2, parmater_size, parmater_color_num, parmater_color_back); 
    //ADC原始值
    lcd_show_num(48, 116, adc_value0, 4, parmater_size, parmater_color_num, parmater_color_back);  
    lcd_show_num(48, 128, adc_value1, 4, parmater_size, parmater_color_num, parmater_color_back);
    lcd_show_num(48, 140, adc_value2, 4, parmater_size, parmater_color_num, parmater_color_back);
}

// 函数功能：显示电量
void show_batery(void)
{   
    lcd_show_dat(bat_x, bat_y, "电量:", bat_size, 0, bat_word_color, bat_back_color);
    lcd_show_num(bat_x + 2.5*(bat_size+1), bat_y, capcity_bfb, 2, bat_size, bat_word_color, bat_back_color);
    lcd_show_char(bat_x + 3.5*(bat_size + 1), bat_y, '%', bat_size, 0, bat_word_color, bat_back_color);
}

// 函数功能：显示时间
void show_time(void)
{
    static uint8_t fre_sec = 0;
    static _Bool dir = 0;
    lcd_show_num(time_x, time_y, calendar.hour, 2, time_size, time_word_color, time_back_color);
    if(fre_sec != calendar.sec)dir = !dir;
    fre_sec = calendar.sec;
    lcd_show_char(time_x + 2*(time_size/2)+1, time_y, ':', time_size, 0, dir?GREEN:time_word_color, time_back_color);
    lcd_show_num(time_x + 3*(time_size/2)+1, time_y, calendar.min, 2, time_size, time_word_color, time_back_color);  
}

// 函数功能：电机状态设置界面
void motor_sta_ui(void)
{
    //静态界面初始化
    if(flag_updat_ui)
    {
        flag_updat_ui = 0;
        lcd_fill(50, 60, 110, 90, rgb_24to16(0xc5d4f2)); /* 绘制选择界面，浅蓝色 */
    }

    if(key_event == 1|| key_event == 2)
    {
        motor_handle.sta = !motor_handle.sta; 
        key_event = 0;
    }        
    else if(key_event == 3) // 退出
    {
        show_ui_index = 0;
        moter_menu_init();
        key_event = 0;
    }
         
    if(motor_handle.sta)lcd_show_dat(70, 70, "开", 12, 0, RED, rgb_24to16(0xc5d4f2));       
    else lcd_show_dat(70, 70, "关", 12, 0, RED, rgb_24to16(0xc5d4f2));     
}

/***************************************菜单界面管理*******************************************/


// 函数功能：电机菜单按键事件处理
void moter_btn_event(void)
{
    uint8_t key_index = get_key_index(); /* 得到按键索引值 */
    if(key_index == 0)
    {
        flag_updat_ui = 1;
        show_ui_index = 4; 
    }
    else if(key_index == 1)
    {
        flag_updat_ui = 1;
        show_ui_index = 5;
    }
    else if(key_index == 2) /* 返回 */
    {
        set_menu_init();
    }
}

// 函数功能：电机菜单初始化
void moter_menu_init(void)
{
    pmoter = linklist_init();
    create_mylist(pmoter, "电机");
    set_size_list(90, 100);
    set_pos_list(20, 40);
    set_color_list(0xf6fa7d); /* 设置背景颜色: 黄色 */
    create_sonlist(pmoter, "状态", moter_btn_event);
    create_sonlist(pmoter, "速度", moter_btn_event);    
    create_sonlist(pmoter, "返回", moter_btn_event);    
}

// 函数功能：设置菜单按键事件处理
void set_btn_event(void)
{
    uint8_t key_index = get_key_index();
    if(key_index == 0)
    {
        flag_updat_ui = 1;
        show_ui_index = 1; 
    }
    else if(key_index == 1)
    {
        flag_updat_ui = 1;
        show_ui_index = 2; 
    }
    else if(key_index == 2)
    {
        flag_updat_ui = 1;
        show_ui_index = 3;
    }
    else if(key_index == 3)
    {
        dwater_menu_init();
    }
    else if(key_index == 4) 
    {
        /* 密码设置 */
    }
    else if(key_index == 5) 
    {
        moter_menu_init();
    }
    else if(key_index == 6) 
    {
        flag_updat_ui = 1;
        show_ui_index = 9;
    }
}

// 函数功能：设置菜单初始化
void set_menu_init(void)
{
    pset = linklist_init(); 
    create_mylist(pset, "设置");   /* 创建主菜单 */ 
    set_size_list(144, 128); /* 设置尺寸 */
    set_pos_list(0, 16); /* 设置初始位置 */
    set_color_list(0xead6a3); /* 设置背景颜色: 浅棕色 */
    //set_text_color(0xc75050);
//    set_title_color(0x444ba7);
//    set_text_size(16);
//    set_title_size(24);
    create_sonlist(pset, "参数", set_btn_event); /* 创建菜单栏 */
    create_sonlist(pset, "声音", set_btn_event);
    create_sonlist(pset, "时钟", set_btn_event);
    create_sonlist(pset, "浇水模式", set_btn_event);
    create_sonlist(pset, "密码",set_btn_event);
    create_sonlist(pset, "电机",set_btn_event);     
    create_sonlist(pset, "低功耗",set_btn_event); 
    create_sonlist(pset, "不知道1",NULL); 
    create_sonlist(pset, "不知道2",NULL); 
    create_sonlist(pset, "不知道3",NULL); 
    create_sonlist(pset, "不知道4",NULL); 
    create_sonlist(pset, "不知道5",NULL); 
    create_sonlist(pset, "不知道6",NULL); 
}

// 函数功能：浇水模式菜单栏按键事件
void dwater_btn_event(void)
{
    uint8_t key_index = get_key_index();
    if(key_index == 0) 
    {
        flag_updat_ui = 1;
        show_ui_index = 8;
    }
    else if(key_index == 1) 
    {
        flag_updat_ui = 1;
        show_ui_index = 7;
    }
    else if(key_index == 2) 
    {
        flag_updat_ui = 1;
        show_ui_index = 6;
    }
    else if(key_index == 3)
    {
        flag_updat_ui = 1;
        show_ui_index = 11;
    }
    else if(key_index == 4) /* 返回 */
    {
        set_menu_init();
    }
}

// 函数功能：浇水模式菜单初始化
void dwater_menu_init(void)
{
    pdwater = linklist_init();
    create_mylist(pdwater, "浇水模式");
    set_size_list(100, 100);
    set_pos_list(20, 40);
    set_color_list(0xf6fa7d); /* 设置背景颜色: 黄色 */
    /* 根据模式值初始化 */
    if(dwater_mode == AUTO)
    {
        create_sonlist(pdwater, "自动<--", dwater_btn_event);
        create_sonlist(pdwater, "定时", dwater_btn_event);    
        create_sonlist(pdwater, "闹钟", dwater_btn_event);   
        create_sonlist(pdwater, "自动+闹钟", dwater_btn_event); 
    }
    else if(dwater_mode == TIMER)
    {
        create_sonlist(pdwater, "自动", dwater_btn_event);
        create_sonlist(pdwater, "定时<--", dwater_btn_event);    
        create_sonlist(pdwater, "闹钟", dwater_btn_event); 
        create_sonlist(pdwater, "自动+闹钟", dwater_btn_event); 
    }
    else if(dwater_mode == ALARM)
    {
        create_sonlist(pdwater, "自动", dwater_btn_event);
        create_sonlist(pdwater, "定时", dwater_btn_event);    
        create_sonlist(pdwater, "闹钟<--", dwater_btn_event);
        create_sonlist(pdwater, "自动+闹钟", dwater_btn_event);         
    }
    else if(dwater_mode == AL_ATO)
    {
        create_sonlist(pdwater, "自动", dwater_btn_event);
        create_sonlist(pdwater, "定时", dwater_btn_event);    
        create_sonlist(pdwater, "闹钟", dwater_btn_event);
        create_sonlist(pdwater, "自动+闹钟<-", dwater_btn_event); 
    }
    else
    {
        create_sonlist(pdwater, "自动", dwater_btn_event);
        create_sonlist(pdwater, "定时", dwater_btn_event);    
        create_sonlist(pdwater, "闹钟", dwater_btn_event); 
        create_sonlist(pdwater, "自动+闹钟", dwater_btn_event); 
    }
    create_sonlist(pdwater, "返回", dwater_btn_event);  
}

