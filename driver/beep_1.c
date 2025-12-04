#include "beep.h"
#include "stm32f10x.h"

extern unsigned char menu;
extern unsigned char temp;
extern unsigned char humi;
extern unsigned char temp_high;
extern unsigned char temp_low;
extern unsigned char humi_high;
extern unsigned char humi_low;

void Init_tiemr4(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 开启时钟 (PB口 和 TIM4)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 2. 配置 GPIO: 改为 PB9 (对应 TIM4_CH4)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 时基配置 (产生 1kHz 频率，适合蜂鸣器)
    // 72MHz / (71+1) = 1MHz (1us)
    // 1MHz / (999+1) = 1kHz (1ms)
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // 4. 输出比较配置:  通道4 (OC4)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 500; // 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    
    TIM_OC4Init(TIM4, &TIM_OCInitStructure); 
    
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    // 5. 使能定时器
    TIM_Cmd(TIM4, ENABLE);
}

void Open_Beep(void)
{
    TIM_SetCompare4(TIM4, 500);
}

void Close_Beep(void)
{
    TIM_SetCompare4(TIM4, 0);
}

void alarm_proc(void)
{
	  switch(menu)
		{
			case 0:
				if(temp > temp_high || temp < temp_low)
            Open_Beep();
        else
            Close_Beep();
			break;
			case 1:
				 if(humi > humi_high || humi < humi_low)
            Open_Beep();
        else
            Close_Beep();
			break;
			default:Close_Beep(); break;
		}
   
    
}
