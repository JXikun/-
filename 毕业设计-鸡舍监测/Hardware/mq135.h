#ifndef __MQ135_H
#define	__MQ135_H
#include "stm32f10x.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"

/*****************Ӳ******************
											STM32
 * Ŀ			:10.mq135ʵ                     
 * 汾			:V1.0
 * 			:2025.6.19
 * MCU			:STM32F103C8T6
 * ӿ			:οmq135.h							
 * BILIBILI		:Ӳ
 * CSDN			:Ӳ
 * 			: 

**********************BEGIN***********************/

#define MQ135_READ_TIMES	10  //MQ-135ADCѭȡ

//ģʽѡ	
//ģAO:	1
//DO:	0
#define	MODE 	1

/***************Լ****************/
// MQ-135 GPIO궨
#if MODE
#define		MQ135_AO_GPIO_CLK								RCC_APB2Periph_GPIOB
#define 	MQ135_AO_GPIO_PORT							GPIOB
#define 	MQ135_AO_GPIO_PIN								GPIO_Pin_4
#define   ADC_CHANNEL               			ADC_Channel_4	// ADC ͨ궨

#else
#define		MQ135_DO_GPIO_CLK								RCC_APB2Periph_GPIOB
#define 	MQ135_DO_GPIO_PORT							GPIOB
#define 	MQ135_DO_GPIO_PIN								GPIO_Pin_5			

#endif
/*********************END**********************/


void MQ135_Init(void);
uint16_t MQ135_GetData(void);
float MQ135_GetData_PPM(void);

#endif /* __ADC_H */

