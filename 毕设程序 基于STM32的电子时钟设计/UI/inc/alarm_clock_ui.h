#ifndef __ALARM_CLOCK_UI_H
#define __ALARM_CLOCK-UI_H
#include "stm32f10x.h"

extern u8 alarm_TI_flag;
extern u8 alarm_clock_start;
extern u8 open_or_close;

int set_alarm_clock();


void  alarm_clock_ui();

void alarm_clock_open_or_close();
#endif
