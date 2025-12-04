#ifndef __DELAY_H 
#define __DELAY_H  
#include "stm32f10x.h" 




static uint32_t systick_us = 0;
//static uint32_t systick_ms = 0;

void delay_init(void); //µÎ´ğ¶¨Ê±Æ÷ÅäÖÃ
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);


#endif
