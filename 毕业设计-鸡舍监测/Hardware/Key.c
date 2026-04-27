#include "Key.h"
#include "Delay.h"

/**
 * 按键初始化
 * 按键1: PA7 - 页面切换
 * 按键2: PB0 - 模式设置
 * 按键3: PB1 - 风扇设置
 * 按键4: PB10 - 一键报警
 * 按键5: PB11 - 开关灯
 */
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 开启GPIOB时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* 开启GPIOA时钟 (PA7) */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* 所有按键配置为上拉输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* 按键1 PB1 */
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

    /* 按键2 PB10 */
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);

    /* 按键3 PB11 */
    GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);

    /* 按键4 PB0 */
    GPIO_InitStructure.GPIO_Pin = KEY4_GPIO_PIN;
    GPIO_Init(KEY4_GPIO_PORT, &GPIO_InitStructure);

    /* 按键5 PA7 */
    GPIO_InitStructure.GPIO_Pin = KEY5_GPIO_PIN;
    GPIO_Init(KEY5_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * 获取按键键码
 * 返回值: 1-页面切换, 2-模式设置, 3-风扇设置, 4-一键报警, 5-开关灯, 0-无
 */
uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0;

    if (GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == 0)
    {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == 0);
        delay_ms(20);
        KeyNum = 1;
    }

    if (GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == 0)
    {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == 0);
        delay_ms(20);
        KeyNum = 2;
    }

    if (GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == 0)
    {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == 0);
        delay_ms(20);
        KeyNum = 3;
    }

    if (GPIO_ReadInputDataBit(KEY4_GPIO_PORT, KEY4_GPIO_PIN) == 0)
    {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(KEY4_GPIO_PORT, KEY4_GPIO_PIN) == 0);
        delay_ms(20);
        KeyNum = 4;
    }

    if (GPIO_ReadInputDataBit(KEY5_GPIO_PORT, KEY5_GPIO_PIN) == 0)
    {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(KEY5_GPIO_PORT, KEY5_GPIO_PIN) == 0);
        delay_ms(20);
        KeyNum = 5;
    }

    return KeyNum;
}