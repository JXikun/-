#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"
#include <stdint.h>

/**
 * 直流电机驱动
 * 引脚: PB3 - 开关控制
 */

/* GPIO配置 */
#define MOTOR_GPIO_PORT        GPIOB
#define MOTOR_GPIO_PIN        GPIO_Pin_3
#define MOTOR_GPIO_CLK        RCC_APB2Periph_GPIOB

/**
 * 函    数: 电机初始化
 * 参    数: 无
 * 返 回 值: 无
 * 说    明: 初始化PB3为推挽输出，电机默认关闭
 */
void Motor_Init(void);

/**
 * 函    数: 开启电机
 * 参    数: 无
 * 返 回 值: 无
 * 说    明: PB3输出高电平
 */
void Motor_On(void);

/**
 * 函    数: 关闭电机
 * 参    数: 无
 * 返 回 值: 无
 * 说    明: PB3输出低电平
 */
void Motor_Off(void);

/**
 * 函    数: 获取当前电机状态
 * 参    数: 无
 * 返 回 值: 1-开启，0-关闭
 */
uint8_t Motor_GetState(void);

#endif