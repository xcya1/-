#include "menu.h"
#include "oled.h"
#include "exti.h"
#include "bmp.h"

#include "LED.h"

#include "set_time_ui.h"
#include "alarm_clock_ui.h"
#include "stop_watch_ui.h"
#include "weather_ui.h"
#include "Game_ui.h"
#include "explain_ui.h"





int menu(void)
{
	
static	u8 flag=1;

	OLED_Clear();
	
	while(1)
	{
		if(exti_2==1)  //选项
		{
			exti_2=0;
			flag++;
			if(flag==7) flag=1;
		}
		else if(exti_1==1)  //选项
		{
			exti_1=0;
			flag--;
			if(flag<1) flag=6;
		}
		
		
		else	if(exti_3==1)  //返回主界面
		{
			exti_3=0;
			
			exti_0=0;
			exti_1=0;
			exti_2=0;
			
			OLED_Clear();
			return 0;
		
		}
		else	if(exti_0==1)    //确认
		{
			exti_0=0;
			
			exti_1=0;
			exti_2=0;
			exti_3=0;
			
			OLED_Clear();
			
			if(flag==1) {weather_ui();}
			else	if(flag==2) {alarm_clock_ui();}
			else if(flag==3) {stop_watch();}
			else if(flag==4) {Game_ui();}
			else if(flag==5) {set_ui();}
			else if(flag==6) {explain_ui();}			
			
		}
		
		switch(flag)
		{
			
			case 1:
			{
					OLED_ShowPicture(40,6,44,44,BMP5,1);  //天气
					OLED_ShowChinese(16*3,16*3,0,16,1);
					OLED_ShowChinese(16*4,16*3,11,16,1);
					OLED_Refresh();
				
			}break;

			case 2:
			{
					OLED_ShowPicture(40,6,44,44,BMP3,1); //闹钟
					OLED_ShowChinese(16*3,16*3,21,16,1);
					OLED_ShowChinese(16*4,16*3,22,16,1);
					OLED_Refresh();
			}break;
			
			case 3:
			{
					OLED_ShowPicture(40,6,44,44,BMP4,1); //秒表
					OLED_ShowChinese(16*3,16*3,23,16,1);
					OLED_ShowChinese(16*4,16*3,24,16,1);
					OLED_Refresh();	
			}break;	
			
			case 4:
			{
					OLED_ShowPicture(40,6,44,44,BMP8,1);  //游戏
					OLED_ShowChinese(16*3,16*3,71,16,1);
					OLED_ShowChinese(16*4,16*3,72,16,1);
					OLED_Refresh();
			}break;
			
			
			case 5:
			{
					OLED_ShowPicture(40,6,44,44,BMP6,1); //设置
					OLED_ShowChinese(16*3,16*3,27,16,1);
					OLED_ShowChinese(16*4,16*3,28,16,1);
					OLED_Refresh();
			}break;
			

			case 6:
			{
					OLED_ShowPicture(40,6,44,44,BMP7,1); //关于
					OLED_ShowChinese(16*3,16*3,52,16,1);
					OLED_ShowChinese(16*4,16*3,53,16,1);
					OLED_Refresh();
			}break;
		}
		
	
		
	}

}