#ifndef __BEEP_H
#define __BEEP_H
#include "stm32f10x.h"

void Init_tiemr4(void);
void Open_Beep(void);
void Close_Beep(void);
void alarm_proc(void);    // 报警处理函数

#endif
