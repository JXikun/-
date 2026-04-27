#include "mq135.h"

/*****************������Ӳ�����******************
											STM32
 * ��Ŀ			:10.mq135��������������ʵ��                     
 * �汾			:V1.0
 * ����			:2025.6.19
 * MCU			:STM32F103C8T6
 * �ӿ�			:�ο�mq135.h							
 * BILIBILI		:������Ӳ�����
 * CSDN			:������Ӳ�����
 * ����			:���� 

**********************BEGIN***********************/

void MQ135_Init(void)
{
	#if MODE
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (MQ135_AO_GPIO_CLK, ENABLE );	// �� ADC IO�˿�ʱ��
		GPIO_InitStructure.GPIO_Pin = MQ135_AO_GPIO_PIN;					// ���� ADC IO ����ģʽ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		// ����Ϊģ������
		
		GPIO_Init(MQ135_AO_GPIO_PORT, &GPIO_InitStructure);				// ��ʼ�� ADC IO

		ADCx_Init();
	}
	#else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (MQ135_DO_GPIO_CLK, ENABLE );	// ������ ������DO �ĵ�Ƭ�����Ŷ˿�ʱ��
		GPIO_InitStructure.GPIO_Pin = MQ135_DO_GPIO_PIN;			// �������� ������DO �ĵ�Ƭ������ģʽ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			// ����Ϊ��������
		
		GPIO_Init(MQ135_DO_GPIO_PORT, &GPIO_InitStructure);				// ��ʼ�� 
		
	}
	#endif
	
}

#if MODE
uint16_t MQ135_ADC_Read(void)
{
	//����ָ��ADC�Ĺ�����ͨ��������ʱ��
	return ADC_GetValue(ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}
#endif

uint16_t MQ135_GetData(void)
{
	
	#if MODE
	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < MQ135_READ_TIMES; i++)
	{
		tempData += MQ135_ADC_Read();
		delay_ms(5);
	}

	tempData /= MQ135_READ_TIMES;
	return tempData;
	
	#else
	uint16_t tempData;
	tempData = !GPIO_ReadInputDataBit(MQ135_DO_GPIO_PORT, MQ135_DO_GPIO_PIN);
	return tempData;
	#endif
}


float MQ135_GetData_PPM(void)
{
	#if MODE
	float  tempData = 0;
	uint16_t adcValue;

	for (uint8_t i = 0; i < MQ135_READ_TIMES; i++)
	{
		adcValue = MQ135_ADC_Read();
		tempData += adcValue;
		delay_ms(5);
	}
	tempData /= MQ135_READ_TIMES;

	// 计算电压 (V)
	float Vol = (tempData * 5.0f) / 4096.0f;

	// 计算传感器阻值 Rs
	float RS = (5.0f - Vol) / (Vol * 0.5f);

	// MQ135在清洁空气中的标定阻值R0 (需要根据实际环境校准)
	// 使用典型值 10K ohm (清洁空气)
	float R0 = 10.0f;

	// 计算ppm: ppm = 113.6 * (RS/R0)^-2.193
	float ppm = 113.6f * pow(RS / R0, -2.193f);

	// 限制范围
	if (ppm < 0) ppm = 0;
	if (ppm > 1000) ppm = 1000;

	return ppm;
	#endif
}
