#include "stm32f10x.h"

// 全局变量：SysTick每微秒的计数值（需根据主频配置，以72MHz为例）
#define SYS_CLOCK 72000000UL  // 单片机主频（根据实际修改：如8MHz则改为8000000）
static uint32_t systick_us = SYS_CLOCK / 1000000UL;  // 72MHz下=72

// 微秒延时（修正版，解决1us/多us计数偏差）
void delay_us(uint32_t us)
{
    uint32_t reload;
    uint32_t temp;
    
    // 防止us=0导致死循环
    if(us == 0) return;
    
    // 计算重载值：SysTick是24位计数器，最大值0xFFFFFF，需判断是否溢出
    reload = us * systick_us;
    if(reload > 0xFFFFFF) reload = 0xFFFFFF;  // 限制最大计数值
    
    SysTick->LOAD = reload - 1;  // 减1：因为计数器从reload减到0才触发FLAG
    SysTick->VAL  = 0x00;        // 清空当前计数值
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;  // 开启SysTick，使用内核时钟
    
    do
    {
        temp = SysTick->CTRL;
    } while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));  // 等待计数完成
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  // 关闭SysTick
    SysTick->VAL  = 0x00;                        // 清空计数器
}

// 毫秒延时（基于微秒封装，无卡顿）
void delay_ms(uint32_t ms)
{
    // 拆分ms为多次us延时，避免单次us过大溢出（如72MHz下单次us最大≈1864us）
    while(ms--)
    {
        delay_us(1000);  // 1ms = 1000us
    }
}
