/**
  ******************************************************************************
  * @file    bsp_Key.c
  * @author  luyang
  * @version V1.0
  * @date    2016-10-14
  * @brief   Ӳ��������ʼ��
  ******************************************************************************
  */
#include "bsp_Key.h"
#include "bsp_usart3.h"
#include "stdio.h"

u16 key_press_time=0;//��������ʱ��
char WIFI_Con_Flag=2;//wifi���ӱ�־

void KEY_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable GPIOA and GPIOE clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET) //ȷ���Ƿ������EXTI Line�ж�
	{
//		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		TIM_Cmd(TIM4, ENABLE);
		key_press_time=0;
//		printf("���밴���ж�\r\n");
		EXTI_ClearITPendingBit(EXTI_Line3);     //����жϱ�־λ
	}  
}
// TIM7�ж����ȼ�����
void TIM4_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/************************************************************************************
 * ��������BSP_TIM7_init
 * ����  ����ʼ����ʱ��7
 * ����  ����
 * ���  ����
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * �ж�����Ϊ = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> �ж� ��TIMxCNT����Ϊ0���¼��� 
 ***********************************************************************************/
void BSP_TIM4_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* ����TIM6CLK Ϊ 72MHZ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
	//TIM_DeInit(TIM6);
	TIM4_NVIC_Configuration();
	/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
	TIM_TimeBaseStructure.TIM_Period=1000;

	/* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
	/* ʱ��Ԥ��Ƶ��*/
	TIM_TimeBaseStructure.TIM_Prescaler= 71;

	/* ���ⲿʱ�ӽ��в�����ʱ�ӷ�Ƶ,����û���õ� */
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4, DISABLE);				
}
void TIM4_IRQHandler(void)
{
	static char key_press_flag=0;
	
	if(TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
	 {	
		 key_press_time++;
		 if((key_press_time>30)&&(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0))//������������
		 {
			 /*�������±�־*/
			key_press_flag=1;
		 }
		 if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)!=0)&&(key_press_flag==1))
			{
				key_press_flag=0;
				/*�̰�ʵ��wifi���*/
				if(key_press_time<10000) 
				{
					WIFI_Put_Str("AT+CWSTARTSMART\r\n");
//					printf("DUANAN");
					/*��������ʱ������*/
					key_press_time=0; 
					WIFI_Con_Flag=0;
					TIM_Cmd(TIM4, DISABLE);	//�رն�ʱ��
				}				
			}
		 if((key_press_time>10000)&&(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0))//������������
			{
			  printf("�ָ���������");//�����ظ���������
			  key_press_time=0;			//����ʱ������
		//			Set_up_init();
			  TIM_Cmd(TIM4, DISABLE);//�رն�ʱ��
			}
		 if (key_press_time>=60000)key_press_time=60000;
		 TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);  		 
	 }		 	
 }
/**************************************end of file**************************************/
