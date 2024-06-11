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


const u8* wifista_ssid="xc";			//·����SSID��
const u8* wifista_password="123456xc"; 	//��������
 
#define WEATHER_PORTNUM 	"80"		//�������Ӷ˿ں�:80	
#define WEATHER_SERVERIP 	"api.seniverse.com"		//����������IP    // 

#define TIME_SERVERIP 	"api.k780.com"		//ʱ�������IP    api.k780.com 		

#define TIME_PORTNUM 	"80"		//�������Ӷ˿ں�:80	
u8 *p;
Results results[] = {{0}};       //----------------��������Results  ������ results

char timestamp[12];    //-----------------ʱ��


//ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����;����,�ڴ�Ӧ������λ��(str��λ��)
u8* esp8266_check_cmd(u8 *str)
{
	char *strx=0;               //��ָ�룬�������Ұָ��
	if(USART_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//��ӽ�������----------------------�ַ��������\0
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);   //-------strstr�����ж�Ŀ���ַ����а�������ĳstr�������������Ŀ���ַ�����str��λ��
	} 
	return (u8*)strx;     //-----����USART_RX_BUF�ַ����г���ack�ĵ�ַ
}


//��ESP8266��������
//cmd:���͵������ַ���;ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��;waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����);1,����ʧ��
u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART_RX_STA=0;
	printf("%s\r\n",cmd);	//��������-----------------------����ATָ������WIFIģ��
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ  ��--------------------�ȴ�WIFIģ��ظ��������ʱ��û�ظ��ͳ�ʱ��
		{
			Delay_ms(10);
			if(USART_RX_STA&0X8000)//�������   -------------USART_RX_STA�����λ��������ڴ��ڽ����лᱻ��1  &��λ��
			{
				if(esp8266_check_cmd(ack))   //----------------esp8266_check_cmd����������ز�ΪNULL���ɹ���Ӧ���������
				{
					u2_printf("ACK:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
					USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1;          //��--------------------��ʱ����WIFIģ��û��Ӧ
	}
	return res;
} 


//����WIFI
//����ֵ:0,�ɹ�;1,ʧ��
u8 esp8266_start_trans(void)
{
	u8 *p;
	u8 res=1; 
	int trans_time=0;
	p=mymalloc(SRAMIN,50);							//����32�ֽ��ڴ棬���ڴ�wifista_ssid��wifista_password
	
	
	
	u2_printf("send:AT\r\n");	          //---------�ô���2���ԣ�������Ϣ
	while(esp8266_send_cmd("AT","OK",20))//���WIFIģ���Ƿ�����
	{
	} 
	//���ù���ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
	u2_printf("send:AT+CWMODE=1\r\n");	
	esp8266_send_cmd("AT+CWMODE=1","OK",100);
	
	//Wifiģ������
	u2_printf("send:AT+RST\r\n");	
	esp8266_send_cmd("AT+RST","OK",50);
	Delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	Delay_ms(1000);
	Delay_ms(1000);	
	
	//���õ�·����ģʽ
	u2_printf("send:AT+CIPMUX=0\r\n");	
	esp8266_send_cmd("AT+CIPMUX=0","OK",20);   

	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	u2_printf("send:AT+CWJAP=\"%s\",\"%s\"\r\n",wifista_ssid,wifista_password);
	
	while(esp8266_send_cmd(p,"WIFI GOT IP",300))		//����Ŀ��·����,���һ��IP
	{
		 //���Ӳ����˳�
		 Delay_ms(500);
     trans_time++;
		 if(trans_time>8)   //����if·�������ӳ�ʱ
		 {
		   return 10;       //---------���������������жϣ��������10����ʾWIFI����ʧ��
		 }
	}
	myfree(SRAMIN,p);
	return 0;		
}


//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�ɹ�;1,ʧ��
u8 atk_8266_quit_trans(void)
{
	while((USART1->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	Delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART1->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	Delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART1->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	Delay_ms(500);					//�ȴ�500ms
	return esp8266_send_cmd("AT","OK",20);//�˳�͸���ж�.
}


//��ȡһ��ʵʱ����
//���أ�0---��ȡ�ɹ���1---��ȡʧ��
u8 get_current_weather(void)
{
	u8 res;
	p=mymalloc(SRAMIN,40);							//����40�ֽ��ڴ�
	
	//����Ŀ��TCP������
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",WEATHER_SERVERIP,WEATHER_PORTNUM);    
	u2_printf("send:%s\r\n",p);  
	res = esp8266_send_cmd(p,"OK",100);//���ӵ�Ŀ��TCP������
	if(res==1)                          //����Ϊ1����ʧ��
	{
		myfree(SRAMIN,p);
		return 1;
	}
	Delay_ms(300);
	
	
	//����ģʽΪ��͸��	
	u2_printf("send:AT+CIPMODE=1\r\n");	
	esp8266_send_cmd("AT+CIPMODE=1","OK",100);      
	
	
	//��ʼ͸��
	USART_RX_STA=0;
	u2_printf("send:AT+CIPSEND\r\n");	
	esp8266_send_cmd("AT+CIPSEND","OK",100);         //����1AT������2�����յ��������Ƿ���OK������3 ʱ�� 

	u2_printf("GET https://api.seniverse.com/v3/weather/now.json?key=SYjNypb_cjhFbSr5n&location=beijing&language=en&unit=c\r\n"); 

	printf("GET https://api.seniverse.com/v3/weather/now.json?key=SYjNypb_cjhFbSr5n&location=beijing&language=en&unit=c\r\n");    //ֱ�ӷ����ü���
	
	
	
	Delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
	USART_RX_STA=0;	//���㴮������
	Delay_ms(1000);
	if(USART_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//��ӽ�����
	} 
		
	//	u1_printf("DATA=%s\r\n",USART_RX_BUF);   //------��������ε����ܻ����
   u2_printf("DATA=%s\r\n",USART_RX_BUF);
	//������������
	cJSON_WeatherParse(USART_RX_BUF, results);	

cJSON_time(USART_RX_BUF);
	
//	//��ӡ�ṹ��������
//	u1_printf("city:%s \r\n",results[0].location.name);//����
//	u1_printf("weather:%s \r\n",results[0].now.text);//����
//	u1_printf("code:%s \r\n",results[0].now.code);//����
//	u1_printf("temperature:%s \r\n",results[0].now.temperature);//�¶�
//	u1_printf("time:%s \r\n",results[0].last_update);//ʱ��

	
	//�˳�͸��;	
	atk_8266_quit_trans();   //δ�˳�͸��ģʽ������ָ�����������

	//�ر�TCP����
	u2_printf("send:AT+CIPCLOSE\r\n");	    //-----------�Ͽ�����������
	esp8266_send_cmd("AT+CIPCLOSE","OK",50);    
	
	myfree(SRAMIN,p);
	return 0;
}


u8 get_current_time(void)
{


		u8 res;
		p=mymalloc(SRAMIN,40);							//����40�ֽ��ڴ�
		//����Ŀ��TCP������
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",TIME_SERVERIP ,TIME_PORTNUM );    
		u2_printf("send:%s\r\n",p);  
		res = esp8266_send_cmd(p,"OK",100);//���ӵ�Ŀ��TCP������
		if(res==1)                          //����Ϊ1����ʧ��
		{
			myfree(SRAMIN,p);
			return 1;
		}
		Delay_ms(300);
		//����ģʽΪ��͸��	
		u2_printf("send:AT+CIPMODE=1\r\n");	
		esp8266_send_cmd("AT+CIPMODE=1","OK",100);      
		//��ʼ͸��
		USART_RX_STA=0;
		u2_printf("send:AT+CIPSEND\r\n");	
		esp8266_send_cmd("AT+CIPSEND","OK",100);         //����1AT������2�����յ��������Ƿ���OK������3 ʱ�� 

		u2_printf("GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json&HTTP/1.1\r\n"); 
		printf("GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json&HTTP/1.1\r\n");    //ֱ�ӷ����ü���
		
		Delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
		USART_RX_STA=0;	//���㴮������
		Delay_ms(1000);
		if(USART_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
		{ 
			USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//��ӽ�����
		} 
	u2_printf("DATA=%s\r\n",USART_RX_BUF);
		//����ʱ������
		cJSON_time(USART_RX_BUF);
		
		//�˳�͸��;	
		atk_8266_quit_trans();   //δ�˳�͸��ģʽ������ָ�����������

		//�ر�TCP����
		u2_printf("send:AT+CIPCLOSE\r\n");	    //-----------�Ͽ�����������
		esp8266_send_cmd("AT+CIPCLOSE","OK",50);    
		
		myfree(SRAMIN,p);
		return 0;
}



/***************************����ʱ��JSON*****************************/

int cJSON_time(char *JSON)
{
	cJSON *json,*arrayItem,*object,*subobject,*item;
	
	json = cJSON_Parse(JSON); //����JSON���ݰ�
	if(json == NULL)		  //���JSON���ݰ��Ƿ�����﷨�ϵĴ��󣬷���NULL��ʾ���ݰ���Ч
	{
		u2_printf("Error before: [%s] \r\n",cJSON_GetErrorPtr()); //��ӡ���ݰ��﷨�����λ��
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
* Function Name    �� cJSON_WeatherParse,������������
* Parameter		   �� JSON���������ݰ�  results�����������õ������õ�����
* Return Value     �� 0���ɹ� ����:����
* Function Explain �� 
* Create Date      �� 2017.12.6 by lzn
**********************************************************************************/
int cJSON_WeatherParse(char *JSON, Results *results)
{
	cJSON *json,*arrayItem,*object,*subobject,*item;
	
	json = cJSON_Parse(JSON); //����JSON���ݰ�
	if(json == NULL)		  //���JSON���ݰ��Ƿ�����﷨�ϵĴ��󣬷���NULL��ʾ���ݰ���Ч
	{
		u2_printf("Error before: [%s] \r\n",cJSON_GetErrorPtr()); //��ӡ���ݰ��﷨�����λ��
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //ƥ���ַ���"results",��ȡ��������
		{
			int size = cJSON_GetArraySize(arrayItem);     //��ȡ�����ж������
			u2_printf("cJSON_GetArraySize: size=%d \r\n",size); 
			
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//��ȡ����������
			{
				/* ƥ���Ӷ���1 */
//				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
//				{
//					printf("---------------------------------subobject1-------------------------------\r\n");
//					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   //ƥ���Ӷ���1��Ա"id"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.id,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //ƥ���Ӷ���1��Ա"name"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.name,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)//ƥ���Ӷ���1��Ա"country"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.country,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  //ƥ���Ӷ���1��Ա"path"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.path,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)//ƥ���Ӷ���1��Ա"timezone"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.timezone,item->valuestring,strlen(item->valuestring));
//					}
//					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)//ƥ���Ӷ���1��Ա"timezone_offset"
//					{
//						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
//						memcpy(results[0].location.timezone_offset,item->valuestring,strlen(item->valuestring));
//					}
//				}
				/* ƥ���Ӷ���2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					u2_printf("---------------------------------subobject2-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//ƥ���Ӷ���2��Ա"text"
					{
						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.text,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//ƥ���Ӷ���2��Ա"code"
					{
						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.code,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //ƥ���Ӷ���2��Ա"temperature"
					{
						u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.temperature,item->valuestring,strlen(item->valuestring));
					}
				}
				/* ƥ���Ӷ���3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					u2_printf("---------------------------------subobject3-------------------------------\r\n");
					u2_printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",subobject->type,subobject->string,subobject->valuestring);
					memcpy(results[0].last_update,subobject->valuestring,strlen(subobject->valuestring));
				}
			} 
		}
	}
	
	cJSON_Delete(json); //�ͷ�cJSON_Parse()����������ڴ�ռ�
	
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
//	USART1_Send_String("AT+CWMODE=1\r\n");   //��Ϊ������
//	Delay_ms(500);
//	USART1_Send_String("AT+CIPMUX=0\r\n");           //������ģʽ
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







