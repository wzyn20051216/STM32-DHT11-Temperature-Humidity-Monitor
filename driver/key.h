#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

// 读取引脚状态的宏 (0:按下, 1:松开)
#define KEY1  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)
#define KEY2  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)
#define KEY3  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)
#define KEY4  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)

// 键码定义
#define KEY_NONE  0
#define KEY_1     1
#define KEY_2     2
#define KEY_3     3
#define KEY_4     4

// 函数声明
void Key_init(void);
uint8_t key_read(void);
void key_proc(void);
#endif