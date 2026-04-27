#ifndef _WIFI_H
#define _WIFI_H

/* WiFi账号密码 */
#define WIFI_ACCOUNT "Redmi"
#define WIFI_PASSWORD "wb200313"

/* 华为云IoT平台配置 */
#define HUAIWEI_USERNANME "69e32016c00ccb6d4b4cf17f_esp8266"
#define HUAIWEI_DEVICEID "69e32016c00ccb6d4b4cf17f_esp8266"
#define HUAIWEI_PASSWORD "a5176e7344f080f479bfcc4c0ca704c1e24fa0493839fa2915481574ab5d569c"
#define HUAIWEI_HOSTNANME "b96072ab5b.st1.iotda-device.cn-south-1.myhuaweicloud.com"
#define HUAIWEI_CLIENDID "69e32016c00ccb6d4b4cf17f_esp8266_0_0_2026041809"
#define HUAIWEI_SERVICE "display"

/* 物模型属性名称 */
#define OBJ_TEM "temperature"
#define OBJ_HUM "humidity"
#define OBJ_GAS "gas"
#define OBJ_LIGHT "light"
#define OBJ_BEEP "alarm"
#define OBJ_LED "led"
#define OBJ_FAN "fan"
#define OBJ_MODE "mode"
#define OBJ_TEMP_MAX "temp_max"
#define OBJ_GAS_MAX "gas_max"
#define OBJ_LIGHT_MIN "light_min"

#define ON "ON"
#define OFF "OFF"
#define MANUAL "Manual"
#define AUTO "Auto"

/* 云端命令状态变量（供外部访问） */
extern uint8_t cloud_fan_state;
extern uint8_t cloud_alarm_state;
extern uint8_t cloud_led_state;
extern uint8_t cloud_mode_state;
extern uint8_t cloud_cmd_flag;  // 云端命令标志
extern uint8_t cloud_temp_max;  // 温度上限
extern uint16_t cloud_gas_max;   // 气体上限
extern uint16_t cloud_light_min; // 光照下限

void Wifi_Init();
void HUAIWEI_PUBdata(char *DEVICE_ID, char *SERVICE_ID, char *OBJ, int16_t VALUE);
void HUAIWEI_PUBstr(char *DEVICE_ID, char *SERVICE_ID, char *OBJ, char *string);
void HUAIWEI_RECEVICEcmd();

#endif