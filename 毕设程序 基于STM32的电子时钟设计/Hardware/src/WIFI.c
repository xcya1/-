#include "WIFI.h"
#include "string.h"
#include "USART3_TIM3_wifi.h"	  
#include "USART2.h"	  
#include "stm32f10x.h"
#include "Delay.h"
#include "stdlib.h"
#include "malloc.h"
#include "cJSON.h"
#include "stdio.h"	

#include "oled.h"

u8 WIFI_connect_succeed;


const u8* wifista_ssid="xc";			//路由器SSID号
const u8* wifista_password="123456xc"; 	//连接密码
 
#define WEATHER_PORTNUM 	"80"		//天气连接端口号:80	
#define WEATHER_SERVERIP 	"api.seniverse.com"		//天气服务器IP    // 

#define TIME_SERVERIP 	"api.k780.com"		//时间服务器IP    api.k780.com 		

#define TIME_PORTNUM 	"80"		//天气连接端口号:80	
u8 *p;
Results results[] = {{0}};       //----------------数据类型Results  数组名 results

char timestamp[12];    //-----------------时间


//ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果;其他,期待应答结果的位置(str的位置)
u8* esp8266_check_cmd(u8 *str)
{
	char *strx=0;               //空指针，避免出现野指针
	if(USART_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//添加结束符，----------------------字符串数组的\0
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);   //-------strstr（）判断目标字符串中包不包含某str，如果包含返回目标字符串中str的位置
	} 
	return (u8*)strx;     //-----返回USART_RX_BUF字符串中出现ack的地址
}


//向ESP8266发送命令
//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败
u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART_RX_STA=0;
	printf("%s\r\n",cmd);	//发送命令-----------------------发送AT指令配置WIFI模块
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时  ，--------------------等待WIFI模块回复，如果长时间没回复就超时了
		{
			Delay_ms(10);
			if(USART_RX_STA&0X8000)//接收完成   -------------USART_RX_STA的最高位接收完后，在串口接收中会被置1  &按位与
			{
				if(esp8266_check_cmd(ack))   //----------------esp8266_check_cmd函数结果返回不为NULL，成功响应，检查数据
				{
					u2_printf("ACK:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1;          //，--------------------超时代表WIFI模块没响应
	}
	return res;
} 


//连接WIFI
//返回值:0,成功;1,失败
u8 esp8266_start_trans(void)
{
	u8 *p;
	u8 res=1; 
	int trans_time=0;
	p=mymalloc(SRAMIN,50);							//申请32字节内存，用于存wifista_ssid，wifista_password
	
	
	
	u2_printf("send:AT\r\n");	          //---------用串口2调试，调试信息
	while(esp8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
	{
	} 
	//设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
	u2_printf("send:AT+CWMODE=1\r\n");	
	esp8266_send_cmd("AT+CWMODE=1","OK",100);
	
	//Wifi模块重启
	u2_printf("send:AT+RST\r\n");	
	esp8266_send_cmd("AT+RST","OK",50);
	Delay_ms(1000);         //延时3S等待重启成功
	Delay_ms(1000);
	Delay_ms(1000);	
	
	//设置单路连接模式
	u2_printf("send:AT+CIPMUX=0\r\n");	
	esp8266_send_cmd("AT+CIPMUX=0","OK",20);   

	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	u2_printf("send:AT+CWJAP=\"%s\",\"%s\"\r\n",wifista_ssid,wifista_password);
	
	while(esp8266_send_cmd(p,"WIFI GOT IP",300))		//连接目标路由器,并且获得IP
	{
		 //连接不到退出
		 Delay_ms(500);
     trans_time++;
		 if(trans_time>8)   //进入if路由器连接超时
		 {
		   return 10;       //---------可以在主函数中判断，如果返回10，显示WIFI连接失败
		 }
	}
	myfree(SRAMIN,p);
	return 0;		
}


//ATK-ESP8266退出透传模式
//返回值:0,成功;1,失败
u8 atk_8266_quit_trans(void)
{
	while((USART1->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	Delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART1->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	Delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART1->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	Delay_ms(500);					//等待500ms
	return esp8266_send_cmd("AT","OK",20);//退出透传判断.
}


//获取一次实时天气
//返回：0---获取成功，1---获取失败
u8 get_current_weather(void)
{
	u8 res;
	p=mymalloc(SRAMIN,40);							//申请40字节内存
	
	//配置目标TCP服务器
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",WEATHER_SERVERIP,WEATHER_PORTNUM);    
	u2_printf("send:%s\r\n",p);  
	res = esp8266_send_cmd(p,"OK",100);//连接到目标TCP服务器
	if(res==1)                          //返回为1连接失败
	{
		myfree(SRAMIN,p);
		return 1;
	}
	Delay_ms(300);
	
	
	//传输模式为：透传	
	u2_printf("send:AT+CIPMODE=1\r\n");	
	esp8266_send_cmd("AT+CIPMODE=1","OK",100);      
	
	
	//开始透传
	USART_RX_STA=0;
	u2_printf("send:AT+CIPSEND\r\n");	
	esp8266_send_cmd("AT+CIPSEND","OK",100);         //参数1AT，参数2检查接收的数据中是否有OK，参数3 时长 

	u2_printf("GET https://api.seniverse.com/v3/weather/now.json?key=SYjNypb_cjhFbSr5n&location=beijing&language=en&unit=c\r\n"); 

	printf("GET https://api.seniverse.com/v3/weather/now.json?key=SYjNypb_cjhFbSr5n&location=beijing&language=en&unit=c\r\n");    //直接发不用检查接
	
	
	
	Delay_ms(20);//延时20ms返回的是指令发送成功的状态
	USART_RX_STA=0;	//清零串口数据
	Delay_ms(1000);
	if(USART_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//添加结束符
	} 
		
	//	u1_printf("DATA=%s\r\n",USART_RX_BUF);   //------这个不屏蔽调可能会出错
   u2_printf("DATA=%s\r\n",USART_RX_BUF);
	//解析天气数据
	cJSON_WeatherParse(USART_RX_BUF, results);	

cJSON_time(USART_RX_BUF);
	
//	//打印结构体内内容
//	u1_printf("city:%s \r\n",results[0].location.name);//城市
//	u1_printf("weather:%s \r\n",results[0].now.text);//天气
//	u1_printf("code:%s \r\n",results[0].now.code);//风力
//	u1_printf("temperature:%s \r\n",results[0].now.temperature);//温度
//	u1_printf("time:%s \r\n",results[0].last_update);//时间

	
	//退出透传;	
	atk_8266_quit_trans();   //未退出透传模式，发送指令返回乱码数据

	//关闭TCP连接
	u2_printf("send:AT+CIPCLOSE\r\n");	    //-----------断开服务器连接
	esp8266_send_cmd("AT+CIPCLOSE","OK",50);    
	
	myfree(SRAMIN,p);
	return 0;
}


u8 get_current_time(void)
{


		u8 res;
		p=mymalloc(SRAMIN,40);							//申请40字节内存
		//配置目标TCP服务器
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",TIME_SERVERIP ,TIME_PORTNUM );    
		u2_printf("send:%s\r\n",p);  
		res = esp8266_send_cmd(p,"OK",100);//连接到目标TCP服务器
		if(res==1)                          //返回为1连接失败
		{
			myfree(SRAMIN,p);
			return 1;
		}
		Delay_ms(300);
		//传输模式为：透传	
		u2_printf("send:AT+CIPMODE=1\r\n");	
		esp8266_send_cmd("AT+CIPMODE=1","OK",100);      
		//开始透传
		USART_RX_STA=0;
		u2_printf("send:AT+CIPSEND\r\n");	
		esp8266_send_cmd("AT+CIPSEND","OK",100);         //参数1AT，参数2检查接收的数据中是否有OK，参数3 时长 

		u2_printf("GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json&HTTP/1.1\r\n"); 
		printf("GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json&HTTP/1.1\r\n");    //直接发不用检查接
		
		Delay_ms(20);//延时20ms返回的是指令发送成功的状态
		USART_RX_STA=0;	//清零串口数据
		Delay_ms(1000);
		if(USART_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
		{ 
			USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//添加结束符
		} 
	u2_printf("DATA=%s\r\n",USART_RX_BUF);
		//解析时间数据
		cJSON_time(USART_RX_BUF);
		
		//退出透传;	
		atk_8266_quit_trans();   //未退出透传模式，发送指令返回乱码数据

		//关闭TCP连接
		u2_printf("send:AT+CIPCLOSE\r\n");	    //-----------断开服务器连接
		esp8266_send_cmd("AT+CIPCLOSE","OK",50);    
		
		myfree(SRAMIN,p);
		return 0;
}



/***************************解析时间JSON*****************************/

int cJSON_time(char *JSON)
{
	cJSON *json,*arrayItem,*object,*subobject,*item;
	
	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		u2_printf("Error before: [%s] \r\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		item=json->child;
		while(item)
		{
				char *string=item->string;
				if(!strcmp(string,"result"))
				{
						if(subobject=cJSON_GetObjectItem(item,"timestamp"))
						{
							u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",subobject->type,subobject->string,subobject->valuestring);
							memcpy(timestamp,subobject->valuestring,strlen(subobject->valuestring));
						}				
				}
				item=item->next;		
		}
	}
	cJSON_Delete(json); 

}




/*********************************************************************************
* Function Name    ： cJSON_WeatherParse,解析天气数据
* Parameter		   ： JSON：天气数据包  results：保存解析后得到的有用的数据
* Return Value     ： 0：成功 其他:错误
* Function Explain ： 
* Create Date      ： 2017.12.6 by lzn
**********************************************************************************/
int cJSON_WeatherParse(char *JSON, Results *results)
{
	cJSON *json,*arrayItem,*object,*subobject,*item;
	
	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		u2_printf("Error before: [%s] \r\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //匹配字符串"results",获取数组内容
		{
			int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
			u2_printf("cJSON_GetArraySize: size=%d \r\n",size); 
			
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//获取父对象内容
			{
				/* 匹配子对象1 */
//				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
//				{
//					printf("---------------------------------subobject1-------------------------------\r\n");
//					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   //匹配子对象1成员"id"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.id,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //匹配子对象1成员"name"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.name,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)//匹配子对象1成员"country"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.country,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  //匹配子对象1成员"path"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.path,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)//匹配子对象1成员"timezone"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.timezone,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)//匹配子对象1成员"timezone_offset"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.timezone_offset,item->valuestring,strlen(item->valuestring));
//					}
//				}
				/* 匹配子对象2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					u2_printf("---------------------------------subobject2-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//匹配子对象2成员"text"
					{
						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.text,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//匹配子对象2成员"code"
					{
						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.code,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //匹配子对象2成员"temperature"
					{
						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.temperature,item->valuestring,strlen(item->valuestring));
					}
				}
				/* 匹配子对象3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					u2_printf("---------------------------------subobject3-------------------------------\r\n");
					u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",subobject->type,subobject->string,subobject->valuestring);
					memcpy(results[0].last_update,subobject->valuestring,strlen(subobject->valuestring));
				}
			} 
		}
	}
	
	cJSON_Delete(json); //释放cJSON_Parse()分配出来的内存空间
	
	return 0;
}


void acquire_weather_message()
{
	
	if(esp8266_start_trans())
	{
		WIFI_connect_succeed=0;
//		return ;
	}
	else
	{
		WIFI_connect_succeed=1;
	}

	for(u8 i=0;i<3;i++)
	{
		Delay_ms(500);
		get_current_weather();
	
	}

}



void acquire_time_message()
{
	//esp8266_start_trans();
	
	for(u8 i=0;i<3;i++)
	{
		Delay_ms(500);
		get_current_time();
	
	}


}


//	
//	USART1_Send_String("AT+RESTORE\r\n");
//	Delay_ms(2000);
//	USART1_Send_String("AT+CWMODE=1\r\n");   //设为服务器
//	Delay_ms(500);
//	USART1_Send_String("AT+CIPMUX=0\r\n");           //单连接模式
//	Delay_ms(1000);
//	USART1_Send_String("AT+CWJAP=\"xc\",\"123456xc\"\r\n");
//	Delay_ms(7000);
//	USART1_Send_String("AT+CIPSTART=\"TCP\",\"116.62.81.138\",80\r\n");  
//	Delay_ms(2000);
//	USART1_Send_String("AT+CIPMODE=1\r\n");  
//	Delay_ms(500);
//	
//		USART1_Send_String("AT+CIPSEND\r\n");
//	Delay_ms(500);



//	USART1_Send_String("rse.com/v3/weather/now.json?key=SYjNypb_cjhFbSr5n&location=beijing&language=en&unit=c\r\n");
//	Delay_ms(500);







