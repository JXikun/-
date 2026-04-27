#include "BH1750.h"
#include "delay.h"

/*****************絥Ƭ******************
											STM32
 * ļ			:	BH1750ǿȴcļ                   
 * 汾			: V1.0
 * 			: 2024.8.5
 * MCU			:	STM32F103C8T6
 * ӿ			:	οbh1750.h							
 * BILIBILI	:	絥Ƭ
 * CSDN			:	絥Ƭ
 * 			:	
 * 		: 췽
 * Ƶ	:	https://www.bilibili.com/video/BV14T421678Q/?share_source=copy_web&vd_source=097fdeaf6b6ecfed8a9ff7119c32faf2
 * ٷվ	:	www.yfcdz.cn

**********************BEGIN***********************/

typedef   unsigned char BYTE;
//BYTE    BUF[8];                         //ݻ   
//iicӿڳʼ

/**
**  SDAΪ
**/
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= BH1750_SDA_GPIO_PIN;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(BH1750_GPIO_PORT, &GPIO_InitStructer);
}


/**
**  SDAΪ
**/
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= BH1750_SDA_GPIO_PIN;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(BH1750_GPIO_PORT, &GPIO_InitStructer);
}


void Single_Write_BH1750(uchar REG_Address)
{
   BH1750_IIC_Start();                  //ʼź
   BH1750_IIC_Send_Byte(BHAddWrite);   //豸ַ+дź
   BH1750_IIC_Send_Byte(REG_Address);    //ڲĴַ
  //  BH1750_SendByte(REG_data); 	//ڲĴݣ
   BH1750_IIC_Stop();                   //ֹͣź
}

void BH1750_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BH1750_GPIO_CLK, ENABLE);	 //ʹA˿ʱ
	GPIO_InitStructure.GPIO_Pin = BH1750_SCL_GPIO_PIN|BH1750_SDA_GPIO_PIN;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ٶ50MHz
 	GPIO_Init(BH1750_GPIO_PORT, &GPIO_InitStructure);	  //ʼGPIOD3,6
 	GPIO_SetBits(BH1750_GPIO_PORT,BH1750_SCL_GPIO_PIN|BH1750_SDA_GPIO_PIN);	

	//Single_Write_BH1750(0x01);
	bh_data_send(BHPowOn);  //BH1750ϵ
	bh_data_send(BHReset);	//BH1750λ
	bh_data_send(BHModeH2); //BH1750дģʽѡǷֱ4lxʱ16ms
}

//IICʼź
void BH1750_IIC_Start(void)
{
	SDA_OUT();     //sda
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2Cߣ׼ͻ 
}	  
//IICֹͣź
void BH1750_IIC_Stop(void)
{
	SDA_OUT();//sda
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//I2C߽ź
	delay_us(4);							   	
}
//ȴӦźŵ
//ֵ1Ӧʧ
//        0Ӧɹ
u8 BH1750_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDAΪ  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			BH1750_IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ0 	   
	return 0;  
} 
//ACKӦ
void BH1750_IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//ACKӦ		    
void BH1750_IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IICһֽ
//شӻӦ
//1Ӧ
//0Ӧ			  
void BH1750_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//ʱӿʼݴ
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //TEA5767ʱǱ
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//1ֽڣack=1ʱACKack=0nACK   
u8 BH1750_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDAΪ
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        BH1750_IIC_NAck();//nACK
    else
        BH1750_IIC_Ack(); //ACK   
    return receive;
}

/*************************************************************************************/
void bh_data_send(u8 command)
{
    do{
    BH1750_IIC_Start();                      //iicʼź
    BH1750_IIC_Send_Byte(BHAddWrite);       //ַ
    }while(BH1750_IIC_Wait_Ack());           //ȴӻӦ
    BH1750_IIC_Send_Byte(command);          //ָ
    BH1750_IIC_Wait_Ack();                   //ȴӻӦ
    BH1750_IIC_Stop();                       //iicֹͣź
}

u16 bh_data_read(void)
{
	u16 buf;
	BH1750_IIC_Start();                       //iicʼź
	BH1750_IIC_Send_Byte(BHAddRead);         //ַ+־λ
	BH1750_IIC_Wait_Ack();                     //ȴӻӦ
	buf=BH1750_IIC_Read_Byte(1);              //ȡ
	buf=buf<<8;                        //ȡ߰λ
	buf+=0x00ff&BH1750_IIC_Read_Byte(0);      //ȡڰλ
	BH1750_IIC_Stop();                        //ֹͣź 
	return buf; 
}


