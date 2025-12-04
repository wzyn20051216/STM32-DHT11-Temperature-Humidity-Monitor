#include "timer.h"
#include "smg.h"
#include "stm32f10x.h"                  // Device header

unsigned long int uwtick = 0;
// 全局变量：定时计数（可用于延时/任务触发）
volatile uint32_t tim3_count = 0;
// 全局显示缓冲区：smg_display_buf[0]=最左位，[3]=最右位
uint8_t smg_buf[4] = {10, 10, 10, 10};
// 扫描位置索引（0-3）
static uint8_t smg_scan_pos = 0;
// TIM3初始化（1ms中断，72MHz主频）
void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 使能TIM3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 2. 配置TIM3时基参数
    TIM_TimeBaseStructure.TIM_Period = TIM3_PERIOD;          // 自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler = TIM3_PRESCALER;    // 预分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;             // 时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 3. 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    // 4. 开启更新中断 + 启动定时器
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

// TIM3中断服务函数（1ms进入一次）
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
			 uwtick++;      // 系统时基递增
			 SMG_Display_Single(smg_scan_pos, smg_buf[smg_scan_pos], 0);
        
        // 2. 切换到下一个位置（循环0-3）
        smg_scan_pos = (smg_scan_pos + 1) % 4;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志
    }
}