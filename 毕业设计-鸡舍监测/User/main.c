#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "SHT30.h"
#include "adcx.h"
#include "mq135.h"
#include "BH1750.h"
#include "wifi.h"
#include "usart.h"
#include "Key.h"
#include "beep.h"
#include "led.h"
#include "motor.h"

uint8_t temperature = 0;
uint8_t humidity = 0;
uint16_t light = 0;
float mq135_ppm = 0;
uint8_t key_num = 0;

/* 按键状态变量 */
uint8_t current_page = 0;       // 当前页面 (0-数据页面, 1-设置页面, 2-阈值页面)
uint8_t system_mode = 0;        // 系统模式 (0-自动模式, 1-手动模式)
int8_t fan_speed = 0;           // 风扇速度 (-100 ~ 100)
uint8_t alarm_flag = 0;         // 报警标志
uint8_t led2_state = 0;         // LED2灯状态 (0-关, 1-开)
uint8_t fan_state = 0;          // 风扇状态 (0-关, 1-开)
uint16_t report_count = 0;      // 上报计数器
uint8_t state_changed = 0;      // 设备状态变化标志
uint8_t alarm_blink = 0;        // 报警闪烁状态 (0-响, 1-闪)

/* 云端命令状态（来自wifi.c） */
extern uint8_t cloud_fan_state;
extern uint8_t cloud_alarm_state;
extern uint8_t cloud_led_state;
extern uint8_t cloud_mode_state;
extern uint8_t cloud_cmd_flag;
extern uint8_t cloud_temp_max;
extern uint16_t cloud_gas_max;
extern uint16_t cloud_light_min;

/* 阈值参数 */
uint8_t temp_max = 35;         // 温度阈值上限 (℃)
uint16_t gas_max = 100;        // 有害气体阈值上限 (ppm)
uint16_t light_min = 100;       // 光照阈值下限 (lx)

int main(void)
{
    /* 系统初始化 */
    SystemInit();

    /* 串口1初始化 (调试) */
    uart_init(115200);
    uart2_init(115200);

    /* OLED初始化 */
    OLED_Init();
    OLED_ShowString(2, 2, "Wait Starting");

    /* WiFi和华为云初始化 */
    Wifi_Init();
    OLED_Clear();
    OLED_ShowString(1, 1, "Temp:   C");
    OLED_ShowString(2, 1, "Humi:   %");
    OLED_ShowString(3, 1, "Gas:  ppm");
    OLED_ShowString(4, 1, "Light:   lx");

    /* 初始化SHT30温湿度传感器 */
    SHT30_Init();

    /* 初始化MQ-135气体传感器 */
    MQ135_Init();
    ADCx_Init();
    
    /* 初始化BH1750光照传感器 */
    BH1750_Init();
    BEEP_Init();
    LED_Init();

    /* 禁用JTAG调试，释放PB3等引脚 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    Motor_Init();
    /* 按键初始化 */
    Key_Init();

 
    delay_ms(500);  // 等待传感器稳定

    /* 上报初始设备状态到华为云 */
    HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_MODE, system_mode == 0 ? AUTO : MANUAL);
    delay_ms(50);
    HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_FAN, fan_state == 1 ? ON : OFF);
    delay_ms(50);
    HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_LED, led2_state == 1 ? ON : OFF);
    delay_ms(50);
    HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_BEEP, alarm_flag == 1 ? ON : OFF);
    delay_ms(50);
    HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_TEMP_MAX, temp_max);
    delay_ms(50);
    HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_GAS_MAX, gas_max);
    delay_ms(50);
    HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_LIGHT_MIN, light_min);
    delay_ms(50);

    while (1)
    {
        key_num = Key_GetNum();

        switch(key_num)
        {
            case 1:
                current_page++;
                if(current_page > 2) current_page = 0;
                if(current_page == 0)
                {
                    OLED_Clear();
                    OLED_ShowString(1, 1, "Temp:   C");
                    OLED_ShowString(2, 1, "Humi:   %");
                    OLED_ShowString(3, 1, "Gas:  ppm");
                    OLED_ShowString(4, 1, "Light:   lx");
                }
                else if(current_page == 1)
                {
                    OLED_Clear();
                    OLED_ShowString(1, 1, "Mode:        ");
                    OLED_ShowString(2, 1, "Fan:         ");
                    OLED_ShowString(3, 1, "Alarm:       ");
                    OLED_ShowString(4, 1, "LED:         ");
                }
                else
                {
                    OLED_Clear();
                    OLED_ShowString(1, 1, "Mod:");
                    OLED_ShowString(2, 1, "tHi:");
                    OLED_ShowString(3, 1, "gHi:");
                    OLED_ShowString(4, 1, "lLo:");
                    OLED_ShowString(1, 5, system_mode == 0 ? "Auto" : "Manu");
                    OLED_ShowNum(2, 5, temp_max, 3);
                    OLED_ShowNum(3, 5, gas_max, 3);
                    OLED_ShowNum(4, 5, light_min, 3);
                }
                break;

            case 2:
                if(current_page == 1)
                {
                    system_mode++;
                    if(system_mode > 1) system_mode = 0;
                    OLED_ShowString(1, 6, system_mode == 0 ? "Auto " : "Manu ");
                    state_changed = 1;
                }
                break;

            case 3:
                if(current_page == 1)
                {
                    fan_state = !fan_state;
                    if(fan_state)
                    {
                        Motor_On();
                        OLED_ShowString(2, 5, "ON ");
                    }
                    else
                    {
                        Motor_Off();
                        OLED_ShowString(2, 5, "OFF");
                    }
                    state_changed = 1;
                }
                break;

            case 4:
                alarm_flag = !alarm_flag;
                if(alarm_flag == 0)
                {
                    BEEP_OFF();
                    LED1_OFF();
                    OLED_ShowString(3, 7, "OFF");
                }
                state_changed = 1;
                break;

            case 5:
                if(current_page == 1)
                {
                    led2_state = !led2_state;
                    if(led2_state)
                        LED2_ON();
                    else
                        LED2_OFF();
                    OLED_ShowString(4, 5, led2_state ? "ON " : "OFF");
                    state_changed = 1;
                }
                break;
        }

        if(current_page == 0)
        {
            SHT30_Read_Humiture(&temperature, &humidity);
            OLED_ShowNum(1, 6, temperature, 3);
            OLED_ShowNum(2, 6, humidity, 3);

            mq135_ppm = MQ135_GetData_PPM();
            OLED_ShowNum(3, 5, (uint16_t)mq135_ppm, 3);

            light = bh_data_read();
            OLED_ShowNum(4, 7, light, 4);
        }
        else if(current_page == 1)
        {
            OLED_ShowString(1, 6, system_mode == 0 ? "Auto " : "Manu ");
            OLED_ShowString(2, 5, fan_state ? "ON " : "OFF");
            OLED_ShowString(3, 7, alarm_flag ? "ON " : "OFF");
            OLED_ShowString(4, 5, led2_state ? "ON " : "OFF");
        }

        if(state_changed)
        {
            state_changed = 0;
            HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_MODE, system_mode == 0 ? AUTO : MANUAL);
            HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_FAN, fan_state == 1 ? ON : OFF);
            HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_LED, led2_state == 1 ? ON : OFF);
            HUAIWEI_PUBstr(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_BEEP, alarm_flag == 1 ? ON : OFF);
            HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_TEMP_MAX, temp_max);
            HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_GAS_MAX, gas_max);
            HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_LIGHT_MIN, light_min);
        }

        report_count++;
        if(report_count >= 10)
        {
            report_count = 0;

            HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_TEM, temperature);
            delay_ms(50);
            HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_HUM, humidity);
            delay_ms(50);
            HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_GAS, (int16_t)mq135_ppm);
            delay_ms(50);
            HUAIWEI_PUBdata(HUAIWEI_DEVICEID, HUAIWEI_SERVICE, OBJ_LIGHT, light);
        }

        if(alarm_flag)
        {
            alarm_blink++;
            if(alarm_blink >= 3)
            {
                alarm_blink = 0;
                BEEP_ON();
                LED1_ON();
            }
            else if(alarm_blink == 1)
            {
                BEEP_OFF();
                LED1_OFF();
            }
        }

        HUAIWEI_RECEVICEcmd();

        /* 处理云端下发的命令 */
        if(cloud_cmd_flag)
        {
            cloud_cmd_flag = 0;

            /* 同步云端命令状态到本地 */
            if(fan_state != cloud_fan_state)
            {
                fan_state = cloud_fan_state;
                if(fan_state)
                    Motor_On();
                else
                    Motor_Off();
                state_changed = 1;
            }

            if(alarm_flag != cloud_alarm_state)
            {
                alarm_flag = cloud_alarm_state;
                if(alarm_flag == 0)
                {
                    BEEP_OFF();
                    LED1_OFF();
                }
                state_changed = 1;
            }

            if(led2_state != cloud_led_state)
            {
                led2_state = cloud_led_state;
                if(led2_state)
                    LED2_ON();
                else
                    LED2_OFF();
                state_changed = 1;
            }

            if(system_mode != cloud_mode_state)
            {
                system_mode = cloud_mode_state;
                state_changed = 1;
            }

            /* 同步阈值参数 */
            if(temp_max != cloud_temp_max)
            {
                temp_max = cloud_temp_max;
                state_changed = 1;
            }
            if(gas_max != cloud_gas_max)
            {
                gas_max = cloud_gas_max;
                state_changed = 1;
            }
            if(light_min != cloud_light_min)
            {
                light_min = cloud_light_min;
                state_changed = 1;
            }

            /* 更新设置页面显示 */
            if(current_page == 1)
            {
                OLED_ShowString(1, 6, system_mode == 0 ? "Auto " : "Manu ");
                OLED_ShowString(2, 5, fan_state ? "ON " : "OFF");
                OLED_ShowString(3, 7, alarm_flag ? "ON " : "OFF");
                OLED_ShowString(4, 5, led2_state ? "ON " : "OFF");
            }

            /* 更新阈值页面显示 */
            if(current_page == 2)
            {
                OLED_ShowString(1, 5, system_mode == 0 ? "Auto" : "Manu");
                OLED_ShowNum(2, 5, temp_max, 3);
                OLED_ShowNum(3, 5, gas_max, 3);
                OLED_ShowNum(4, 5, light_min, 3);
            }
        }

        /* 自动模式控制逻辑 */
        if(system_mode == 0)
        {
            uint8_t auto_alarm = 0;

            /* 温度超限检测 */
            if(temperature > temp_max)
            {
                auto_alarm = 1;
                if(alarm_flag == 0)
                {
                    alarm_flag = 1;
                    state_changed = 1;
                }
                if(fan_state == 0)
                {
                    fan_state = 1;
                    Motor_On();
                    state_changed = 1;
                }
            }

            /* 气体浓度超限检测 */
            if((uint16_t)mq135_ppm > gas_max)
            {
                auto_alarm = 1;
                if(alarm_flag == 0)
                {
                    alarm_flag = 1;
                    state_changed = 1;
                }
                if(fan_state == 0)
                {
                    fan_state = 1;
                    Motor_On();
                    state_changed = 1;
                }
            }

            /* 光照低于下限检测 */
            if(light < light_min)
            {
                if(led2_state == 0)
                {
                    led2_state = 1;
                    LED2_ON();
                    state_changed = 1;
                }
            }

            /* 参数恢复正常 */
            if(temperature <= temp_max && (uint16_t)mq135_ppm <= gas_max)
            {
                /* 只有在自动模式下才自动关闭报警 */
                if(alarm_flag == 1)
                {
                    alarm_flag = 0;
                    state_changed = 1;
                }
            }

            if(light >= light_min)
            {
                if(led2_state == 1)
                {
                    led2_state = 0;
                    LED2_OFF();
                    state_changed = 1;
                }
            }

            /* 自动模式下也控制风扇关闭（当无报警时） */
            if(alarm_flag == 0 && fan_state == 1)
            {
                fan_state = 0;
                Motor_Off();
                state_changed = 1;
            }
        }

        delay_ms(200);
    }
}