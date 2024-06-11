#include "USART2.h"
#include "stdio.h"
#include "cstring"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"	


//串口接收缓存区 
char USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大200个字节.
u8  USART2_TX_BUF[USART2_SEND_LEN]; //发送缓冲,最大200个字节.
u16 USART2_RX_STA=0;       //接收状态标记	
u16 len2=0;	  //获取接受到数据长度


void USART2_init(u32 bound){

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,  ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//USART1_TX   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.1  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);                    //使能串口1 
	
		
}


void USART2_IRQHandler(void)
{
 	char Res;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		Res =USART_ReceiveData(USART2);		//读取接收到的数据,读取数据寄存器就自动清除

		//判断结束符号 0X0D 0X0A
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART2_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0A)
				{
					USART2_RX_STA=0;//接收错误,重新开始
				}
				else
				{
					USART2_RX_STA|=0x8000;	//接收完成了 
					//return;
				}
			}
			else //还没收到0X0D
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
						USART2_RX_STA=0;//接收数据错误,重新开始接收	
					}				
				}				
			}
		}
		if(USART2_RX_STA&0x8000)
			{				
				len2=USART2_RX_STA&0x3fff;//得到此次接收到的数据长度		 
				
				USART2_RX_STA=0;     //接收状态归0

			}
		
	}  				 											 
}


//定义串口2发送字符的函数
void USART2SendByte(uint16_t Data) 
{
  USART_SendData(USART2, Data);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  //等待数据发完

}

//串口2,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}









