#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"

// 函数声明
void led_init(void);
void led_proc(void);    // LED处理函数

// 宏定义 - LED引脚
#define LED0_PIN      GPIO_Pin_13
#define LED1_PIN      GPIO_Pin_6
#define LED2_PIN      GPIO_Pin_7

#define LED_GROUP1    GPIOC
#define LED_GROUP2    GPIOB

// 宏定义 - LED控制
//LED0(1)表示亮，LED0(0)表示灭
#define LED0(x)       (x==1) ?  GPIO_ResetBits(LED_GROUP1, LED0_PIN) : GPIO_SetBits(LED_GROUP1, LED0_PIN)
#define LED1(x)       (x==1) ?  GPIO_SetBits(LED_GROUP2, LED1_PIN) : GPIO_ResetBits(LED_GROUP2, LED1_PIN)
#define LED2(x)       (x==1) ?  GPIO_SetBits(LED_GROUP2, LED2_PIN) : GPIO_ResetBits(LED_GROUP2, LED2_PIN)

#endif
