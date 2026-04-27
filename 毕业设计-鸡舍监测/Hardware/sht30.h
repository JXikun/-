#ifndef __SHT30_H
#define	__SHT30_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

/*****************消哥软硬件设计******************
											STM32
 * 项目			:	3.SHT30温度湿度传感器实验                     
 * 版本			: V1.0
 * 日期			: 2025.6.05
 * MCU			:	STM32F103C8T6
 * 接口			:	参看SHT30.h							
 * BILIBILI	:	消哥软硬件设计
 * CSDN			:	消哥软硬件设计
 * 作者			:	消哥 

**********************BEGIN***********************/


/***************根据自己需求更改****************/
// CSHT30 GPIO宏定义

#define		SHT30_IIC_CLK									RCC_APB2Periph_GPIOB
#define 	SHT30_IIC_PORT									GPIOB
#define 	SHT30_IIC_SDA_PIN								GPIO_Pin_7	
#define 	SHT30_IIC_SCL_PIN								GPIO_Pin_6	

//IO操作函数	 
#define  SHT30_IIC_SDA_H   			GPIO_SetBits(SHT30_IIC_PORT,SHT30_IIC_SDA_PIN)
#define  SHT30_IIC_SDA_L   			GPIO_ResetBits(SHT30_IIC_PORT,SHT30_IIC_SDA_PIN)
				 
#define  SHT30_IIC_SCL_H   			GPIO_SetBits(SHT30_IIC_PORT,SHT30_IIC_SCL_PIN)
#define  SHT30_IIC_SCL_L   			GPIO_ResetBits(SHT30_IIC_PORT,SHT30_IIC_SCL_PIN) 

#define SHT30_READ_SDA  				GPIO_ReadInputDataBit(SHT30_IIC_PORT, SHT30_IIC_SDA_PIN) 		//输入SDA 

/*********************END**********************/

#define SHT30_ADDR (uint8_t)(0x44<<1) //sht30 i2c地址，ADDR管脚接低电平时为0x44,接高电平为0x45

//SHT30命令
#define SHT30_READ_HUMITURE (uint16_t)0x2c06  //读温湿度

//CRC多项式
#define POLYNOMIAL 0x31 // X^8 + X^5 + X^4 + 1




//SHT30

void SHT30_Init(void);
void SHT30_IIC_SDA_IN(void);
void SHT30_IIC_SDA_OUT(void);
	

void SHT30_IIC_Init(void);
void SHT30_IIC_start(void); 
void SHT30_IIC_stop(void);
uint8_t SHT30_IIC_Get_ack(void);
void SHT30_IIC_ACK(void);
void SHT30_IIC_NACK(void);
void SHT30_IIC_write_byte(uint8_t Data);
uint8_t SHT30_IIC_read_byte(uint8_t ack);



/**********************************
函数声明
**********************************/
unsigned char SHT3X_CRC(uint8_t *data, uint8_t len);
void SHT30_CMD(uint16_t cmd);
u8 SHT30_Read_Humiture(u8 *temp,u8 *humi);


#endif



