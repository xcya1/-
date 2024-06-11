#include "explain_ui.h"
#include "oled.h"
#include "exti.h"
#include "alarm_clock_ui.h"
#include "LED.h"

void explain_ui()
{
	OLED_Clear();
	
	OLED_ShowChinese(0,0,32,16,0);     //智能电子时钟
	OLED_ShowChinese(16,0,58,16,0);
	OLED_ShowChinese(16*2,0,59,16,0);
	OLED_ShowChinese(16*3,0,60,16,0);
	OLED_ShowChinese(16*4,0,61,16,0);
	OLED_ShowChinese(16*5,0,62,16,0);
	OLED_ShowChinese(16*6,0,63,16,0);
	OLED_ShowChinese(16*7,0,32,16,0);
	
	OLED_ShowChinese(0,16+6,54,16,1);   //版本
	OLED_ShowChinese(16,16+6,55,16,1);
	OLED_ShowChar(16*2,16+6,':',16,1);
	OLED_ShowString(16*3,16+6,(u8*)"1.0",16,1);
	
	OLED_ShowChinese(0,16*2+12,56,16,1);   //开发
	OLED_ShowChinese(16,16*2+12,57,16,1);
	OLED_ShowChar(16*2,16*2+12,':',16,1);
	OLED_ShowChinese(16*3,16*2+12,64,16,1);
	OLED_ShowChinese(16*4,16*2+12,65,16,1);
	
	OLED_Refresh(); 
	
	while(1)
	{
	
		if(exti_3==1)
		{
			exti_3=0;
			exti_0=0;
			exti_1=0;
			exti_2=0;
			OLED_Clear();
			return ;
		}
	}
}