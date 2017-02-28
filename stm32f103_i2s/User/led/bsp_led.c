/**
  ******************************************************************************
  * @file bsp_led.c 
  * @author  luyang
  * @version  V1.0
  * @date  2016-10-09
  * @brief  LED ≥ı ºªØ
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "bsp_led.h"
          
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_0);	
}
/******************* (C) COPYRIGHT 2015 STMicroelectronics *********/
