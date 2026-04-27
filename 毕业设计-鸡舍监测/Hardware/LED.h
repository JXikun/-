#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

/*
 * LED驱动
 * LED1 (PA1): 补光LED
 * LED2 (PA2): 预留/PWM输出
 */

void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);
uint8_t LED1_GetState(void);
uint8_t LED2_GetState(void);

#endif