#include "set_time_ui.h"
#include "rtc.h"
#include "Delay.h"
#include "oled.h"
#include "exti.h"

#include "LED.h"
#include "alarm_clock_ui.h"

	u8 s=1;   //选项
	u8 m=0;  //进入的菜单

		int time1[6];
u16 set_year;
u8 set_mon,set_day,set_hour,set_min,set_sec;

void confirm_set_time();
	

void set_ui(void)
{
	

	
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
	
		OLED_ShowChinese(8+8,16+6,25,16,1);
		OLED_ShowChinese(16+8+8,16+6,26,16,1);
		
		
			if(exti_2==1)  //设置选项列表
			{
				exti_2=0;
//				s++;
//				if(s==3)
//				{
//					s=1;
//				}
			}
			else if(exti_0==1)
			{
				exti_0=0;
				m=s;             //确认选项
			}
		else if(exti_3==1)   //返回总菜单界面
		{
			exti_3=0;
			
			exti_1=0;
			
			OLED_Clear();
			return ;
		}
			
		switch(s)
		{
			case 1 : 	OLED_ShowChar(0,16+6,'*',16,1);
								OLED_ShowChar(0,16*2+6,' ',16,1);
								OLED_Refresh();
								break;
			
//			case 2 :	OLED_ShowChar(0,16+6,' ',16,1);
//								OLED_ShowChar(0,16*2+6,'*',16,1);
//								OLED_Refresh(); 
//								break;
		
		}
			if(m==1)   
			{
				m=0;
				set_time();         //进入时间设置界面
				confirm_set_time(); //确认设置时间界面		
			}		
	}
	
}







void time_set_show()
{
	
		OLED_ShowChinese(16*3,0,25,16,1);
		OLED_ShowChinese(16*4,0,26,16,1);
	
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
		OLED_Refresh();

}


void set_time()
{
	
			OLED_Clear();
			//exti_1=0;
	
	  	RTC_Get();
	
			time1[0]=	ryear;
			time1[1]=rmon;
			time1[2]=rday;
			time1[3]=rhour;
			time1[4]=rmin;
			time1[5]=rsec;
	
			u8 flag=0;
	
			time_set_show();

			while(1)
			{
			
				if(flag==0)
				{
						while(1)
						{
			
							g:
							exti_back=0;
							if(exti_3==1)
							{
								exti_3=0;
								OLED_Clear();
								return ;  
							}
											
							else	if(exti_0==1)
							{

								exti_0=0;
								time1[5]++;
								if(time1[5]>59){time1[5]=0;}//秒越界判断
								
							}
							else if(exti_1==1)
							{

								exti_1=0;
								time1[5]--;
								
								if(time1[5]<0){time1[5]=59;}//秒越界判断 
								
							}
					
							else if(exti_2==1)    //按间要消抖，按得太快感应不了
							{
								exti_2=0;	
								flag=1;
								OLED_ShowNum(10*8,16*2,time1[5],2,16,1);
								OLED_Refresh();	
								break;
							}		
									
						
							if(exti_back==1)
							{
								goto g;
							}
							
							OLED_ShowNum(10*8,16*2,time1[5],2,16,1);
							Delay_ms(350);
							OLED_Refresh();	
							OLED_ShowString(10*8,16*2,"  ",16,1);	
							Delay_ms(350);
							OLED_Refresh();	

						}		

				
				}
			if(flag==1) 
				{
				
						while(1)
						{
							f:
							exti_back=0;
							if(exti_3==1)
							{
									exti_3=0;
									OLED_Clear();
									return ;  
							}
											
							else if(exti_0==1)
							{
								exti_0=0;
								time1[4]++;
								if(time1[4]>59){time1[4]=0;}//分越界判断
								
							}
							else if(exti_1==1)
							{

								exti_1=0;
								time1[4]--;
								if(time1[4]<0){time1[4]=59;}//分越界判断
							}
					
							else if(exti_2==1)    //按间要消抖，按得太快感应不了
							{
								exti_2=0;	
								flag=2;
								OLED_ShowNum(7*8,16*2,time1[4],2,16,1);
								OLED_Refresh();	
								break;
							}		
							if(exti_back==1)
							{
								goto f;
							}
							
							OLED_ShowNum(7*8,16*2,time1[4],2,16,1);	
							Delay_ms(350);	
							OLED_Refresh();								
								
							OLED_ShowString(7*8,16*2,"  ",16,1);	
							Delay_ms(350);
							OLED_Refresh();	
						}

				}
				if(flag==2)
				{
						while(1)
						{
							e:
							exti_back=0;
							if(exti_3==1)
							{
								exti_3=0;
								OLED_Clear();
								return ;  
							}
							
							else if(exti_0==1)
							{
								exti_0=0;
								time1[3]++;
								if(time1[3]>23){time1[3]=0;}//时越界判断	
							}
							else if(exti_1==1)
							{
								exti_1=0;
								time1[3]--;
								if(time1[3]<0){time1[3]=23;}//时越界判断	
							}
					
							else if(exti_2==1)
							{
							exti_2=0;
							flag=3;
							OLED_ShowNum(4*8,16*2,time1[3],2,16,1);
								OLED_Refresh();	
							break;
						
							}	
							
							if(exti_back==1)
							{
								goto e;
							}
							
							OLED_ShowNum(4*8,16*2,time1[3],2,16,1);
							Delay_ms(350);		
							OLED_Refresh();	
							OLED_ShowString(4*8,16*2,"  ",16,1);
							Delay_ms(350);
											OLED_Refresh();	
						}	 
				
				}
				if(flag==3)
				{
					while(1)
					{
						d:
						exti_back=0;
						if(exti_3==1)
						{
							exti_3=0;
				       OLED_Clear();
					     return ;     //结束函数
						}
						else if(exti_0==1)
						{
								exti_0=0;
								time1[2]++;
								if( time1[1]==1 || time1[1]==3 ||time1[1]==5 ||time1[1]==7 || time1[1]==8 || time1[1]==10 || time1[1]==12)//日越界判断
								{
									if(time1[2]>31){time1[2]=1;}//大月
								}
								else if(time1[1]==4 || time1[1]==6 || time1[1]==9 || time1[1]==11)
								{
									if(time1[2]>30){time1[2]=1;}//小月
								}
								else if(time1[1]==2)
								{
									if(time1[0]%4==0)
									{
										if(time1[2]>29){time1[2]=1;}//闰年2月
									}
									else
									{
										if(time1[2]>28){time1[2]=1;}//平年2月
									}
								}
						}
						else if(exti_1==1)
						{
								exti_1=0;
								time1[2]--;
							
								if( time1[1]==1 || time1[1]==3 || time1[1]==5 || time1[1]==7 || time1[1]==8 ||time1[1]==10||time1[1]==12)//日越界判断
								{
									if(time1[2]<1){time1[2]=31;}//大月
									if(time1[2]>31){time1[2]=1;}
								}
								else if(time1[1]==4 || time1[1]==6 ||time1[1]==9 ||time1[1]==11)
								{
									if(time1[2]<1){time1[2]=30;}//小月
									if(time1[2]>30){time1[2]=1;}
								}
								else if(time1[1]==2)
								{
									if(time1[0]%4==0)
									{
										if(time1[2]<1){time1[2]=29;}//闰年2月
										if(time1[2]>29){time1[2]=1;}
									}
									else
									{
										if(time1[2]<1){time1[2]=28;}//平年2月
										if(time1[2]>28){time1[2]=1;}
									}
								}	
						}
				
						else if(exti_2==1)
						{

							exti_2=0;
							flag=4;
							OLED_ShowNum(11*8,16,time1[2],2,16,1);
							OLED_Refresh();	
							break;    //跳出当前while
						}	
					
					if(exti_back==1)
					{
						goto d;
					}
						
						OLED_ShowNum(11*8,16,time1[2],2,16,1);
					 Delay_ms(350);
					OLED_Refresh();	
		       OLED_ShowString(11*8,16,"  ",16,1);	
						Delay_ms(350);
					OLED_Refresh();	
											
					}
	
				}
				
				if(flag==4)
				{
					while(1)
					{
						c:
						exti_back=0;
						if(exti_3==1)
						{
							exti_3=0;
							OLED_Clear();
							return ;  
						}
						else if(exti_0==1)
						{
							exti_0=0;
							time1[1]++;
							if(time1[1]>12){time1[1]=1;}//月越界判断
						}
						else if(exti_1==1)
						{
							exti_1=0;
							time1[1]--;
							if(time1[1]<1){time1[1]=12;}//月越界判断
							
						}
				
					else	if(exti_2==1)
						{
							
							exti_2=0;
							flag=5;
	            OLED_ShowNum(8*8,16,time1[1],2,16,1);
							break;
						}
					
						if(exti_back==1)
						{
							goto c;
						}
						
							OLED_ShowNum(8*8,16,time1[1],2,16,1);
							Delay_ms(350);
						OLED_Refresh();	
	
							OLED_ShowString(8*8,16,(u8*)"  ",16,1);
							Delay_ms(350);
							OLED_Refresh();	
					}

				}		
				if(flag==5)
				{
					while(1)
					{
						B:
						exti_back=0;
						if(exti_3==1)
						{
							exti_3=0;
							OLED_Clear();
							return ;  
						}
						
						else if(exti_0==1)
						{
							exti_0=0;
							time1[0]++;
							if(time1[0]>2099){time1[0]=1970;}//年越界判断		
						}
						else if(exti_1==1)
						{
							exti_1=0;
							time1[0]--;
							if(time1[0]<1970){time1[0]=2099;} //年越界判断
						}
						else if(exti_2==1)
						{
							exti_2=0;
							flag=0;
							OLED_ShowNum(3*8,16,time1[0],4,16,1);
							OLED_Refresh();	
							break;
						}
						
						if(exti_back==1)
						{
							goto B;
						}
						
						OLED_ShowNum(3*8,16,time1[0],4,16,1);
						Delay_ms(350);
						OLED_Refresh();	
						OLED_ShowString(3*8,16,"    ",16,1);
						Delay_ms(350);	
						OLED_Refresh();	
					}						
				}									
		}	
}




void confirm_set_time()
{
		u8 f=1;	
		u8 m=0;
	
		OLED_Clear();
		OLED_ShowChinese(0,0,32,16,0);   //是否保存设置
		OLED_ShowChinese(16,0,44,16,0);
		OLED_ShowChinese(16*2,0,45,16,0);
		OLED_ShowChinese(16*3,0,50,16,0);
		OLED_ShowChinese(16*4,0,51,16,0);	
		OLED_ShowChinese(16*5,0,27,16,0);	
		OLED_ShowChinese(16*6,0,28,16,0);	
		OLED_ShowChinese(16*7,0,32,16,0);	
	
		OLED_ShowChinese(16+4,16+6,44,16,1);    //是
		OLED_ShowChinese(16+4,16*2+10,45,16,1); //否
	
	
	while(1)
	{
			if(exti_2==1)
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
			else if(exti_0==1)
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
	
			if(m==1)  //保存设置
			{
			
				int ret=RTC_Set(time1[0],time1[1],time1[2],time1[3],time1[4],time1[5]);
				OLED_Clear();
				return ;
			}
			if(m==2) //取消设置
			{
			  OLED_Clear();
			  return ;
			
			}
			
			
			
			
			
	}








}




















