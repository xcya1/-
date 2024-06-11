#include "USART1_su30t.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "LED.h"
#include "WIFI.h"

#include "rtc.h"

#include "oled.h"
//�����ı������ַ���

#include "USART2.h"


#define USART1_MAX_SEND_LEN		600					//����ͻ����ֽ���

uint8_t USART1_RxPacket[2];				//����������ݰ�����
u8 USART1_RverFlag;					//����������ݰ���־λ

u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 

//-----------��������------------------

u8 yin_TxPacket[]={0x01};    //��
u8 qing_TxPacket[]={0x03};    //��
u8 yu_TxPacket[]={0x04};    //��
u8 duoyun_TxPacket[]={0x05};    //����

//------------------------------------------

void usart1_init(u32 bound)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9    su30t B6 R
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
	//USART1_RX	  GPIOA.10��ʼ��   SU30T-B7 T
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
	
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	USART_Init(USART1, &USART_InitStructure); //��ʼ������	2
	
	//ʹ�ܽ����ж�
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_Cmd(USART1, ENABLE);    
}


void USART1_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//�ȴ��������
	/*�´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ*/
}


void USART1_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//��������
	{
		USART1_SendByte(Array[i]);		//���ε���Serial_SendByte����ÿ���ֽ�����
	}
}

void USART1_SendPacket(uint8_t *Array)
{
	USART1_SendByte(0xAA);
	USART1_SendByte(0x55);
	USART1_SendArray(Array, 1);
	USART1_SendByte(0x55);
	USART1_SendByte(0xAA);
}




void USART1_IRQHandler(void)
{
	
	static u8 RxState=0;     //�������̬������״̬����̬�������˸ú�����������ֵ������0
	static u8 pRxPacket=0;  //���ڿ����յ��ڼ���������
	
//HEX���ݰ���ʽ AA 01 01 AA    AAΪ��ͷ��β
	   //AA 02 02 AA  
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�����
	{	 
			 
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		uint8_t RxData = USART_ReceiveData(USART1);				//��ȡ���ݼĴ���������ڽ��յ����ݱ���
		
		/*ʹ��״̬����˼·�����δ������ݰ��Ĳ�ͬ����*/
		
		/*��ǰ״̬Ϊ0���������ݰ���ͷ*/
		if (RxState == 0)
		{
			if (RxData == 0xAA)			//�������ȷʵ�ǰ�ͷ
			{
				RxState = 1;			//����һ��״̬
				pRxPacket = 0;			//���ݰ���λ�ù���
			} 
		}
		/*��ǰ״̬Ϊ1���������ݰ�����*/
		else if (RxState == 1)
		{
			USART1_RxPacket[pRxPacket] = RxData;	//�����ݴ������ݰ������ָ��λ��
			pRxPacket ++;				//���ݰ���λ������
			if (pRxPacket >= 2)			//����չ�4������
			{
				RxState = 2;			//����һ��״̬
			}
		}
		/*��ǰ״̬Ϊ2���������ݰ���β*/
		else if (RxState == 2)
		{
			if (RxData == 0xAA)			//�������ȷʵ�ǰ�β��
			{
				RxState = 0;			//״̬��0
				USART1_RverFlag= 1;		//�������ݰ���־λ��1���ɹ�����һ�����ݰ�
				
			
				
			}
		}
			
	}
			
} 


void USART1_send_weather_message()
{
	
		if(strcmp(results[0].now.text,"Overcast")==0)  //��
		{	
			USART1_SendPacket(yin_TxPacket);
		}
		else if((strcmp(results[0].now.text,"Clear")==0) || (strcmp(results[0].now.text,"Sunny")==0))  //��
		{
			USART1_SendPacket(qing_TxPacket);
		}
		else if((strcmp(results[0].now.text,"Heavy rain")==0) || (strcmp(results[0].now.text,"Light rain")==0) || (strcmp(results[0].now.text,"Moderate rain")==0) || 
						(strcmp(results[0].now.text,"Shower")==0)  || (strcmp(results[0].now.text,"Storm")==0)  )  //��
		{
			USART1_SendPacket(yu_TxPacket);
		}
		else if(strcmp(results[0].now.text,"Cloudy")==0)  //����
		{
			USART1_SendPacket(duoyun_TxPacket);
		
		}
			
	
}

void USART1_send_time_message()
{
	
	RTC_Get();

	USART1_SendByte(0xAA);
	USART1_SendByte(0x55);
	USART1_SendByte(0x02);  
       
	USART1_SendByte(rhour);  //su30t����16���Ƶķ�ʽ���գ����Զ�תΪ16����
	USART1_SendByte(rmin);
	     
	USART1_SendByte(0x55);
	USART1_SendByte(0xAA);

}



//void u2_printf(char* fmt,...)  
//{  
//	u16 i,j; 
//	va_list ap; 
//	va_start(ap,fmt);
//	vsprintf((char*)USART2_TX_BUF,fmt,ap);
//	va_end(ap);
//	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
//	for(j=0;j<i;j++)							//ѭ����������
//	{
//	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
//		USART_SendData(USART2,USART2_TX_BUF[j]); 
//	} 
//}









	

