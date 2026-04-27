#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "wifi.h"
#define RCC_BEEP RCC_APB2Periph_GPIOA
#define BEEP_GPIO  GPIOA
#define BEEP_PIN   GPIO_Pin_5

/**
  * ��    ������������ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void BEEP_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_BEEP, ENABLE);		//����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);						
	
	/*����GPIO��ʼ�����Ĭ�ϵ�ƽ*/
	GPIO_SetBits(BEEP_GPIO, BEEP_PIN );		
	
}

/**
  * ��    ��������������
  * ��    ������
  * �� �� ֵ����
  */
void BEEP_ON(void)
{
	GPIO_ResetBits(BEEP_GPIO, BEEP_PIN);		//����PA1����Ϊ�͵�ƽ
	
	
}

/**
  * ��    �����������ر�
  * ��    ������
  * �� �� ֵ����
  */
void BEEP_OFF(void)
{
	GPIO_SetBits(BEEP_GPIO, BEEP_PIN);		//����PA1����Ϊ�ߵ�ƽ
	
	
}

/**
  * ��    ����������״̬��ת
  * ��    ������
  * �� �� ֵ����
  */
void BEEP_Turn(void)
{
	if (GPIO_ReadOutputDataBit(BEEP_GPIO, BEEP_PIN) == 0)		//��ȡ����Ĵ�����״̬�������ǰ��������͵�ƽ
	{
		GPIO_SetBits(BEEP_GPIO, BEEP_PIN);					//������PA1����Ϊ�ߵ�ƽ
      
		
	}
	else													//���򣬼���ǰ��������ߵ�ƽ
	{
		GPIO_ResetBits(BEEP_GPIO, BEEP_PIN);					//������PA1����Ϊ�͵�ƽ
        
		
	}
}

