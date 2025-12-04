#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"

// 宏定义：定时周期（以1ms为例，主频72MHz）
#define TIM3_PERIOD    1000 - 1  // 自动重装值
#define TIM3_PRESCALER 72 - 1    // 预分频系数：72MHz/72=1MHz

// 函数声明
void TIM3_Init(void);            // TIM3初始化（1ms中断）
void TIM3_IRQHandler(void);      // TIM3中断服务函数
extern uint8_t smg_buf[4] ;
// 扫描位置索引（0-3）
extern  uint8_t smg_scan_pos ;
extern unsigned long int uwtick;
#endif
