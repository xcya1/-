#include "Game_ui.h"
#include "oled.h"
#include "exti.h"
#include "Delay.h"

#include "LED.h"
#include "alarm_clock_ui.h"

void DinoGame();

void Game_ui()     //选择游戏界面
{
	u8 s=1,m=0;
	
	OLED_Clear();
	
	while(1)
	{
		OLED_ShowChinese(0,0,32,16,0);  //选项
		OLED_ShowChinese(16,0,32,16,0);
		OLED_ShowChinese(16*2,0,32,16,0);
		OLED_ShowChinese(16*3,0,48,16,0);
		OLED_ShowChinese(16*4,0,49,16,0);	
		OLED_ShowChinese(16*5,0,32,16,0);	
		OLED_ShowChinese(16*6,0,32,16,0);	
		OLED_ShowChinese(16*7,0,32,16,0);
	
		OLED_ShowChinese(16,16+6,66,16,1);   //谷歌小恐龙
		OLED_ShowChinese(16*2,16+6,67,16,1);
		OLED_ShowChinese(16*3,16+6,68,16,1);
		OLED_ShowChinese(16*4,16+6,69,16,1);
		OLED_ShowChinese(16*5,16+6,70,16,1);	
		
		
//		if(exti_2==1)
//		{
//			exti_2=0;
//			s++;
//			
//		}

		if(exti_3==1)
		{
			exti_3=0;
			
			exti_1=0;
			exti_2=0;
			
			OLED_Clear();
			return ;
		}
		else if(exti_0==1)
		{
			exti_0=0;
			
			exti_1=0;
			exti_2=0;
			
			m=s;
		}
		switch(s)
		{
			case 1: OLED_ShowChar(0,16+6,'*',16,1); OLED_Refresh();break;
	
		}
		
		if(m==1)
		{
			DinoGame();    //进入小恐龙游戏界面
			m=0;
		}
		
		
	}








}


void DinoGame()
{

	unsigned char cactus_category = 0;
	unsigned char cactus_length = 8;
	int cactus_pos = 128;
	int height = 0;
	char reset = 0;
 	char failed = 0;
	 u8 statr_Cover=1;

	OLED_Clear();
		
	
	while(1)
	{
	
		if(statr_Cover==1)   //游戏封面
		{
			
			OLED_DrawCover();
			
			
			if(exti_0==1)   //确认开启游戏
			{
					exti_0=0;
				
				exti_1=0;
				exti_2=0;
				
					statr_Cover=0;
					failed =0;
					failed = 0;
					height = 0;
					reset = 1;
					OLED_DrawDinoJump(reset);   //数据恢复默认
					OLED_DrawCactusRandom(cactus_category, reset);
					OLED_Clear();
			}
			
			if(exti_3==1)
			{
				exti_3=0;
				OLED_Clear();
				
				return ;
			}	
			continue;
		}
		
			
	
		if (failed == 1)  //游戏结束
		{
		
				OLED_ShowString(20,30,"G A M E  O V E R",8,1);
				OLED_Refresh(); 
				if (exti_2 == 1)  //重新开始
				{
					
					exti_2 =0;
					
					exti_0=0;
					exti_1=0;

					failed = 0;
					height = 0;
					reset = 1;
					OLED_DrawDinoJump(reset);
					OLED_DrawCactusRandom(cactus_category, reset);
					OLED_Clear();
				}
				
				if(exti_3==1)   //返回游戏菜单
				{
					exti_3=0;
					exti_0=0;
					exti_1=0;
					
					OLED_Clear();
					failed = 0;
					height = 0;
					reset = 1;
					failed =0;			
					
				
					return ;
				}
				continue;
		}
	
	//score ++;
	OLED_DrawGround();
	OLED_DrawCloud();
	
	
	if(exti_1==1 || height >0 )
	{
		exti_1=0;
		
		exti_0=0;
		exti_2=0;
		exti_3=0;
		
		height=OLED_DrawDinoJump(reset);	
	}
	else
	{
		OLED_DrawDino();
	}
		cactus_pos = OLED_DrawCactusRandom(cactus_category, 0);
		if(cactus_category == 0) cactus_length = 8;
		else if(cactus_category == 1) cactus_length = 16;
		else cactus_length = 24;
		if (cactus_pos + cactus_length < 0)
		{
		  cactus_category = rand()%4;
			OLED_DrawCactusRandom(cactus_category, 1);
		}
		if ((height < 16) && ( (cactus_pos>=16 && cactus_pos <=32) || (cactus_pos + cactus_length>=16 && cactus_pos + cactus_length <=32)))
		{
			failed = 1;
			OLED_Clear();
		}
	reset = 0;
	Delay_ms(50);

	}




}


