#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include <stdio.h>  
void USART1_Init(void);
void USART_SendString(unsigned char *str);  // 发送字符串函数
void uart_proc(void);                       // 串口处理函数
extern uint8_t RxData;


#endif
