/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   usart应用bsp
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
  * @brief  USART3 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
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
	
	/* 使能串口3接收中断 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
	// 配置USART3接收中断
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/********************************
 * 函数名：UART3_Put_Str
 * 描述  ：初始化串口3
 * 输入  ：无
 * 输出  ：无
 *********************************/
void UART3_Put_Str(char *s)
{
	u16 i=0;
	while(s[i]!='\0')
	{
		//将要发送的字节写到UART3的发送缓冲区
		USART_SendData(USART3, s[i]);
		
		#ifdef USART3_Remap_USART1
				USART1->DR = (s[i] & (uint16_t)0x01FF);
		#endif
		
		//等待发送完成
		while (!(USART3->SR & USART_FLAG_TXE));
		i++;
	}
}
//uart3发送字符
void UART3_Put_Char(char s)
{
	//将要发送的字节写到UART3的发送缓冲区
	USART_SendData(USART3, s);
	//等待发送完成
	while (!(USART3->SR & USART_FLAG_TXE));
}
//读取GPRS模块返回的字符串，并作和第一个入参相比较，延时单位是10mS,1S=100;
uint8_t WIFI_ResponseCheck(char *s,u16 Deley_1S)
{
	u16 i;
	
	//清空接收缓存指针
	WIFI_USART_RX_BUFF_p=0;
//	IP_RX_BUFF_p=0;
	
	for(i=0;i<Deley_1S*10;i++)
	{
		/*先进行延时 */
		delay_ms(100);
			
		/*截取字符串，防止旧的缓存被读到*/
		WIFI_USART_RX_BUFF[WIFI_USART_RX_BUFF_p+1]=0;
		
		//比较字符串
		if(strstr(WIFI_USART_RX_BUFF,s)!=0)return 0;
		
		//返回错误
		//if(strstr(WIFI_USART_RX_BUFF,"ERROR")!=0)return 100;
		//if(strstr(WIFI_USART_RX_BUFF,"+CPIN: NOT READY")!=0)return 100;
		
	}
	return 1;
}

//向WIFI模块发送命令
//CommandStr 命令字符串
//Check 检测的命令回复
//retry 重试次数
//每次等待命令回复的时间 单位1S
uint8_t WIFI_SendCommand(char *CommandStr,char *CheckStr,u16 retry,u16 WaitTimeForEvery_1S)
{
	u8 re=0,i;
	u16 iu;
	/*先进行延时，防止两条命令之间没有间隔引起出错*/
	delay_ms(10);
	
	for(i=0;i<retry;i++)
	{
		//发送AT指令
		WIFI_Put_Str(CommandStr);
				
		/*清空GPRS串口缓存*/
		for(iu=0;iu<LEN_WIFI_USART_RX;iu++)WIFI_USART_RX_BUFF[iu]=0;
		
		//读取GPRS模块返回的字符串，并作和第一个入参相比较，延时单位是10mS,1S=100;
		re=WIFI_ResponseCheck(CheckStr,WaitTimeForEvery_1S);
		
		//读取返回值,如果正确跳出循环
		if(re==0) break;
		
		//读到Error
		//if(re==100) return 100;
	}
	if(re!=0)return 1;
	
	/*先进行延时，防止两条命令之间没有间隔引起出错*/
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
	WIFI_SendCommand("AT+CIFSR\r\n","OK\r\n",2,10);//获取控制模块IP地址
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
	
	sprintf(Command,"AT+CIPSTART=\"UDP\",\"%s255\",8080,1112,2\r\n",IP_RX_BUFF+1);//连接字符串
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
		
		#ifdef USART3_Remap_USART1//调试WIFI时使用
				USART1->DR = (tmp & (uint16_t)0x01FF);
		#endif
    /*监测是否有检测仪或者控制器下发控制命令*/
		if(lastdata==0xAB && tmp==0xBA)//收到帧头
		{
			pdataM=0;//缓存指针归零
		}
		Air_RX[pdataM]=tmp;//将收到的命令存入缓存中
		pdataM++;//缓存指针加一
		if(pdataM>10)pdataM=10;//防止缓存溢出
    /*检测是否连接到AP*/
		if(tmp=='T'&&lastdata=='O')
		{
//			W_f=1;
		}	
    /*WIFI数据写入缓存*/
		WIFI_USART_RX_BUFF[WIFI_USART_RX_BUFF_p]=tmp;
		WIFI_USART_RX_BUFF_p++;
		
		if(WIFI_USART_RX_BUFF_p>LEN_WIFI_USART_RX)WIFI_USART_RX_BUFF_p=LEN_WIFI_USART_RX;//防止数据缓存溢出
		
		lastdata=tmp;//缓存上一字节数据		
	}
	USART_ClearFlag(USART3,USART_FLAG_RXNE); 
}

/*字符串转整数函数*/
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
