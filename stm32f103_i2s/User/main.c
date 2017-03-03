/**
  ******************************************************************************
  * @file    main.c
  * @author  luyang
  * @version V1.0
  * @date    2017-02-22
  * @brief   stm32f103 IIS处理程序
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_i2s.h"
#include "bsp_Tim2.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint8_t i=0;
//	delay_init();
	Led_Init();
	USART1_Config(2000000);
	STM32F10X_I2S_Init();
	TIM2_Config();

	while(1)
	{
		if(Flag_USART1_RX_Finish==1)
		{
      sample_audio=0;
			sample_audio=USART1_Rx_Buffer[2];
			sample_audio<<=8;
			sample_audio|=USART1_Rx_Buffer[1];
//			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
			Flag_USART1_RX_Finish=0;
			SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);
		}
		if(Flag_USART1_TX_Finish==1)
		{
			USART1_Tx_Buffer[0]=0xEC;
		  USART1_Tx_Buffer[1]=0x10;
//			USART1_Tx_Buffer[4]=0xAA;
//		  USART1_Tx_Buffer[5]=0xBB;
			for(i=0;i<4;)
			{
				while((USART1->SR & 0x0080) == (uint16_t)RESET)
					;
				USART_SendData(USART1, USART1_Tx_Buffer[i]);
				i++;
			}
			Flag_USART1_TX_Finish=0;
			SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);	
	  }
//		delay_ms(500);
//		printf( "HelloWorld!!\r\n" );
//		GPIOD->ODR ^=GPIO_Pin_0;
//		USART_SendData(USART1, 0XEF);
	}
}

/*********************************END OF FILE**********************************/
