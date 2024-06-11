#include "USART1_su30t.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "LED.h"
#include "WIFI.h"

#include "rtc.h"

#include "oled.h"
//接收文本才用字符串

#include "USART2.h"


#define USART1_MAX_SEND_LEN		600					//最大发送缓存字节数

uint8_t USART1_RxPacket[2];				//定义接收数据包数组
u8 USART1_RverFlag;					//定义接收数据包标志位

u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 

//-----------天气数据------------------

u8 yin_TxPacket[]={0x01};    //阴
u8 qing_TxPacket[]={0x03};    //晴
u8 yu_TxPacket[]={0x04};    //雨
u8 duoyun_TxPacket[]={0x05};    //多云

//------------------------------------------

void usart1_init(u32 bound)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9    su30t B6 R
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART1_RX	  GPIOA.10初始化   SU30T-B7 T
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(USART1, &USART_InitStructure); //初始化串口	2
	
	//使能接收中断
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_Cmd(USART1, ENABLE);    
}


void USART1_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}


void USART1_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		USART1_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
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
	
	static u8 RxState=0;     //定义个静态变量表状态，静态变量出了该函数的作用域值不会清0
	static u8 pRxPacket=0;  //用于看接收到第几个数据了
	
//HEX数据包格式 AA 01 01 AA    AA为包头包尾
	   //AA 02 02 AA  
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据
	{	 
			 
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		uint8_t RxData = USART_ReceiveData(USART1);				//读取数据寄存器，存放在接收的数据变量
		
		/*使用状态机的思路，依次处理数据包的不同部分*/
		
		/*当前状态为0，接收数据包包头*/
		if (RxState == 0)
		{
			if (RxData == 0xAA)			//如果数据确实是包头
			{
				RxState = 1;			//置下一个状态
				pRxPacket = 0;			//数据包的位置归零
			} 
		}
		/*当前状态为1，接收数据包数据*/
		else if (RxState == 1)
		{
			USART1_RxPacket[pRxPacket] = RxData;	//将数据存入数据包数组的指定位置
			pRxPacket ++;				//数据包的位置自增
			if (pRxPacket >= 2)			//如果收够4个数据
			{
				RxState = 2;			//置下一个状态
			}
		}
		/*当前状态为2，接收数据包包尾*/
		else if (RxState == 2)
		{
			if (RxData == 0xAA)			//如果数据确实是包尾部
			{
				RxState = 0;			//状态归0
				USART1_RverFlag= 1;		//接收数据包标志位置1，成功接收一个数据包
				
			
				
			}
		}
			
	}
			
} 


void USART1_send_weather_message()
{
	
		if(strcmp(results[0].now.text,"Overcast")==0)  //阴
		{	
			USART1_SendPacket(yin_TxPacket);
		}
		else if((strcmp(results[0].now.text,"Clear")==0) || (strcmp(results[0].now.text,"Sunny")==0))  //晴
		{
			USART1_SendPacket(qing_TxPacket);
		}
		else if((strcmp(results[0].now.text,"Heavy rain")==0) || (strcmp(results[0].now.text,"Light rain")==0) || (strcmp(results[0].now.text,"Moderate rain")==0) || 
						(strcmp(results[0].now.text,"Shower")==0)  || (strcmp(results[0].now.text,"Storm")==0)  )  //雨
		{
			USART1_SendPacket(yu_TxPacket);
		}
		else if(strcmp(results[0].now.text,"Cloudy")==0)  //多云
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
       
	USART1_SendByte(rhour);  //su30t会以16进制的方式接收，会自动转为16进制
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
//	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
//	for(j=0;j<i;j++)							//循环发送数据
//	{
//	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
//		USART_SendData(USART2,USART2_TX_BUF[j]); 
//	} 
//}









	

