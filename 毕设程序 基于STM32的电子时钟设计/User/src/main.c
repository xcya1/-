#include "stm32f10x.h"                  // Device hea main()
#include "LED.h"
#include "Key.h"
#include "Delay.h"
#include "oled.h"
#include "rtc.h"
#include "cJSON.h"
#include "USART3_TIM3_wifi.h"	 
#include "USART2.h"
#include "WIFI.h"
#include "USART1_su30t.h"
#include "string.h"
#include "exti.h"
#include "TIM_2.h"
#include "BEEP.h"

#include "menu.h"
#include "set_time_ui.h"
#include "alarm_clock_ui.h"
#include "stop_watch_ui.h"
#include "weather_ui.h"

#include "math.h"

void time_show();
void weather_show();

extern char timestamp[12];


int simple_atoi(char *source) //�ַ���ת����
{
	int length = strlen(source); //���㳤��
	int sum = 0;
	for(int i=0;i<length;i++) sum += (source[i]-'0') * pow(10,length-1-i); //�ַ�ת���ֲ��ϲ�
	return sum;
}

void set_network_time()
{
		u32 seccount=simple_atoi(timestamp);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE); 
		RTC_WaitForLastTask(); //�ȴ�д�����
		RTC_SetCounter(seccount+28800+3);//�ѻ���õļ�����ֵд��
	
}




int main()
{

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	OLED_Init();
	BEEP_Init();
	Exit_Init();
	TIM_2_Init();
	RTC_Config();

	USART2_init(9600);	
	usart3_init(115200);
	usart1_init(9600);
	
	OLED_Clear();

	OLED_ShowChinese(16*1,16+6,33,16,1);
	OLED_ShowChinese(16*2,16+6,34,16,1);
	OLED_ShowChinese(16*3,16+6,35,16,1);
	OLED_ShowChinese(16*4,16+6,36,16,1);
	OLED_ShowChinese(16*5,16+6,37,16,1);
	OLED_ShowString(16*6,16+6,(u8*)"...",16,1);
	OLED_Refresh();

  acquire_weather_message();
	acquire_time_message();
	set_network_time();
	
	OLED_ShowString(16*1,16+6,(u8*)"                ",16,1);  //���������ʾ

	while(1)
	{

		time_show();
			
		if(exti_2==1)     //����1���빦��ѡ�����
		{
			
			exti_2=0;
			exti_3=0;
			
			menu();	
		}
		else if(exti_1==1)
		{
			exti_1=0;
			//�ر�����
			BEEP_OFF();
			
			alarm_clock_start=0;
			open_or_close=0;
			
		}
		
		if(USART1_RverFlag==1)
			{
				//������������
				USART1_RverFlag=0;
				
				if(USART1_RxPacket[1]==0x01)
				{
					LED1_ON();
						USART1_send_weather_message();
				}
				if(USART1_RxPacket[1]==0x02)
				{
					 USART1_send_time_message();  //����ʱ������
				}	
			}		
	}
}


void time_show()
{
		RTC_Get();
		OLED_ShowNum(3*8,16,ryear,4,16,1);
		OLED_ShowChar(7*8,16,'-',16,1);
		OLED_ShowNum(8*8,16,rmon,2,16,1);
		OLED_ShowChar(10*8,16,'-',16,1);
		OLED_ShowNum(11*8,16,rday,2,16,1);
		
		OLED_ShowNum(4*8,16*2,rhour,2,16,1);
		OLED_ShowChar(6*8,16*2,':',16,1);
		OLED_ShowNum(7*8,16*2,rmin,2,16,1);
		OLED_ShowChar(9*8,16*2,':',16,1);
		OLED_ShowNum(10*8,16*2,rsec,2,16,1);
		
	
		OLED_ShowChinese(16*1,16*3,7,16,1);  //��
		OLED_ShowChinese(16*2,16*3,8,16,1); //��
	
	
		if(rweek==0) 	OLED_ShowChinese(16*3,16*3,0,16,1); 
		else if(rweek==1)OLED_ShowChinese(16*3,16*3,1,16,1);
		else if(rweek==2)OLED_ShowChinese(16*3,16*3,2,16,1);
		else if(rweek==3)OLED_ShowChinese(16*3,16*3,3,16,1);
		else if(rweek==4)OLED_ShowChinese(16*3,16*3,4,16,1);
		else if(rweek==5)OLED_ShowChinese(16*3,16*3,5,16,1);
		else OLED_ShowChinese(16*3,16*3,6,16,1);
		
	
			if(USART1_RverFlag==1)
			{
				//������������
				USART1_RverFlag=0;
				if(USART1_RxPacket[2]==0x01)
				{
						USART1_send_weather_message();
				}
				else
				{
					 USART1_send_time_message();  //����ʱ������
				}
	
			}
			
	if(alarm_clock_start==1)
	{
		//���ӿ���ͼ��
		OLED_ShowChinese(16*6,0,29,16,1);
	}
	else
	{
		OLED_ShowChinese(16*6,0,32,16,1);
	}
	
	if(WIFI_connect_succeed==1)
	{
		//WIFIl����ͼ��
		OLED_ShowChinese(16*7,0,30,16,1);
	}
	else
	{
		//WIFIδ����ͼ��
		OLED_ShowChinese(16*7,0,31,16,1);
	}
		weather_show();
		OLED_Refresh();
}



void weather_show()
{
	OLED_ShowString(16*5+10,16*3,(u8 *)"            ",16,1);	
	
	if(strcmp(results[0].now.text,"Overcast")==0)   //��
	{
		OLED_ShowChinese(16*5+10,16*3,18,16,1);
	}
	else if((strcmp(results[0].now.text,"Clear")==0) || (strcmp(results[0].now.text,"Sunny")==0))  //��
	{
		OLED_ShowChinese(16*5+10,16*3,17,16,1);
	}
	else if((strcmp(results[0].now.text,"Heavy rain")==0) || (strcmp(results[0].now.text,"Light rain")==0) || (strcmp(results[0].now.text,"Moderate rain")==0) || 
		      (strcmp(results[0].now.text,"Shower")==0)  || (strcmp(results[0].now.text,"Storm")==0)  )  //��
	{
		OLED_ShowChinese(16*5+10,16*3,16,16,1);
	
	}
	else if(strcmp(results[0].now.text,"Cloudy")==0)  //����
	{
		OLED_ShowChinese(16*5+10,16*3,14,16,1);
		OLED_ShowChinese(16*6+10,16*3,15,16,1);
	
	}
	else   //Ӣ������
	{
			OLED_ShowString(16*5+10,16*3,(u8 *)results[0].now.text,16,1);	
	}

}




