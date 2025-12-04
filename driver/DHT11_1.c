#include "DHT11.h"
#include "delay.h" // 确保这里引用了你刚才发的 delay.c 对应的头文件
#include "usart.h"
// 引脚定义：PB8
#define DHT11_PIN    GPIO_Pin_8
#define DHT11_PORT   GPIOB

// 方便操作
#define DHT11_High()  GPIO_SetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_Low()   GPIO_ResetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_Read()  GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)

// 模式切换函数
void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

// ==========================================
// 重点看这里！！！ 重点看这里！！！
// ==========================================
void DHT11_Rst(void)
{
    DHT11_Mode_Out_PP(); 
    DHT11_Low();         // 拉低总线
    
    // -------------------------------------------------------
    // 你的原代码可能是 delay_us(20);  <-- 这是错的！只有0.02毫秒
    // 必须改成 delay_ms(20);          <-- 这是对的！20毫秒
    // -------------------------------------------------------
    delay_ms(20);        
    
    DHT11_High();        // 拉高释放
    delay_us(30);        // 等待 30微秒，让DHT11反应过来
}


u8 DHT11_Check(void)
{
    u8 retry=0;
    DHT11_Mode_IPU(); // 切换输入模式
    
    // 1. 等待 DHT11 拉低 (回应信号)
    // 刚才Wave里的那些 0 就是在这里被检测到的
    while (DHT11_Read() && retry<100) 
    {
        retry++;
        delay_us(1);
    }
    if(retry>=100) return 1; 
    else retry=0;
    
    // 2. 等待 DHT11 拉高 (回应结束)
    // 刚才Wave里的那些 1 就是在这里被检测到的
    while (!DHT11_Read() && retry<100) 
    {
        retry++;
        delay_us(1);
    }
    if(retry>=100) return 1;
    
    return 0; // 握手成功，准备读数据！
}
// 读一位 (Bit)
u8 DHT11_Read_Bit(void)
{
    u8 retry=0;
    while(DHT11_Read() && retry<100) // 等低电平
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!DHT11_Read() && retry<100) // 等高电平
    {
        retry++;
        delay_us(1);
    }
    
    // 延时 40us 采样
    delay_us(40);
    
    if(DHT11_Read()) return 1;
    else return 0;
}

// 读字节
u8 DHT11_Read_Byte(void)
{
    u8 i, dat;
    dat=0;
    for (i=0; i<8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

// 读数据主体
u8 DHT11_Read_Data(u8 *temp, u8 *humi)
{
    u8 buf[5];
    u8 i;
    
    DHT11_Rst(); // 发送启动信号
    
    if(DHT11_Check() == 0) // 如果有人回应
    {
        for(i=0; i<5; i++)
        {
            buf[i] = DHT11_Read_Byte();
        }
        
        // 校验
        if((buf[0]+buf[1]+buf[2]+buf[3]) == buf[4])
        {
            *temp = buf[0];
            *humi = buf[2];
        }
    }
    else return 1; // 失败
    
    return 0;
}

// 初始化
u8 DHT11_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    DHT11_Mode_Out_PP();
    DHT11_High();
    
    DHT11_Rst();  
    return DHT11_Check();
}


