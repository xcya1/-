#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "Delay.h"

u8 OLED_GRAM[144][8];

//反显函数
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//延时
void IIC_delay(void)
{
	u8 t=10;
	while(t--);
}

//起始信号
void I2C_Start(void)
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//结束信号
void I2C_Stop(void)
{
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}

//等待信号响应
void I2C_WaitAck(void) //测数据信号的电平
{
	OLED_SDA_Set();
	IIC_delay();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

//写入一个字节
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)//将dat的8位从最高位依次写入
		{
			OLED_SDA_Set();
    }
		else
		{
			OLED_SDA_Clr();
    }
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();//将时钟信号设置为低电平
		dat<<=1;
  }
}

//发送一个字节
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
  else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

//开启OLED显示 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}

//更新显存到OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for(n=0;n<128;n++)
		{
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
  }
}
//清屏函数
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空	
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}

//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
//mode:0,反色显示;1,正常显示
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//显示数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,反色显示;1,正常显示
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
	u8 m,temp;
	u8 x0=x,y0=y;
	u16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
	for(i=0;i<size3;i++)
	{
		if(size1==16)
				{temp=Hzk1[num][i];}//调用16*16字体
		else if(size1==24)
				{temp=Hzk2[num][i];}//调用24*24字体
		else if(size1==32)       
				{temp=Hzk3[num][i];}//调用32*32字体
		else if(size1==64)
				{temp=Hzk4[num][i];}//调用64*64字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{x=x0;y0=y0+8;}
		y=y0;
	}
}

//num 显示汉字的个数
//space 每一遍显示的间隔
//mode:0,反色显示;1,正常显示
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //显示间隔
				 {
					for(i=1;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=1;i<144;i++)   //实现左移
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}
//OLED的初始化
void OLED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6);	
	
	
	
	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//使能PORTA~E,PORTG时钟
//  	
//	//GPIO初始化设置
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;              //SDA
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
//	
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
////	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
////	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
//	
//	//GPIO初始化设置
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;           //SCL
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	
//	OLED_RES_Clr();
			Delay_ms(200);
//	OLED_RES_Set();
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD);
}


//------------------------------------------------------------------------------------

void OLED_SetPos(unsigned char x, unsigned char y)
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD);
}




// 快速绘制图像
void OLED_DrawBMPFast(const unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	for (y = 0; y < 8; y++)  //一共有8页（0~7）行，一页8个像素点， 循环8次可以覆盖全部就为一张图片
	{                       
		OLED_SetPos(0, y);
		I2C_Start();  
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for (x = 0; x < 128; x++)  //128列（0~127）
		{
			Send_Byte(BMP[j++]);
			I2C_WaitAck();
		}
		I2C_Stop();
	}
}

void oled_drawbmp_block_clear(int bx, int by, int clear_size)
{
	unsigned int i;
	OLED_SetPos(bx, by);
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	Send_Byte(0x40);
	I2C_WaitAck();

	for (i = 0; i < clear_size; i++)
	{
		if (bx + i>128) break;
		Send_Byte(0x0);
		I2C_WaitAck();
	}
	I2C_Stop();
}





// 绘制地面
void OLED_DrawGround()
{
	static unsigned int pos = 0;               //这里的pos代表显示数据数组中的第几个          
	unsigned char speed = 5;                   //每次移动的像素  每次向左移的个数（列）
	unsigned int ground_length = sizeof(GROUND);  //横向有多少个像素点，地面的长度 x
	unsigned char x;

	OLED_SetPos(0, 7);   //在第7页显示地面  y   //设置显示位置再第7页
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	Send_Byte(0x40);
	I2C_WaitAck();
	for (x = 0; x < 128; x++)
	{
		Send_Byte(GROUND[(x+pos)%ground_length]);    //取余防越界，当位置超过从源头重新取数据，数据是循环显示，类似一帧一帧显示，越界就从头0开始取，一次循环
		I2C_WaitAck();                              //       1 1 1 1 1
	}                                             //       1        1
	I2C_Stop();                                    //      1  1 1 1 1      //128列都会被取满，所以不用消隐（被覆盖了）
                                    //显示完一张后（128），再开始 偏移
	pos = pos + speed;               //当前位置加上偏移位置得到新的位置
	//if(pos>ground_length) pos=0;

}


// 绘制云朵
void OLED_DrawCloud()
{
	static int pos = 128;    //从右由往左，初始位置为128  ，因为地面是从左向右的，要返着来才能看出动态，参考系不一样
	static char height=0;
	char speed = 3;          //每次向左移的个数（列）
	unsigned int i=0;
	int x;
	int start_x = 0;
	int length = sizeof(CLOUD);  //云长度
	unsigned char byte;

	//if (pos + length <= -speed) pos = 128;

	if (pos + length <= -speed)    //如果整个云，越界了，从128再开始，OLED_SetPos(128, 0)表再第128列，第0页显示，128已经越界了，OLED屏不会显示
	{
		pos = 128;                   //x  第几列
		height = rand()%3;            //y  第几行（页）
	}
	if(pos < 0)                    //云有一部分出界面，此时再左边
	{
		start_x = -pos;
		OLED_SetPos(0, 1+height);    //在坐标(0, 1+height)处，显示部分云
	}
	else                            //云没越界
	{
		OLED_SetPos(pos, 1+height);    //在坐标（pos, 1+height）会显示正个云再界面中
	}
		//上面确认显示位置
	//----------------------------------
	//下面显示图片
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	Send_Byte(0x40);
	I2C_WaitAck();
	for (x = start_x; x < length + speed; x++)   //显示云数据（图片）
	{
		if (pos + x > 127) break;           //如果显示的位置越界，结束
		if (x < length) byte = CLOUD[x];
		else byte = 0x0;                       //局部刷新，要消影    云显示的大小加了个speed，显示会重叠，要消影

		Send_Byte(byte);
		I2C_WaitAck();
	}
	I2C_Stop();

	pos = pos - speed;  //向左移
}

// 绘制小恐龙
void OLED_DrawDino()
{
	static unsigned char dino_dir = 0;  //用于判断显示恐龙左右脚图片
	unsigned int j=0;
	unsigned char x, y;
	unsigned char byte;

	dino_dir++;
	dino_dir = dino_dir%2;    //越界清0 
	for(y=0; y<2; y++)        //恐龙图片占2页 先显示6页的8个像素，在显示7页的像素
	{
		OLED_SetPos(16, 6+y);   //确定显示位置，恐龙静止在一个地方没有动
		
		
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for (x = 0; x < 16; x++)    //依次显示16个数据
		{
			j = y*16 + x;
			byte = DINO[dino_dir][j];    //dino_dir为0左脚图片，1右脚图片，交互显示形成动图
                                   
			Send_Byte(byte);
			I2C_WaitAck();
		}
		I2C_Stop();
	}
}


// 绘制仙人掌障碍物
void OLED_DrawCactus()
{
	char speed = 5;
	static int pos = 128;
	int start_x = 0;
	int length = sizeof(CACTUS_1)/2;

	unsigned int j=0;               //与云朵相似，先确定显示的位置，在显示图片，图片也会有部分和整个，也要消影
	unsigned char x, y;
	unsigned char byte;

	if (pos + length <= 0)
	{
		oled_drawbmp_block_clear(0, 6, speed);
		pos = 128;
	}

	for(y=0; y<2; y++)
	{
		if(pos < 0)
		{
			start_x = -pos;
			OLED_SetPos(0, 6+y);
		}
		else
		{
			OLED_SetPos(pos, 6+y);
		}
	//上面确认显示位置
	//----------------------------------
	//下面显示图片
		
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();

		for (x = start_x; x < length; x++)
		{
			if (pos + x > 127) break;
			j = y*length + x;
			byte = CACTUS_1[j];
			Send_Byte(byte);
			I2C_WaitAck();
		}
		I2C_Stop();
	}                           
	oled_drawbmp_block_clear(pos + length, 6, speed); //------ 清除残影---------
	pos = pos - speed;
}

// 绘制随机出现的仙人掌障碍物
int OLED_DrawCactusRandom(unsigned char ver, unsigned char reset)
{
	char speed = 5;
	static int pos = 128;
	int start_x = 0;
	int length = 0;

	unsigned int i=0, j=0;
	unsigned char x, y;
	unsigned char byte;
	if (reset == 1)
	{
		pos = 128;
		oled_drawbmp_block_clear(0, 6, speed);
		return 128;
	}
	if (ver == 0) length = 8; //sizeof(CACTUS_1) / 2;
	else if (ver == 1) length = 16; //sizeof(CACTUS_2) / 2;
	else if (ver == 2 || ver == 3) length = 24;

	for(y=0; y<2; y++)
	{
		if(pos < 0)
		{
			start_x = -pos;
			OLED_SetPos(0, 6+y);
		}
		else
		{
			OLED_SetPos(pos, 6+y);
		}

		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();

		for (x = start_x; x < length; x++)
		{
			if (pos + x > 127) break;

			j = y*length + x;
			if (ver == 0) byte = CACTUS_1[j];
			else if (ver == 1) byte = CACTUS_2[j];
			else if(ver == 2) byte = CACTUS_3[j];
			else byte = CACTUS_4[j];

			Send_Byte(byte);
			I2C_WaitAck();
		}
		I2C_Stop();
	}

	oled_drawbmp_block_clear(pos + length, 6, speed);  //消影

	pos = pos - speed;
	return pos + speed;
}

// 绘制跳跃小恐龙
int OLED_DrawDinoJump(char reset)
{
	char speed_arr[] = {1, 1, 3, 3, 4, 4, 5, 6, 7};  //跳跃速度不是恒定的
	static char speed_idx = sizeof(speed_arr)-1;
	static int height = 0;    //高度
	static char dir = 0;      //方向，0向上跳，1从下坠
	//char speed = 4;

	unsigned int j=0;
	unsigned char x, y;
	char offset = 0;
	unsigned char byte;
	if(reset == 1)
	{
		height = 0;
		dir = 0;
		speed_idx = sizeof(speed_arr)-1;
		return 0;
	}
	if (dir==0)  //动作向上
	{
		height += speed_arr[speed_idx];
		speed_idx --;
		if (speed_idx<0) speed_idx = 0;
	}
	if (dir==1) //动作向下
	{
		height -= speed_arr[speed_idx];
		speed_idx ++;
		if (speed_idx>sizeof(speed_arr)-1) speed_idx = sizeof(speed_arr)-1;
	}
	if(height >= 31)  //高度跳到最高是第2页，h0~32个像素
	{
		dir = 1;        //到达最高处，下坠
		height = 31;
	}
	if(height <= 0)  //降到地面
	{
		dir = 0;
		height = 0;
	}
	if(height <= 7) offset = 0;         //根据高度偏移，依次来判断在那个位置绘图
	else if(height <= 15) offset = 1;
	else if(height <= 23) offset = 2;
	else if(height <= 31) offset = 3;
	else offset = 4;

	for(y=0; y<3; y++) // 4
	{
		OLED_SetPos(16, 5- offset + y);  //位置

		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for (x = 0; x < 16; x++) // 32
		{
			j = y*16 + x; // 32
			byte = DINO_JUMP[height%8][j];    //数据

			Send_Byte(byte);
			I2C_WaitAck();
		}
		I2C_Stop();
	}
	if (dir == 0) oled_drawbmp_block_clear(16, 8- offset, 16);    //消影
	if (dir == 1) oled_drawbmp_block_clear(16, 4- offset, 16);   //消影
	return height;
}

// 绘制游戏重启
void OLED_DrawRestart()
{
	unsigned int j=0;
	unsigned char x, y;
	unsigned char byte;
	//OLED_SetPos(0, 0);
	for (y = 2; y < 5; y++)
	{
		OLED_SetPos(52, y);
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for (x = 0; x < 24; x++)
		{
			byte = RESTART[j++];
			Send_Byte(byte);
			I2C_WaitAck();
		}
		I2C_Stop();
	}
	OLED_ShowString(10, 3, (u8*)"GAME", 16,1);
	OLED_ShowString(86, 3, (u8*)"OVER", 16,1);
}

// 绘制封面
void OLED_DrawCover()
{
	OLED_DrawBMPFast(COVER);
}
