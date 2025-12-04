#ifndef __SMG_H
#define __SMG_H
#include "stm32f10x.h"   
#include "delay.h"// Device header


void SMG_Init(void);
void SMG_Display_Single(uint8_t pos, uint8_t num, uint8_t dp_en);

void seg_proc(void);
#endif
