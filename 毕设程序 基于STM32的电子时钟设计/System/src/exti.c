#include "exti.h"
#include "Delay.h"


u8 exti_0,exti_1,exti_2,exti_3;
u8 exti_back;

extern u8 alarm_TI_flag;



void delay_us(u16 time){ 
   
   u16 i=0;  
 
   while(time--){
 
      i=10;  //自己定义
      while(i--) ;    
   }

}



void Exit_Init()
{

	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	  //开启AFIO的时钟，外部中断必须开启AFIO的时钟

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;					
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		
	EXTI_Init(&EXTI_InitStructure);		
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;	
	EXTI_Init(&EXTI_InitStructure);	
	

	EXTI_InitStructure.EXTI_Line = EXTI_Line14;	
	EXTI_Init(&EXTI_InitStructure);	

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;	
	EXTI_Init(&EXTI_InitStructure);	


	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;		//在"stm32f10x.h" 里面查
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);	

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn ;
//	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn  ;
//	NVIC_Init(&NVIC_InitStructure);

}






void EXTI15_10_IRQHandler()
{

	if (EXTI_GetITStatus(EXTI_Line12) == SET)	
	{	
		EXTI_ClearITPendingBit(EXTI_Line12);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
		{
			delay_us(100);
			while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);
			delay_us(100);
			exti_0=1;				
		}	
	}
	
	
	if (EXTI_GetITStatus(EXTI_Line13) == SET)	
	{	
		EXTI_ClearITPendingBit(EXTI_Line13);
		
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
		{
				delay_us(100);
			while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);
				delay_us(100);
			exti_1=1;	
		}		
	}

	if (EXTI_GetITStatus(EXTI_Line14) == SET)	
	{	
		EXTI_ClearITPendingBit(EXTI_Line14);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		{
				delay_us(100);
			while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
				delay_us(100);
			exti_2=1;	
		}	
	}
	
	if (EXTI_GetITStatus(EXTI_Line15) == SET)	
	{	
		EXTI_ClearITPendingBit(EXTI_Line15);
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
			{		
					delay_us(100);
					while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);
					delay_us(100);
				exti_3=1;		
			}	
	}


}
