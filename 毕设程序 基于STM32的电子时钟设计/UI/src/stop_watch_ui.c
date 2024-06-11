#include "stop_watch_ui.h"
#include "oled.h"
#include "rtc.h"
#include "exti.h"

#include "LED.h" 

#include "alarm_clock_ui.h"

u8 second[3];

void second_show()
{
		OLED_ShowNum(4*8,16*2-6,second[0],2,16,1);
		OLED_ShowChar(6*8,16*2-6,':',16,1);
		OLED_ShowNum(7*8,16*2-6,second[1],2,16,1);
		OLED_ShowChar(9*8,16*2-6,'.',16,1);
		OLED_ShowNum(10*8,16*2-6,second[2],2,16,1);
		OLED_Refresh();
}

int stop_watch()
{

	
	second[0]=0;
	second[1]=0;
	second[2]=0;
	
		OLED_ShowChinese(16*3,0,23,16,1);
		OLED_ShowChinese(16*4,0,24,16,1);
	
	
	while(1)
	{

		second_show();
		
		if(exti_0==1)                //也可以用switch 按键按下为1 2 3 4
		{
			exti_0=0;
			TIM_Cmd(TIM2, ENABLE);
			
		}
		else 	if(exti_1==1)
		{
			exti_1=0;
			TIM_Cmd(TIM2,DISABLE);
		
		}
		
		else if(exti_2==1)
		{
		
			exti_2=0;
			exti_back=0;
			TIM_Cmd(TIM2,DISABLE);
			second[0]=0;
			second[1]=0;
			second[2]=0;
		
		}
		
		else if(exti_3==1)
		{
			exti_3=0;
			TIM_Cmd(TIM2,DISABLE);
			second[0]=0;
			second[1]=0;
			second[2]=0;
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



void TIM2_IRQHandler(void)
{ 
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
			{	//判断是否是TIM3中断
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

				
				if(second[2]<100)   //秒加加   
				{
					second[2]++;    //10ms自增100次就为1000ms==1s
				}
				else
				{
						second[2]=0;
						if(second[1]<60)   //分加加
						{
							second[1]++;  
						
						}
						else
						{
							second[1]=0; 
							if(second[0]<60)  //时加加
							{
								second[0]++;
							
							}
							else
							{
							
							}
						
						}
				
				}
				
					
			}
			
}

