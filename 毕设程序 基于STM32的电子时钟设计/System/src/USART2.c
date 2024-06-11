#include "USART2.h"
#include "stdio.h"
#include "cstring"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"	


//���ڽ��ջ����� 
char USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���200���ֽ�.
u8  USART2_TX_BUF[USART2_SEND_LEN]; //���ͻ���,���200���ֽ�.
u16 USART2_RX_STA=0;       //����״̬���	
u16 len2=0;	  //��ȡ���ܵ����ݳ���


void USART2_init(u32 bound){

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,  ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//USART1_TX   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.1  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������1
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 
	
		
}


void USART2_IRQHandler(void)
{
 	char Res;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		Res =USART_ReceiveData(USART2);		//��ȡ���յ�������,��ȡ���ݼĴ������Զ����

		//�жϽ������� 0X0D 0X0A
		if((USART2_RX_STA&0x8000)==0)//����δ���
		{
			if(USART2_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0A)
				{
					USART2_RX_STA=0;//���մ���,���¿�ʼ
				}
				else
				{
					USART2_RX_STA|=0x8000;	//��������� 
					//return;
				}
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0D)
				{
					USART2_RX_STA|=0x4000;
				}
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))
					{
						USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	
					}				
				}				
			}
		}
		if(USART2_RX_STA&0x8000)
			{				
				len2=USART2_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���		 
				
				USART2_RX_STA=0;     //����״̬��0

			}
		
	}  				 											 
}


//���崮��2�����ַ��ĺ���
void USART2SendByte(uint16_t Data) 
{
  USART_SendData(USART2, Data);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  //�ȴ����ݷ���

}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}









