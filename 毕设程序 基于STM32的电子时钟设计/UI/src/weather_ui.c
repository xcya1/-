#include "weather_ui.h"
#include "oled.h"
#include "WIFI.h"
#include "Delay.h"
#include "string.h"
#include "exti.h"
#include "LED.h"
#include "rtc.h"

#include "alarm_clock_ui.h"

#include "weather_pic.h"



void weather_hz();

void weather_data_show()
{

	weather_hz();

	OLED_ShowString(16*3,16*2+12,(u8*)results[0].now.temperature,16,1);
	OLED_Refresh();

}


int weather_ui()
{

	OLED_Clear();

	OLED_ShowChinese(0,0,9,16,1);
	OLED_ShowChinese(16,0,10,16,1);
	OLED_ShowChar(16*2,0,':',16,1);
	OLED_ShowChinese(16*3,0,19,16,1);
	OLED_ShowChinese(16*4,0,20,16,1);
	
	
	OLED_ShowChinese(0,16+6,0,16,1);
	OLED_ShowChinese(16,16+6,11,16,1);
	OLED_ShowChar(16*2,16+6,':',16,1);
	
	OLED_ShowChinese(0,16*2+12,12,16,1);
	OLED_ShowChinese(16,16*2+12,13,16,1);
	OLED_ShowChar(16*2,16*2+12,':',16,1);
	
	OLED_Refresh();
	
	while(1)
	{
	
		 weather_data_show();
		
		if(exti_3==1)
		{
			exti_3=0;
	
			exti_0=0;
			exti_1=0;
			exti_2=0;
			
			OLED_Clear();
			return 0;
		
		}
		 if(alarm_TI_flag==1)
		{
			//闹钟时间到
			alarm_TI_flag=0;		
			LED1_ON();
		}
		
		
		
	}
	
}



void weather_hz()
{
	OLED_ShowString(16*3,16,(u8*)"                ",16,1);	
	
	if(strcmp(results[0].now.text,"Overcast")==0)   //阴
	{
		OLED_ShowChinese(16*3,16+6,18,16,1);
		OLED_ShowPicture(16*5,29,38,38,p4,1);
	}
	else if((strcmp(results[0].now.text,"Clear")==0) || (strcmp(results[0].now.text,"Sunny")==0))  //晴
	{
		OLED_ShowChinese(16*3,16+6,17,16,1);
		OLED_ShowPicture(16*5,29,38,38,p3,1);
	}
	else if((strcmp(results[0].now.text,"Heavy rain")==0) || (strcmp(results[0].now.text,"Light rain")==0) || (strcmp(results[0].now.text,"Moderate rain")==0) || 
		      (strcmp(results[0].now.text,"Shower")==0)  || (strcmp(results[0].now.text,"Storm")==0)  )  //雨
	{
		OLED_ShowChinese(16*3,16+6,16,16,1);
		OLED_ShowPicture(16*5,29,38,38,p2,1);
	
	}
	else if(strcmp(results[0].now.text,"Cloudy")==0)  //多云
	{
		OLED_ShowChinese(16*3,16+6,14,16,1);
		OLED_ShowChinese(16*4,16+6,15,16,1);
		OLED_ShowPicture(16*5,29,38,38,p1,1);
	}
	else   //英文天气
	{
			OLED_ShowString(16*3,16+6,(u8 *)results[0].now.text,16,1);	
	}


}