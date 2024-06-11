#ifndef __USART3_TIM3_WIFI_H
#define __USART3_TIM_WIFI_H
#include "stdio.h"	

#include "stm32f10x.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
#define USART3_MAX_RECV_LEN		1024				//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		1024				//����ͻ����ֽ���

#define USART_REC_LEN  			500  	//�����������ֽ��� 500
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void TIM3_Set(u8 sta);
void TIM3_Int_Init(u16 arr,u16 psc);

void USART_OUT(u8 *Data, int Len);

void USART3_Send_Byte(u8 Data) 	;
void USART3_Send_String(u8 *Data) 	;

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart3_init(u32 bound);
#endif


