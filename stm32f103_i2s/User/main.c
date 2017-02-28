/**
  ******************************************************************************
  * @file    main.c
  * @author  luyang
  * @version V1.0
  * @date    2017-02-22
  * @brief   stm32f103 IIS�������
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_i2s.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	delay_init();
	Led_Init();
	USART1_Config();
	STM32F10X_I2S_Init();
	while(1)
	{
		delay_ms(500);
		printf( "HelloWorld!!\r\n" );
		GPIOD->ODR ^=GPIO_Pin_0;
	}
}

/*********************************END OF FILE**********************************/
