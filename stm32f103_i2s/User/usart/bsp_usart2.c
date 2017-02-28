/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usartӦ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO-MINI STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_usart2.h"

 /**
  * @brief  USART3 GPIO ����,����ģʽ���á�9600 8-N-1
  * @param  ��
  * @retval ��
  */
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	/* USART3 GPIO config */
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*485ʹ�ܶ�*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* USART3 mode config */
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
//     GPIO_SetBits(GPIOA,GPIO_Pin_7);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	/* ʹ�ܴ���3�����ж� */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);
	
	/// ����USART3�����ж�
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}


void USART2_IRQHandler(void)
{
	uint8_t ch1;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{ 	
	    //ch = USART1->DR;
			ch1 = USART_ReceiveData(USART2);
	  	printf( "%c", ch1 );    //�����ܵ�������ֱ�ӷ��ش�ӡ
	} 
}


/// �ض���c�⺯��printf��USART1
// int fputc(int ch, FILE *f)
// {
// 		/* ����һ���ֽ����ݵ�USART1 */
// 		USART_SendData(USART1, (uint8_t) ch);
// 		
// 		/* �ȴ�������� */
// 		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
// 	
// 		return (ch);
// }

// /// �ض���c�⺯��scanf��USART1
// int fgetc(FILE *f)
// {
// 		/* �ȴ�����1�������� */
// 		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

// 		return (int)USART_ReceiveData(USART1);
// }
/*********************************************END OF FILE**********************/
