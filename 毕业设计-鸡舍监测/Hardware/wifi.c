#include "stm32f10x.h"
#include "Delay.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "wifi.h"
#include "LED.h"
#include "BEEP.h"
#include "Motor.h"

extern unsigned char RxCounter, RxBuffer[512];
char str[256];

/* 云端命令状态变量 */
uint8_t cloud_fan_state = 0;
uint8_t cloud_alarm_state = 0;
uint8_t cloud_led_state = 0;
uint8_t cloud_mode_state = 0;
uint8_t cloud_cmd_flag = 0;
uint8_t cloud_temp_max = 26;
uint16_t cloud_gas_max = 100;
uint16_t cloud_light_min = 150;


void AT_write(char atstring[1024])
{
    sprintf(str, "%s", atstring);
    Uart2_SendStr(str);
    delay_ms(200);
}

void Clear_Buffer(void)
{
    RxCounter = 0;
    memset(RxBuffer, 0, sizeof(RxBuffer));
}


void Wifi_Init()
{
    delay_ms(200);
    Clear_Buffer();

    AT_write("AT+RST\r\n");
    delay_ms(1500);
    if(strstr((const char*)RxBuffer, "ready") != NULL)
    {
        printf("\r\nWiFi Reset OK\r\n");
    }
    Clear_Buffer();

    AT_write("AT\r\n");
    while ((strstr(RxBuffer, "OK")) == NULL);
    Clear_Buffer();

    AT_write("AT+CWMODE=1\r\n");
    delay_ms(200);
    while ((strstr(RxBuffer, "OK")) == NULL);
    Clear_Buffer();

    sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_ACCOUNT, WIFI_PASSWORD);
    AT_write(str);
    delay_ms(200);
    while ((strstr(RxBuffer, "OK")) == NULL);
    while ((strstr(RxBuffer, "WIFI CONNECTED")) == NULL);
    while ((strstr(RxBuffer, "WIFI GOT IP")) == NULL);
    Clear_Buffer();
    printf("\r\nWIFI Connected\r\n");

    delay_ms(200);

    sprintf(str, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n", HUAIWEI_USERNANME, HUAIWEI_PASSWORD);
    AT_write(str);
    delay_ms(200);
    while ((strstr(RxBuffer, "OK")) == NULL);
    Clear_Buffer();

    sprintf(str, "AT+MQTTCLIENTID=0,\"%s\"\r\n", HUAIWEI_CLIENDID);
    AT_write(str);
    delay_ms(200);
    while ((strstr(RxBuffer, "OK")) == NULL);
    Clear_Buffer();

    sprintf(str, "AT+MQTTCONN=0,\"%s\",1883,0\r\n", HUAIWEI_HOSTNANME);
    AT_write(str);
    delay_ms(200);
    while ((strstr(RxBuffer, "OK")) == NULL);
    Clear_Buffer();
    printf("\r\nConnected to Huawei Cloud\r\n");

    sprintf(str, "AT+MQTTSUB=0,\"$oc/devices/%s/sys/properties/report\",1\r\n", HUAIWEI_DEVICEID);
    AT_write(str);
    while ((strstr(RxBuffer, "OK")) == NULL);
    Clear_Buffer();
    printf("\r\nSubscribed report topic\r\n");

    sprintf(str, "AT+MQTTSUB=0,\"$oc/devices/%s/sys/commands/#\",1\r\n", HUAIWEI_DEVICEID);
    AT_write(str);
    while ((strstr(RxBuffer, "OK")) == NULL);
    Clear_Buffer();
    printf("\r\nSubscribed command topic\r\n");
}


void HUAIWEI_PUBdata(char *DEVICE_ID,char*SERVICE_ID,char* OBJ,int16_t VALUE)
{
    sprintf(str,"AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"%s\\\"\\,\\\"properties\\\":{\\\"%s\\\":%d}}]}\",0,0\r\n",DEVICE_ID,SERVICE_ID,OBJ,VALUE);
    AT_write(str);
    while ((strstr(RxBuffer,"OK"))==NULL);
    Clear_Buffer();
}

void HUAIWEI_PUBstr(char *DEVICE_ID,char*SERVICE_ID,char* OBJ,char* string)
{
    sprintf(str,"AT+MQTTPUB=0,\"$oc/devices/%s/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"%s\\\"\\,\\\"properties\\\":{\\\"%s\\\":\\\"%s\\\"}}]}\",0,0\r\n",DEVICE_ID,SERVICE_ID,OBJ,string);
    AT_write(str);
    while ((strstr(RxBuffer,"OK"))==NULL);
    Clear_Buffer();
}

void CParsejson(char *data)
{
    char *req_start = strstr(data, "request_id=");
    char request_id[100];

    if (req_start)
    {
        req_start += strlen("request_id=");
        char *req_end = strchr(req_start, '"');
        if (req_end)
            strncpy(request_id, req_start, req_end - req_start);
    }
    if (request_id[0] == '\0')
    {
        printf("receive request_id fail\r\n");
        return;
    }
    request_id[36] = '\0';
    printf("%s", data);

    uint8_t cmd_processed = 0;

    // Fan control - command_name: "fan_set", paras: {"状态": "open"/"close"}
    if((strstr(data, "fan_set")) != NULL)
    {
        if((strstr(data, "open")) != NULL)
        {
            
            cloud_fan_state = 1;
            printf("Fan ON\r\n");
        }
        else if((strstr(data, "close")) != NULL)
        {
            
            cloud_fan_state = 0;
            printf("Fan OFF\r\n");
        }
        cmd_processed = 1;
    }

    // Alarm control - command_name: "alarm_set", paras: {"状态": "on"/"off"}
    if((strstr(data, "alarm_set"))!= NULL)
    {
        if((strstr(data, "open")) != NULL)
        {
           
            cloud_alarm_state = 1;
            printf("Alarm ON\r\n");
        }
        else if((strstr(data, "close")) != NULL)
        {
            
            cloud_alarm_state = 0;
            printf("Alarm OFF\r\n");
        }
        cmd_processed = 1;
    }

    // Light control - command_name: "led_set", paras: {"状态": "open"/"off"}
    if((strstr(data, "led_set"))!= NULL)
    {
        if((strstr(data, "open")) != NULL)
        {
            
            cloud_led_state = 1;
            printf("LED ON\r\n");
        }
        else if((strstr(data, "close")) != NULL)
        {
            
            cloud_led_state = 0;
            printf("LED OFF\r\n");
        }
        cmd_processed = 1;
    }

    // Mode control - command_name: "mode_set", paras: {"mode": "Auto"/"Manual"}
    if((strstr(data, "mode_set"))!= NULL)
    {
        if((strstr(data, "AC")) != NULL)
        {
            cloud_mode_state = 0;
            printf("Mode Auto\r\n");
        }
        else if((strstr(data, "MC")) != NULL)
        {
            cloud_mode_state = 1;
            printf("Mode Manual\r\n");
        }
        cmd_processed = 1;
    }

    // Temp threshold control - command_name: "temp_max_set"
    if((strstr(data, "temp_set")) != NULL)
    {
        char *val_ptr = strstr(data, "\"value\":");
        if(val_ptr != NULL)
        {
            val_ptr += 8;
            cloud_temp_max = atoi(val_ptr);
            printf("Temp Max: %d\r\n", cloud_temp_max);
            cmd_processed = 1;
        }
    }

    // Gas threshold control - command_name: "gas_max_set"
    if((strstr(data, "gas_set")) != NULL)
    {
        char *val_ptr = strstr(data, "\"value\":");
        if(val_ptr != NULL)
        {
            val_ptr += 8;
            cloud_gas_max = atoi(val_ptr);
            printf("Gas Max: %d\r\n", cloud_gas_max);
            cmd_processed = 1;
        }
    }

    // Light threshold control - command_name: "light_min_set"
    if((strstr(data, "light_set")) != NULL)
    {
        char *val_ptr = strstr(data, "\"value\":");
        if(val_ptr != NULL)
        {
            val_ptr += 8;
            cloud_light_min = atoi(val_ptr);
            printf("Light Min: %d\r\n", cloud_light_min);
            cmd_processed = 1;
        }
    }

    // 如果有命令被处理，设置标志
    if(cmd_processed)
    {
        cloud_cmd_flag = 1;
    }

    printf("%s\r\n", request_id);
    sprintf(str,"AT+MQTTPUB=0,\"$oc/devices/%s/sys/commands/response/request_id=%s\",\"\",0,0\r\n",HUAIWEI_DEVICEID,request_id);
    AT_write(str);
}


void HUAIWEI_RECEVICEcmd()
{
    uint8_t recdata[256];
    delay_ms(200);
    if ((strstr(RxBuffer, "+MQTTSUBRECV:")) != NULL)
    {
        if ((strstr(RxBuffer, "\r\n")) != NULL)
        {
            memcpy(recdata, RxBuffer, RxCounter);
            printf("%s\r\n", recdata);
            CParsejson(recdata);
        }
        memset(recdata, 0, 256);
    }
    Clear_Buffer();
}