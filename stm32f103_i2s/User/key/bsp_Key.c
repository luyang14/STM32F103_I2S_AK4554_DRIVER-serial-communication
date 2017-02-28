/**
  ******************************************************************************
  * @file    bsp_Key.c
  * @author  luyang
  * @version V1.0
  * @date    2016-10-14
  * @brief   硬件按键初始化
  ******************************************************************************
  */
#include "bsp_Key.h"
#include "bsp_usart3.h"
#include "stdio.h"

u16 key_press_time=0;//按键按下时间
char WIFI_Con_Flag=2;//wifi连接标志

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
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
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
	if(EXTI_GetITStatus(EXTI_Line3) != RESET) //确保是否产生了EXTI Line中断
	{
//		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		TIM_Cmd(TIM4, ENABLE);
		key_press_time=0;
//		printf("进入按键中断\r\n");
		EXTI_ClearITPendingBit(EXTI_Line3);     //清除中断标志位
	}  
}
// TIM7中断优先级配置
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
 * 函数名：BSP_TIM7_init
 * 描述  ：初始化定时器7
 * 输入  ：无
 * 输出  ：无
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * 中断周期为 = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> 中断 且TIMxCNT重置为0重新计数 
 ***********************************************************************************/
void BSP_TIM4_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* 设置TIM6CLK 为 72MHZ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
	//TIM_DeInit(TIM6);
	TIM4_NVIC_Configuration();
	/* 自动重装载寄存器周期的值(计数值) */
	TIM_TimeBaseStructure.TIM_Period=1000;

	/* 累计 TIM_Period个频率后产生一个更新或者中断 */
	/* 时钟预分频数*/
	TIM_TimeBaseStructure.TIM_Prescaler= 71;

	/* 对外部时钟进行采样的时钟分频,这里没有用到 */
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
		 if((key_press_time>30)&&(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0))//消除按键抖动
		 {
			 /*按键按下标志*/
			key_press_flag=1;
		 }
		 if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)!=0)&&(key_press_flag==1))
			{
				key_press_flag=0;
				/*短按实现wifi配对*/
				if(key_press_time<10000) 
				{
					WIFI_Put_Str("AT+CWSTARTSMART\r\n");
//					printf("DUANAN");
					/*按键按下时间清零*/
					key_press_time=0; 
					WIFI_Con_Flag=0;
					TIM_Cmd(TIM4, DISABLE);	//关闭定时器
				}				
			}
		 if((key_press_time>10000)&&(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0))//消除按键抖动
			{
			  printf("恢复出厂设置");//长按回复出厂设置
			  key_press_time=0;			//按动时间清零
		//			Set_up_init();
			  TIM_Cmd(TIM4, DISABLE);//关闭定时器
			}
		 if (key_press_time>=60000)key_press_time=60000;
		 TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);  		 
	 }		 	
 }
/**************************************end of file**************************************/
