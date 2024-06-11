#include "alarm_clock_ui.h"

#include "oled.h"

#include "rtc.h"
#include "Delay.h"

#include "exti.h"
#include "LED.h"

#include "BEEP.h"


u8 alarm_TI_flag;
u8 alarm_clock_start;

int time[6];

u8 alarm_menu;

u8 open_or_close;

int set_alarm_clock();
void alarm_clock_state();
void alarm_clock_open_or_close();


void  alarm_clock_ui()
{

	u8 select_f=1;
	
	while(1)
	{
		OLED_ShowChinese(0,0,32,16,0);
		OLED_ShowChinese(16,0,32,16,0);
		OLED_ShowChinese(16*2,0,32,16,0);
		OLED_ShowChinese(16*3,0,48,16,0);
		OLED_ShowChinese(16*4,0,49,16,0);	
		OLED_ShowChinese(16*5,0,32,16,0);	
		OLED_ShowChinese(16*6,0,32,16,0);	
		OLED_ShowChinese(16*7,0,32,16,0);
		
		OLED_ShowChinese(8+8,16+6,27,16,1);
		OLED_ShowChinese(16+8+8,16+6,28,16,1);	
		OLED_ShowChinese(16*2+8+8,16+6,21,16,1);
		OLED_ShowChinese(16*3+8+8,16+6,22,16,1);	
			
		OLED_ShowChinese(8+8,16*2+10,38,16,1);
		OLED_ShowChinese(16+8+8,16*2+10,39,16,1);	
		OLED_ShowChinese(16*2+8+8,16*2+10,21,16,1);
		OLED_ShowChinese(16*3+8+8,16*2+10,22,16,1);
		
		if(alarm_menu==1)
		{
			
			set_alarm_clock();   //进入设置闹钟
			alarm_clock_state();
			alarm_menu=0;
		}
		else if(alarm_menu==2)
		{
			alarm_clock_open_or_close();  //进入查看闹钟状态
			alarm_menu=0;
			
		}
		
		
		
		if(exti_2==1)  //选择
		{
				exti_2=0;
				exti_1=0;  //防误触
				select_f++;
			if(select_f==3)
			{
				select_f=1;
			}
		}
	else if(exti_0==1)  //确认
		{
			exti_0=0;
			exti_1=0;    //防误触
			alarm_menu=select_f;
		}
		else if(exti_3==1)  //返回菜单选择界面
		{
			exti_3=0;
			exti_1=0;  //防误触
			
			OLED_Clear();
			return ;
		}
		
		switch(select_f)
		{
			case 1 : 	OLED_ShowChar(0,16+6,'*',16,1);
								OLED_ShowChar(0,16*2+10,' ',16,1);
								OLED_Refresh();
								break;
			
			case 2 :	OLED_ShowChar(0,16+6,' ',16,1);
								OLED_ShowChar(0,16*2+10,'*',16,1);
								OLED_Refresh(); 
								break;
		
		}
		

		
			
	
	}


}



int set_alarm_clock()
{
			OLED_Clear();
			u8 flag_alarm=0;
		  RTC_Get();
	
			time[0]=ryear;
			time[1]=rmon;
			time[2]=rday;
			time[3]=rhour;
			time[4]=rmin;
			time[5]=rsec;

	
		OLED_ShowChinese(16*3,0,21,16,1);
		OLED_ShowChinese(16*4,0,22,16,1);
	
		OLED_ShowNum(3*8,16,time[0],4,16,1);
		OLED_ShowChar(7*8,16,'-',16,1);
		OLED_ShowNum(8*8,16,time[1],2,16,1);
		OLED_ShowChar(10*8,16,'-',16,1);
		OLED_ShowNum(11*8,16,time[2],2,16,1);
		
		OLED_ShowNum(4*8,16*2,time[3],2,16,1);
		OLED_ShowChar(6*8,16*2,':',16,1);
		OLED_ShowNum(7*8,16*2,time[4],2,16,1);
		OLED_ShowChar(9*8,16*2,':',16,1);
		OLED_ShowNum(10*8,16*2,time[5],2,16,1);
		
		OLED_Refresh();
	
	while(1)
	{
			if(flag_alarm==0)     //------------------分
			{
				while(1)
				{
					a:
					exti_back=0;
						if(exti_3==1)
						{
								exti_3=0;				
								//alarm_menu=0;
					      return 0 ;  
						}											
						else if(exti_0==1)
						{
							exti_0=0;
							time[4]++;
							if(time[4]>59){time[4]=0;}
						
						}
						else if(exti_1==1)
						{
							exti_1=0;
							time[4]--;
							if(time[4]<0){time[4]=59;}
						
						}
						else if(exti_2==1)
						{
								exti_2=0;
								flag_alarm=1;
								
								OLED_ShowNum(7*8,16*2,time[4],2,16,1);
								OLED_Refresh();
								break;
						
						}
							
						if(exti_back==1)
						{
							goto a;		
						}
						
						OLED_ShowNum(7*8,16*2,time[4],2,16,1);
						Delay_ms(350);
						OLED_Refresh();	
						
						OLED_ShowString(7*8,16*2,"  ",16,1);
						Delay_ms(350);
						OLED_Refresh();						
				}
			}
			if(flag_alarm==1)        //----------时
			{
				while(1)
				{
					b:
					exti_back=0;
						if(exti_3==1)
						{
							exti_3=0;
							//alarm_menu=0;
							return 0;  
		 				}
						else if(exti_0==1)
						{
							exti_0=0;
							time[3]++;
							if(time[3]>59){time[3]=0;}
						
						}
						else if(exti_1==1)
						{
							exti_1=0;
							time[3]--;
							if(time[3]<0){time[3]=59;}
						
						}
						else if(exti_2==1)
						{
								exti_2=0;
								flag_alarm=2;
								OLED_ShowNum(4*8,16*2,time[3],2,16,1);
								OLED_Refresh();
								break;
						
						}		

						if(exti_back==1)
						{
							goto b;
						}
						
							OLED_ShowNum(4*8,16*2,time[3],2,16,1);	
							Delay_ms(350);
							OLED_Refresh();	

							OLED_ShowString(4*8,16*2,"  ",16,1);		
							Delay_ms(350);
							OLED_Refresh();
				}			
			}
			if(flag_alarm==2)        //----------日
			{
		
				while(1)
				{
					c:
					exti_back=0;
						if(exti_3==1)
						{
							exti_3=0;
//							u32  count_value=Get_ALR_Value(time[0],time[1],time[2],time[3],time[4],time[5]);
//							RTC_SetAlarm(count_value);
//							RTC_WaitForLastTask();

//							RTC_ITConfig(RTC_IT_ALR, ENABLE);//使能闹钟中断   
//							RTC_WaitForLastTask();
//							OLED_Clear();
//							alarm_clock_start=1;
						
							return 0;  
						
		 				}
						else if(exti_0==1)
						{
							exti_0=0;
							time[2]++;
							if( time[1]==1 || time[1]==3 ||time[1]==5 ||time[1]==7 || time[1]==8 || time[1]==10 || time[1]==12)//日越界判断
								{
									if(time[2]>31){time[2]=1;}//大月
								}
								else if(time[1]==4 || time[1]==6 || time[1]==9 || time[1]==11)
								{
									if(time[2]>30){time[2]=1;}//小月
								}
								else if(time[1]==2)
								{
									if(time[0]%4==0)
									{
										if(time[2]>29){time[2]=1;}//闰年2月
									}
									else
									{
										if(time[2]>28){time[2]=1;}//平年2月
									}
								}
						}
						else if(exti_1==1)
						{
							exti_1=0;
							time[2]--;
								if( time[1]==1 || time[1]==3 || time[1]==5 || time[1]==7 || time[1]==8 ||time[1]==10||time[1]==12)//日越界判断
								{
									if(time[2]<1){time[2]=31;}//大月
									if(time[2]>31){time[2]=1;}
								}
								else if(time[1]==4 || time[1]==6 ||time[1]==9 ||time[1]==11)
								{
									if(time[2]<1){time[2]=30;}//小月
									if(time[2]>30){time[2]=1;}
								}
								else if(time[1]==2)
								{
									if(time[0]%4==0)
									{
										if(time[2]<1){time[2]=29;}//闰年2月
										if(time[2]>29){time[2]=1;}
									}
									else
									{
										if(time[2]<1){time[2]=28;}//平年2月
										if(time[2]>28){time[2]=1;}
									}
								}	
						
						}
						else if(exti_2==1)
						{
							exti_2=0;
							flag_alarm=0;
							OLED_ShowNum(11*8,16,time[2],2,16,1);
							OLED_Refresh();
							break;
						}		

						if(exti_back==1)
						{
							goto c;
						}
						OLED_ShowNum(11*8,16,time[2],2,16,1);
						Delay_ms(350);	
						OLED_Refresh();	

						OLED_ShowString(11*8,16,"  ",16,1);		
						Delay_ms(350);
						OLED_Refresh();
				}			
			}
	
	
	}

}


void RTCAlarm_IRQHandler(void)
{     
	if(RTC_GetITStatus(RTC_IT_ALR) == 1)//闹钟中断
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);//清除闹钟中断

			BEEP_ON();
		
	}
	EXTI_ClearITPendingBit(EXTI_Line17);
	RTC_WaitForLastTask();
	RTC_ClearITPendingBit(RTC_IT_ALR);
	RTC_WaitForLastTask();
	
}




void alarm_clock_state()    //是否开启闹钟
{		
	u8 f=1;	
	u8 m=0;
	OLED_Clear();

	OLED_ShowChinese(0,0,32,16,0);
	OLED_ShowChinese(16,0,44,16,0);   //是否开启闹钟
	OLED_ShowChinese(16*2,0,45,16,0);
	OLED_ShowChinese(16*3,0,40,16,0);
	OLED_ShowChinese(16*4,0,46,16,0);
	OLED_ShowChinese(16*5,0,21,16,0);
	OLED_ShowChinese(16*6,0,22,16,0);
	OLED_ShowChinese(16*7,0,32,16,0);
	
	OLED_ShowChinese(16+4,16+6,44,16,1);     //是
	OLED_ShowChinese(16+4,16*2+10,45,16,1);  //否
	
	
	while(1)
	{

			if(exti_2==1)   //选项
			{
				exti_2=0;
				
				exti_1=0;
				exti_3=0;
				
				f++;
				if(f==3)
				{
					f=1;
				}
			}
			else if(exti_0==1)  //确认选项
			{
				exti_0=0;
				
				exti_1=0;
				exti_3=0;
				
				m=f;
			
			}
			switch(f)
			{
				case 1: 		OLED_ShowString(0,16+6,"*",16,1);
										OLED_ShowString(0,16*2+10," ",16,1);
										OLED_Refresh();
										break;
				
				case 2: 		OLED_ShowString(0,16+6," ",16,1);
										OLED_ShowString(0,16*2+10,"*",16,1);
										OLED_Refresh();
										break;
			}
			
			if(m==1)    //确认开启闹钟
			{
				u32  count_value=Get_ALR_Value(time[0],time[1],time[2],time[3],time[4],time[5]);
				RTC_SetAlarm(count_value);
				RTC_WaitForLastTask();
				RTC_ITConfig(RTC_IT_ALR, ENABLE);//使能秒中断   
				RTC_WaitForLastTask();
				
				alarm_clock_start=1;   //闹钟图标
				open_or_close=1;       //闹钟状态
				
				OLED_Clear();
				return ;
			}
			if(m==2)  //不开启闹钟
			{
				OLED_Clear();
				open_or_close=0;
				return ;
			}		
	}
}


void alarm_clock_open_or_close()    //查看闹钟状态
{
	OLED_Clear();
	static u8 s=1;
	u8 m=0;
	OLED_ShowChinese(13,0,21,16,1);    //闹钟
	OLED_ShowChinese(13+16,0,22,16,1);
	
	OLED_ShowChinese(16*5+8,0,42,16,1);  //状态
	OLED_ShowChinese(16*6+8,0,43,16,1);

	OLED_DrawLine(0,16,127,0,1);
	OLED_DrawLine(0,16+16+14,127,0,1);
	OLED_ShowChinese(16*1,16*3,40,16,1);  //开
	OLED_ShowChinese(16*4,16*3,41,16,1);  //关
	OLED_Refresh();
	
	while(1)
	{
		if(open_or_close==1)   //闹钟状态为开，
		{
					OLED_ShowNum(0,17,time[0]/10%10,1,16,1);  //显示设置的闹钟
					OLED_ShowNum(8,17,time[0]/1%10,1,16,1);
					OLED_ShowChar(8*2,17,'-',16,1);
					OLED_ShowNum(8*3,17,time[1],2,16,1);
					OLED_ShowChar(8*5,17,'-',16,1);
					OLED_ShowNum(8*6,17,time[2],2,16,1);
		
					OLED_ShowNum(0,17+13,time[3],2,16,1);
					OLED_ShowChar(2*8,17+13,':',16,1);
					OLED_ShowNum(3*8,17+13,time[4],2,16,1);
					OLED_ShowChar(5*8,17+13,':',16,1);
					OLED_ShowNum(6*8,17+13,time[5],2,16,1);
			
			
				if(exti_2==1)   //选择以设置闹钟的状态
				{
					exti_2=0;
					
					exti_1=0;
					
					s++;
					if(s==3)
						s=1;
					
				}
				else if(exti_0==1)
				{
					exti_0=0;
					m=s;
				
				}
				else if(exti_3==1)
				{
					exti_3=0;
					
					exti_1=0;  //防误触
					OLED_Clear();
					return ;
				
				}
					
				if(alarm_clock_start==1 )  //根据闹钟的图标显示状态开关
				{
					OLED_ShowChinese(16*6,17+10,40,16,1);
				}
				else if(alarm_clock_start==0 )
				{
					OLED_ShowChinese(16*6,17+10,41,16,1);
				}
		
				
				switch(s)
				{
					case 1:  	  
								OLED_ShowString(16*4-8,16*3," ",16,1);
								OLED_ShowChar(8,16*3,'*',16,1);	
								OLED_Refresh();
								break;
			
					case 2: 	
									OLED_ShowString(8,16*3," ",16,1);
									OLED_ShowChar(16*4-8,16*3,'*',16,1);
									OLED_Refresh();
									break;
				}
				if(m==1)
				{
					alarm_clock_start=1;
					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_ALR, ENABLE);//使能秒中断   
					RTC_WaitForLastTask();
					
				}
				else if(m==2)
				{
					alarm_clock_start=0;
					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_ALR,DISABLE);//关闭秒中断   
					RTC_WaitForLastTask();
					
				}
			
		}
		else   //无闹钟
		{
			
			
			OLED_ShowChinese(16+12,16+6,47,16,1);
			OLED_ShowChinese(16*6,17+10,32,16,1);
			OLED_Refresh();
			
			while(1)
			{
				 if(exti_3==1)
				{
					exti_3=0;
					
					exti_0=0;
					exti_1=0;
					exti_2=0;
					
					alarm_menu=0;
					OLED_Clear();
					return ;
				}
			}
			
			
			
		}
		
	
	}


}


