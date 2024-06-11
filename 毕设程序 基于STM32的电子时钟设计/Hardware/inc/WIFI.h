#ifndef __WIFI_H
#define __WIFI_H
#include "stm32f10x.h"

extern u8 WIFI_connect_succeed;

//子对象1结构体--Location
typedef struct
{
	char id[32];
	char name[32];
	char country[32];
	char path[64];
	char timezone[32];
	char timezone_offset[32];
}Location;
 
//子对象2结构体--Now
typedef struct
{
	char text[32];
	char code[32];
	char temperature[32];
}Now;

typedef struct
{
	Location location;		//子对象1
	Now now;				//子对象2
	char last_update[64];	//子对象3
}Results;

extern Results results[] ;       //定义个结构体数组

u8 esp8266_start_trans(void);

u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* esp8266_check_cmd(u8 *str);
u8 atk_8266_quit_trans(void);

u8 get_current_weather(void);
int cJSON_WeatherParse(char *JSON, Results *results);

int cJSON_time(char *JSON);

void acquire_weather_message();
void acquire_time_message();

#endif
