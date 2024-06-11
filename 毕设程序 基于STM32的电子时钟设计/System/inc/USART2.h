#ifndef __USART2_H
#define __USART2_H
#include "stm32f10x.h"

#define USART2_REC_LEN		200					//最大接收缓存字节数
#define USART2_SEND_LEN		200					//最大发送缓存字节数
//#define USART2_RX_EN 			1					//0,不接收;1,接收.



void USART2_init(u32 bound);
void u2_printf(char* fmt,...);




#endif
