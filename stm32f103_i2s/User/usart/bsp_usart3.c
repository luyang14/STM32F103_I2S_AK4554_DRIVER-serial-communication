/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   usartӦ��bsp
  ******************************************************************************
  */
  
#include "bsp_usart3.h"
#include "delay.h"
#include "string.h"

char WIFI_USART_RX_BUFF[LEN_WIFI_USART_RX];
u16 WIFI_USART_RX_BUFF_p=0;
u16 IP_RX_BUFF_p=0;
char IP_RX_BUFF[LEN_IP_USART_RX];
u8 Air_RX[10];

 /**
  * @brief  USART3 GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
void USART3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	/* USART3 GPIO config */
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* USART3 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	/* ʹ�ܴ���3�����ж� */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
	// ����USART3�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/********************************
 * ��������UART3_Put_Str
 * ����  ����ʼ������3
 * ����  ����
 * ���  ����
 *********************************/
void UART3_Put_Str(char *s)
{
	u16 i=0;
	while(s[i]!='\0')
	{
		//��Ҫ���͵��ֽ�д��UART3�ķ��ͻ�����
		USART_SendData(USART3, s[i]);
		
		#ifdef USART3_Remap_USART1
				USART1->DR = (s[i] & (uint16_t)0x01FF);
		#endif
		
		//�ȴ��������
		while (!(USART3->SR & USART_FLAG_TXE));
		i++;
	}
}
//uart3�����ַ�
void UART3_Put_Char(char s)
{
	//��Ҫ���͵��ֽ�д��UART3�ķ��ͻ�����
	USART_SendData(USART3, s);
	//�ȴ��������
	while (!(USART3->SR & USART_FLAG_TXE));
}
//��ȡGPRSģ�鷵�ص��ַ����������͵�һ�������Ƚϣ���ʱ��λ��10mS,1S=100;
uint8_t WIFI_ResponseCheck(char *s,u16 Deley_1S)
{
	u16 i;
	
	//��ս��ջ���ָ��
	WIFI_USART_RX_BUFF_p=0;
//	IP_RX_BUFF_p=0;
	
	for(i=0;i<Deley_1S*10;i++)
	{
		/*�Ƚ�����ʱ */
		delay_ms(100);
			
		/*��ȡ�ַ�������ֹ�ɵĻ��汻����*/
		WIFI_USART_RX_BUFF[WIFI_USART_RX_BUFF_p+1]=0;
		
		//�Ƚ��ַ���
		if(strstr(WIFI_USART_RX_BUFF,s)!=0)return 0;
		
		//���ش���
		//if(strstr(WIFI_USART_RX_BUFF,"ERROR")!=0)return 100;
		//if(strstr(WIFI_USART_RX_BUFF,"+CPIN: NOT READY")!=0)return 100;
		
	}
	return 1;
}

//��WIFIģ�鷢������
//CommandStr �����ַ���
//Check ��������ظ�
//retry ���Դ���
//ÿ�εȴ�����ظ���ʱ�� ��λ1S
uint8_t WIFI_SendCommand(char *CommandStr,char *CheckStr,u16 retry,u16 WaitTimeForEvery_1S)
{
	u8 re=0,i;
	u16 iu;
	/*�Ƚ�����ʱ����ֹ��������֮��û�м���������*/
	delay_ms(10);
	
	for(i=0;i<retry;i++)
	{
		//����ATָ��
		WIFI_Put_Str(CommandStr);
				
		/*���GPRS���ڻ���*/
		for(iu=0;iu<LEN_WIFI_USART_RX;iu++)WIFI_USART_RX_BUFF[iu]=0;
		
		//��ȡGPRSģ�鷵�ص��ַ����������͵�һ�������Ƚϣ���ʱ��λ��10mS,1S=100;
		re=WIFI_ResponseCheck(CheckStr,WaitTimeForEvery_1S);
		
		//��ȡ����ֵ,�����ȷ����ѭ��
		if(re==0) break;
		
		//����Error
		//if(re==100) return 100;
	}
	if(re!=0)return 1;
	
	/*�Ƚ�����ʱ����ֹ��������֮��û�м���������*/
	delay_ms(100);
	return 0;
}
uint8_t Wifi_Init(void)
{
	char *buf;
	int tmp=0;
	char i=0;
	char IP_RX_BUFF_B[20];
	char Command[100];
	
	WIFI_SendCommand("ATE0\r\n","OK\r\n",2,10);
	WIFI_SendCommand("AT+CWMODE=3\r\n","OK\r\n",2,10);
	for(i=0;i<3;i++)delay_ms(1000);
	WIFI_SendCommand("AT+CIFSR\r\n","OK\r\n",2,10);//��ȡ����ģ��IP��ַ
	buf=strstr(WIFI_USART_RX_BUFF,"STAIP")+6;

	for(i=0;i<18;i++)
	{
		if(*buf=='.') tmp++;
		IP_RX_BUFF_B[i]=IP_RX_BUFF[i] = *buf;
		buf++;
		if(tmp==2) 
		{
			IP_RX_BUFF_B[i]=0;
			//IP_RX_BUFF_B[i+1]=0;
		}			
		if(tmp==3) 
		{
			IP_RX_BUFF[i]='.';
			IP_RX_BUFF[i+1]=0;
			break;
		}
	}
	
	printf("IP:%s\r\n",IP_RX_BUFF+1);
	tmp=str2int(IP_RX_BUFF+1);
	printf("tmp:%d\r\n",tmp);
	
	sprintf(Command,"AT+CIPSTART=\"UDP\",\"%s255\",8080,1112,2\r\n",IP_RX_BUFF+1);//�����ַ���
	WIFI_SendCommand(Command,"CONNECT",1,500);
	return 0;
}
void USART3_IRQHandler(void)
{
	static u8 lastdata;
	u8 tmp=0;
	static uint16_t pdataM=0;
	IP_RX_BUFF[14]=0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{ 	
		tmp=USART3->DR;
		
		#ifdef USART3_Remap_USART1//����WIFIʱʹ��
				USART1->DR = (tmp & (uint16_t)0x01FF);
		#endif
    /*����Ƿ��м���ǻ��߿������·���������*/
		if(lastdata==0xAB && tmp==0xBA)//�յ�֡ͷ
		{
			pdataM=0;//����ָ�����
		}
		Air_RX[pdataM]=tmp;//���յ���������뻺����
		pdataM++;//����ָ���һ
		if(pdataM>10)pdataM=10;//��ֹ�������
    /*����Ƿ����ӵ�AP*/
		if(tmp=='T'&&lastdata=='O')
		{
//			W_f=1;
		}	
    /*WIFI����д�뻺��*/
		WIFI_USART_RX_BUFF[WIFI_USART_RX_BUFF_p]=tmp;
		WIFI_USART_RX_BUFF_p++;
		
		if(WIFI_USART_RX_BUFF_p>LEN_WIFI_USART_RX)WIFI_USART_RX_BUFF_p=LEN_WIFI_USART_RX;//��ֹ���ݻ������
		
		lastdata=tmp;//������һ�ֽ�����		
	}
	USART_ClearFlag(USART3,USART_FLAG_RXNE); 
}

/*�ַ���ת��������*/
int str2int(const char *str)
{
		int temp = 0;
    const char *ptr = str;
 
    if(*str == '-' || *str == '+') str++;
    while(*str != 0)
    {
       if ((*str < '0') || (*str > '9')) break;
       temp = temp * 10 + (*str - '0');
       str++;
    }  
    if(*ptr == '-')temp = -temp;
    return temp;
}

/*********************************************END OF FILE**********************/
