#ifndef __USART1_SU30T_H
#define __USART1_SU30T_H
#include "stm32f10x.h"



extern uint8_t USART1_RxPacket[2];	

extern u8 USART1_RverFlag;


void usart1_init(u32 bound);				//´®¿Ú3³õÊ¼»¯ 

//void u2_printf(char* fmt,...) ;

void USART1_send_weather_message();


void USART1_send_time_message();
#endif