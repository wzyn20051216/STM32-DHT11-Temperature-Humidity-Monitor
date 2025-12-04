#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include <stdio.h>

// 全局变量
uint8_t RxData; // 用来存当前收到的那一个字符

// 从main.c引入的全局参数
extern unsigned char menu;
extern unsigned char temp;
extern unsigned char humi;
extern unsigned char temp_high;
extern unsigned char temp_low;
extern unsigned char humi_high;
extern unsigned char humi_low;

/**
  * @brief  USART1 初始化配置
  * @param  bound: 波特率 ( 115200)
  */
void USART1_Init(void)
{
    // 1. 定义结构体变量
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
   
    // 2. 开启时钟 (USART1 和 GPIOA 都在 APB2)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 3. 配置 GPIO - TX (PA9) -> 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 4. 配置 GPIO - RX (PA10) -> 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 5. 配置 USART1 参数
    USART_InitStructure.USART_BaudRate = 115200;                     // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;     // 字长 8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;          // 停止位 1位
    USART_InitStructure.USART_Parity = USART_Parity_No;             // 无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式都开
    
    USART_Init(USART1, &USART_InitStructure);
		
		/*中断*/
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 开启接收中断 (RXNE)
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 6. 使能串口
    USART_Cmd(USART1, ENABLE);
}

// printf重定向
int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    
    // 2. 将数据写入寄存器
    USART_SendData(USART1, (uint8_t) ch);
    
    return ch;
}

// 发送字符串函数
void USART_SendString(unsigned char *str)
{
    while(*str != '\0')
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *str++);
    }
}

// 串口处理函数 - 发送系统状态信息
void uart_proc(void)
{
    char str[100];
    
    // 发送LED状态
    sprintf(str, "LED1:%d LED2:%d ", GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_7)?1:0, 
                                      GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6)?1:0);
    USART_SendString((unsigned char*)str);
    
    // 发送温湿度
    sprintf(str, "Temp:%d'C Humi:%d%% ", temp, humi);
    USART_SendString((unsigned char*)str);
    
    // 发送当前模式
    sprintf(str, "Mode:");
    USART_SendString((unsigned char*)str);
    
    if(menu == 0)
        USART_SendString((unsigned char*)"TEMP_DISP ");
    else if(menu == 1)
        USART_SendString((unsigned char*)"HUMI_DISP ");
    else if(menu == 2)
        USART_SendString((unsigned char*)"TEMP_H_SET ");
    else if(menu == 3)
        USART_SendString((unsigned char*)"TEMP_L_SET ");
    else if(menu == 4)
        USART_SendString((unsigned char*)"HUMI_H_SET ");
    else if(menu == 5)
        USART_SendString((unsigned char*)"HUMI_L_SET ");
    
    // 发送阈值
    sprintf(str, "TH:%d TL:%d HH:%d HL:%d\r\n", temp_high, temp_low, 
                                                 humi_high, humi_low);
    USART_SendString((unsigned char*)str);
}

// 串口中断服务函数
void USART1_IRQHandler(void)
{
    // 1. 判断是不是接收中断
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // 2. 读数据（这就相当于把快递拿进屋，标志位自动清除）
        RxData = USART_ReceiveData(USART1);
        
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}