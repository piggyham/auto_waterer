#include "./BSP/DHT11/dht11.h"
#include "./SYSTEM/delay/delay.h"

//数据 [0]湿度高8位，[1]湿度低8位(全为0), [2]温度高8位(整数部分)，[3]温度低8位(小数部分)
uint8_t rec_data[4] = {0};

// 函数功能：DHT11初始化
void dht11_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	 __HAL_RCC_GPIOB_CLK_ENABLE();//GPIOA时钟使能
    
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_OD; //开漏输出
	GPIO_InitStructure.Pin   = GPIO_PIN_0;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//GPIOB初始化    
}

//主机发送开始信号
void DHT11_Start(void)
{
	dht11_high; //先拉高
	delay_us(30);
	
	dht11_low; //拉低电平至少18us
	delay_ms(20);
	
	dht11_high; //拉高电平20~40us
	delay_us(30);
}

//获取一个字节
char DHT11_Rec_Byte(void)
{
	uint8_t i = 0;
	uint8_t data;
	
	for(i = 0; i < 8; i++) //1个数据就是1个字节byte，1个字节byte有8位bit
	{
		while( Read_Data == 0); //从1bit开始，低电平变高电平，等待低电平结束
		delay_us(30); //延迟30us是为了区别数据0和数据1，0只有26~28us
		
		data <<= 1; //左移
		
		if( Read_Data == 1 ) //如果过了30us还是高电平的话就是数据1
		{
			data |= 1; //数据+1
		}
		
		while( Read_Data == 1 ); //高电平变低电平，等待高电平结束
	}
	
	return data;
}

//获取数据
void DHT11_REC_Data(void)
{
	uint16_t R_H,R_L,T_H,T_L;
	uint8_t CHECK;
	
	DHT11_Start(); //主机发送信号
	dht11_high; //拉高电平
	
	if( Read_Data == 0 ) //判断DHT11是否响应
	{
		while( Read_Data == 0); //低电平变高电平，等待低电平结束
		while( Read_Data == 1); //高电平变低电平，等待高电平结束
		
		R_H = DHT11_Rec_Byte();
		R_L = DHT11_Rec_Byte();
		T_H = DHT11_Rec_Byte();
		T_L = DHT11_Rec_Byte();
		CHECK = DHT11_Rec_Byte(); //接收5个数据
		
		dht11_low; //当最后一bit数据传送完毕后，DHT11拉低总线 50us
		delay_us(55); //这里延时55us
		dht11_high; //随后总线由上拉电阻拉高进入空闲状态。
		
		if(R_H + R_L + T_H + T_L == CHECK) //和检验位对比，判断校验接收到的数据是否正确
		{
            rec_data[0] = R_H;
            rec_data[1] = R_L;
            rec_data[2] = T_H;
            rec_data[3] = T_L;
		}
	}

}


