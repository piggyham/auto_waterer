# 自动浇水系统 v2.0

这是一个基于 STM32F103 的自动浇水系统 Keil 工程。项目使用 ST7735 彩屏作为显示界面，通过按键菜单完成参数查看、浇水模式设置、电机控制、低功耗设置等操作，并结合 ADC、RTC、DHT11、电池检测和水泵电机实现自动浇水逻辑。

## 主要功能

- ST7735 LCD 彩屏显示，支持 ASCII 和部分 GBK 汉字点阵显示
- 三按键菜单交互，支持菜单光标、滚动条和确认回调
- ADC + DMA 采集水槽、花盆、电池相关模拟量
- DHT11 温湿度读取
- RTC 时间读取与闹钟触发
- 电机 PWM 调速控制
- 蜂鸣器开关控制
- 电池电量估算与状态栏显示
- Standby 低功耗模式，支持 PA0/WKUP 唤醒
- 独立模块测试程序，便于逐个验证硬件功能

## 目录结构

```text
.
├── Drivers/                 # HAL、CMSIS、系统驱动和 BSP 外设驱动
│   ├── BSP/
│   │   ├── ADC/             # ADC + DMA 采样
│   │   ├── BATTERY/         # 电池电压/电量估算
│   │   ├── BEER/            # 蜂鸣器
│   │   ├── DHT11/           # 温湿度传感器
│   │   ├── DWATER/          # 自动浇水业务逻辑和 UI 页面
│   │   ├── FLASH/           # 片内 Flash 读写
│   │   ├── GTIM/            # TIM3 全局节拍
│   │   ├── KEY/             # 按键扫描
│   │   ├── LED/             # LED 和外围电源开关
│   │   ├── MENU/            # 通用链表菜单组件
│   │   ├── MOTER/           # 电机 PWM 控制
│   │   ├── PWR/             # 低功耗和唤醒
│   │   ├── RTC/             # RTC 时间和闹钟
│   │   ├── SPI/             # SPI 驱动
│   │   └── ST7735/          # LCD 驱动、字库和图片资源
│   └── SYSTEM/              # delay、sys、usart 等基础系统代码
├── Middlewares/MALLOC/      # 简易内存管理
├── Projects/MDK-ARM/        # Keil MDK 工程文件
├── Test/                    # 独立模块测试程序
├── User/                    # 主程序和中断配置
├── .gitignore               # Git 忽略规则
└── README.md
```

## 主程序流程

主入口位于 `User/main.c`。

系统上电后大致流程如下：

```text
main()
  ↓
检测唤醒源 check_wkup_source()
  ↓
初始化 HAL、时钟、delay、LED、按键、SPI、LCD 等外设
  ↓
初始化 RTC、ADC DMA、DHT11、蜂鸣器、菜单、电池、TIM3
  ↓
dwater_sys_init()
  ↓
进入自动浇水系统主循环
```

`dwater_sys_init()` 是应用层主循环，主要完成：

- 每 400ms 更新 DHT11、ADC、电池和 RTC 时间
- 每 100ms 更新蜂鸣器、LED、电机事件、状态栏时间和电量
- 根据 `show_ui_index` 进入不同 UI 页面
- 调用 `sip_timer_handle()` 处理菜单绘制和按键交互

## UI 结构

LCD 分辨率按代码配置为 `128 x 160`。

界面布局：

```text
顶部状态栏：时间 / 低功耗倒计时 / 电量
主显示区：菜单、参数页、设置页、弹窗页
```

核心 UI 文件：

- `Drivers/BSP/MENU/menu.c`：通用链表菜单、光标、滚动条、按键确认
- `Drivers/BSP/DWATER/dwater.c`：业务菜单和页面状态机
- `Drivers/BSP/ST7735/st7735.c`：LCD 基础绘制接口
- `Drivers/BSP/ST7735/lcdfont.h`：ASCII 与汉字点阵字库

`show_ui_index` 是页面索引：

```text
0  普通菜单界面
1  参数页
2  声音设置
3  时钟设置
4  电机状态
5  电机速度
6  闹钟设置
7  定时设置
8  自动浇水设置
9  低功耗设置
10 锁屏界面
11 自动+闹钟设置
```

## 浇水模式

浇水模式定义在 `Drivers/BSP/DWATER/dwater.h`：

```c
enum dwater_mode {
    ALARM,    // 闹钟
    AUTO,     // 自动
    TIMER,    // 定时
    AL_ATO,   // 闹钟 + 自动
    TI_ATO,   // 定时 + 自动
    CLOSE_D,  // 关闭
};
```

核心控制函数是 `scan_motor_event()`：

- `ALARM`：RTC 闹钟触发后，电机运行设定时长
- `AUTO`：根据花盆 ADC 阈值判断是否开泵
- `TIMER`：定时倒计时结束后，电机运行设定时长
- `AL_ATO`：闹钟更新下一次时间，电机由花盆 ADC 状态控制

## 模块测试程序

`Test/` 目录中提供了独立测试入口，每个文件都包含自己的 `main()`，用于单独验证模块。

```text
LCD_test.c            LCD 显示测试
key_test.c            按键测试
TIM3_test.c           TIM3 全局节拍测试
ADC_test.c            ADC DMA 采样测试
menu_test.c           菜单组件测试
buzzer_test.c         蜂鸣器测试
motor_test.c          电机 PWM 测试
dht11_test.c          DHT11 温湿度测试，带超时保护
RTC_test.c            RTC 走时测试
RTC_alarm_test.c      RTC 闹钟测试
flash_test.c          片内 Flash 读写测试
battery_test.c        电池电量测试
pwr_test.c            Standby 低功耗测试
dwater_logic_test.c   自动浇水逻辑测试
```

测试时注意：Keil 工程中同一时间只能有一个 `main()` 参与编译。  
例如测试 `motor_test.c` 时，需要把 `User/main.c` 和其他测试文件设置为 `Exclude from Build`，只保留 `motor_test.c`。

推荐测试顺序：

```text
LCD_test
key_test
TIM3_test
ADC_test
menu_test
buzzer_test
motor_test
dht11_test
RTC_test
RTC_alarm_test
flash_test
battery_test
pwr_test
dwater_logic_test
```

## 汉字显示说明

本项目 LCD 中文显示默认使用 GBK/GB2312 两字节编码：

```c
#define ENCODE 1
```

汉字点阵存放在：

```text
Drivers/BSP/ST7735/lcdfont.h
```

如果源码文件使用 UTF-8 直接写中文，可能会因为编码不匹配导致显示乱码或问号。测试程序中部分中文使用 GBK 字节转义，例如：

```c
#define LCD_GBK_POWER "\xB5\xE7\xC1\xBF"  // 电量
```

如果某个汉字显示不出来，需要确认对应字号的字库数组中是否包含该汉字点阵。

## 低功耗说明

系统低功耗逻辑位于 `Drivers/BSP/GTIM/gtim.c` 和 `Drivers/BSP/PWR/pwr.c`。

- `SWITCH(0)`：关闭外围电路
- `pwr_enter_standby()`：进入 Standby 待机模式
- `PA0/WKUP`：作为唤醒引脚
- Standby 唤醒后程序会类似复位，从 `main()` 重新开始执行

外围电路重新打开的位置在 `led_init()`：

```c
SWITCH(1);
```

## 构建与烧录

使用 Keil MDK 打开工程：

```text
Projects/MDK-ARM/auto_dwater(2).uvprojx
```

常见烧录注意事项：

- 确认芯片型号和 Flash Algorithm 与实际硬件一致
- 如果程序进入 Standby 后无法下载，尝试 `Connect under Reset`
- 如果屏幕只有复位时亮一下，检查 ST7735 背光 BLK 引脚电平极性
- 如果只显示白屏和顶部绿条，优先排查 DHT11 是否卡在无超时等待

## Git 忽略规则

`.gitignore` 已排除 Keil 编译产物和临时文件，特别是：

```text
Output/
*.o
*.d
*.crf
*.axf
*.hex
*.map
*.lst
```

仓库中保留源码、Keil 工程文件和 `Test/` 测试程序。
