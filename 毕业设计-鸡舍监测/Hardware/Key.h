#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY1_GPIO_PORT     GPIOA
#define KEY1_GPIO_PIN     GPIO_Pin_7

#define KEY2_GPIO_PORT     GPIOB
#define KEY2_GPIO_PIN     GPIO_Pin_0

#define KEY3_GPIO_PORT     GPIOB
#define KEY3_GPIO_PIN     GPIO_Pin_1

#define KEY4_GPIO_PORT     GPIOB
#define KEY4_GPIO_PIN     GPIO_Pin_10

#define KEY5_GPIO_PORT     GPIOB
#define KEY5_GPIO_PIN     GPIO_Pin_11

void Key_Init(void);
uint8_t Key_GetNum(void);

#endif