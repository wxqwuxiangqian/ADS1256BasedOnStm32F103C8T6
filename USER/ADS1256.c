#include <stdio.h>
#include "ADS1256.h"
#include "stm32f10x_spi.h"
#include "delay.h"
#include "Usart.h"	


/*�˿ڶ���*/ 
#define RCC_DRDY			RCC_APB2Periph_GPIOB
#define PORT_DRDY			GPIOB
#define PIN_DRDY			GPIO_Pin_11

#define PORT_CS				GPIOB
#define PIN_CS				GPIO_Pin_12

#define CS_0()				GPIO_ResetBits(PORT_CS, PIN_CS);
#define CS_1()				GPIO_SetBits(PORT_CS, PIN_CS);
#define ADS1256_DRDY  		(PORT_DRDY->IDR & PIN_DRDY)


void SPI2_Init(void)
{
 SPI_InitTypeDef  SPI_InitStructure;
 GPIO_InitTypeDef GPIO_InitStructure;
 /****Initial SPI2******************/
 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
 /* NSS, SCK, MISO and MOSI */
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 
  /* SPI2 configuration */ 
 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI2����Ϊ����ȫ˫��
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                    //����SPI2Ϊ��ģʽ
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI���ͽ���8λ֡�ṹ
 SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                   //����ʱ���ڲ�����ʱ��ʱ��Ϊ�͵�ƽ
 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                 //��һ��ʱ���ؿ�ʼ��������
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                  //NSS�ź��������ʹ��SSIλ������
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ8
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;       //���ݴ����MSBλ��ʼ
 SPI_InitStructure.SPI_CRCPolynomial = 7;         //CRCֵ����Ķ���ʽ
 SPI_Init(SPI2, &SPI_InitStructure);
 /* Enable SPI2  */
 SPI_Cmd(SPI2, ENABLE);  
}  

//��ʼ��ADS1256 GPIO
void Init_ADS1256_GPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd(RCC_DRDY, ENABLE); 

	  GPIO_InitStructure.GPIO_Pin = PIN_DRDY; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(PORT_DRDY, &GPIO_InitStructure);
	 //SPI2 NSS 
 	  CS_1();
    GPIO_InitStructure.GPIO_Pin = PIN_CS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PORT_CS, &GPIO_InitStructure);
  	SPI2_Init();
}


//-----------------------------------------------------------------//
//	��    �ܣ�  SPIͨ��
//	��ڲ���: /	���͵�SPI����
//	���ڲ���: /	���յ�SPI����
//	ȫ�ֱ���: /
//	��    ע: 	���ͽ��պ���
//-----------------------------------------------------------------//
unsigned char SPI_WriteByte(unsigned char TxData)
{

  unsigned char RxData=0;
  while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
  SPI_I2S_SendData(SPI2,TxData);
	
	
  while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);
	
  RxData=SPI_I2S_ReceiveData(SPI2);
	printf("%x,%x\n",TxData,RxData);

  return RxData;

} 

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1256 д����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1220�е�ַΪregaddr�ļĴ���д��һ���ֽ�databyte
//-----------------------------------------------------------------//
void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
	CS_0();
	while(ADS1256_DRDY);//��ADS1256_DRDYΪ��ʱ����д�Ĵ���
	//��Ĵ���д�����ݵ�ַ
    SPI_WriteByte(ADS1256_CMD_WREG | (regaddr & 0x0F));
    //д�����ݵĸ���n-1
    SPI_WriteByte(0x00);
    //��regaddr��ַָ��ļĴ���д������databyte
    SPI_WriteByte(databyte);
	CS_1();
}


//��ʼ��ADS1256
void ADS1256_Init(void)
{
	
	Init_ADS1256_GPIO();//��ʼ��ADS1256 GPIO�ܽ� 

	
	//*************��У׼****************
  while(ADS1256_DRDY);
	CS_0();
	SPI_WriteByte(ADS1256_CMD_SELFCAL);
	while(ADS1256_DRDY);
	CS_1();

	//**********************************

	ADS1256WREG(ADS1256_STATUS,0x06);               // ��λ��ǰ��ʹ�û���
//	ADS1256WREG(ADS1256_STATUS,0x04);               // ��λ��ǰ����ʹ�û���

	//	ADS1256WREG(ADS1256_MUX,0x08);                  // ��ʼ���˿�A0Ϊ��+����AINCOMλ��-��
	ADS1256WREG(ADS1256_ADCON,ADS1256_GAIN_1);                // �Ŵ���1
	ADS1256WREG(ADS1256_DRATE,ADS1256_DRATE_30000SPS);  // ����10sps
	ADS1256WREG(ADS1256_IO,0x00); 


	//*************��У׼****************
	while(ADS1256_DRDY);
	CS_0();
	SPI_WriteByte(ADS1256_CMD_SELFCAL);
	while(ADS1256_DRDY);
	CS_1(); 
	//**********************************
}

//��ȡADֵ
signed int ADS1256ReadData(unsigned char channel)  
{

  unsigned int sum=0;
	
	while(ADS1256_DRDY);//��ADS1256_DRDYΪ��ʱ����д�Ĵ��� 
	ADS1256WREG(ADS1256_MUX,channel);		//����ͨ��
	CS_0();
	SPI_WriteByte(ADS1256_CMD_SYNC);
	SPI_WriteByte(ADS1256_CMD_WAKEUP);	               
	SPI_WriteByte(ADS1256_CMD_RDATA);
	  delay_ms(1);/////////////////////////////////!!!!!!!
  sum |= (SPI_WriteByte(0xff) << 16);
	sum |= (SPI_WriteByte(0xff) << 8);
	sum |= SPI_WriteByte(0xff);
	CS_1();


	if (sum>0x7FFFFF)           // if MSB=1, 
	{
		sum -= 0x1000000;       // do 2's complement

	}
    return sum;
}



