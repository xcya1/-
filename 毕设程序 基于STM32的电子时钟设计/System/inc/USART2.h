#ifndef __USART2_H
#define __USART2_H
#include "stm32f10x.h"

#define USART2_REC_LEN		200					//�����ջ����ֽ���
#define USART2_SEND_LEN		200					//����ͻ����ֽ���
//#define USART2_RX_EN 			1					//0,������;1,����.



void USART2_init(u32 bound);
void u2_printf(char* fmt,...);




#endif
