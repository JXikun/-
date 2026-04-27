#include "Motor.h"

/**
 * 直流电机驱动
 * 引脚: PB3 - 开关控制
 */

static uint8_t motor_state = 0;      // 电机状态: 0-关闭, 1-开启

/**
 * 函    数: 电机初始化
 * 参    数: 无
 * 返 回 值: 无
 */
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能GPIOB时钟 */
    RCC_APB2PeriphClockCmd(MOTOR_GPIO_CLK, ENABLE);

    /* GPIO配置为推挽输出 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = MOTOR_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_GPIO_PORT, &GPIO_InitStructure);

    /* 初始状态关闭（高电平停止） */
    GPIO_SetBits(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN);
    motor_state = 0;
}

/**
 * 函    数: 开启电机
 * 参    数: 无
 * 返 回 值: 无
 */
void Motor_On(void)
{
    motor_state = 1;
    GPIO_ResetBits(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN);  // 低电平启动
}

/**
 * 函    数: 关闭电机
 * 参    数: 无
 * 返 回 值: 无
 */
void Motor_Off(void)
{
    motor_state = 0;
    GPIO_SetBits(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN);   // 高电平停止
}

/**
 * 函    数: 获取当前电机状态
 * 参    数: 无
 * 返 回 值: 1-开启，0-关闭
 */
uint8_t Motor_GetState(void)
{
    return motor_state;
}