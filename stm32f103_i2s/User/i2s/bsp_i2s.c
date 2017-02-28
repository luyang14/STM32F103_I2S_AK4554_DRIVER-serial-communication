/**
  ******************************************************************************
  * @file bsp_i2s.c 
  * @author  luyang
  * @version  V1.0
  * @date  2017-02-23
  * @brief  I2S 初始化
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "bsp_i2s.h"

u8 I2S_RX=0;
u8 I2S_TX=0;

 /*
**************************************************************************
*	函 数 名: I2S_NVIC_Config
*	功能说明: 配置I2S NVIC通道(中断模式)。
*	形    参:  无
*	返 回 值: 无
**************************************************************************
*/
static void I2S_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  /* SPI2 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* SPI3 IRQ channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
}
 /*
*************************************************************************
*	函 数 名: STM32F10X_I2S_Init
*	功能说明: 配置I2S
*	形    参: 无
*	返 回 值: 无
*************************************************************************
*/
void STM32F10X_I2S_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  I2S_InitTypeDef I2S_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  
	/* SPI2 and SPI3 clocks enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3, ENABLE);
  	
	/* Disable the JTAG interface and enable the SWJ interface */
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

  /* Configure SPI2 pins: CK, WS and SD --------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure SPI3 pins: CK and SD ------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure SPI3 pins: WS -------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure I2S3 pins: MCK ------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* Configure I2S2 pins: MCK ------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* I2S peripheral configuration */
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;//extended
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_16k;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;

  /* I2S2 Master Transmitter to I2S3 Slave Receiver communication ------------*/
  /* I2S2 configuration */
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
  I2S_Init(SPI2, &I2S_InitStructure);
	
  /* I2S3 configuration */
  I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;
  I2S_Init(SPI3, &I2S_InitStructure);
  
	/* Enable the I2S2 RxNE interrupt */
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);

  /* Enable the I2S3 TxE interrupt */
  SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);
	
	/* Enable the I2S2 */
  I2S_Cmd(SPI2, ENABLE);
	
  /* Enable the I2S3 */
  I2S_Cmd(SPI3, ENABLE);

	I2S_NVIC_Config();
}

static u16 sample_mic=0;

void SPI2_IRQHandler()
{
  if (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == SET)
		{
			sample_mic = SPI_I2S_ReceiveData(SPI2);
			I2S_RX=1;
		}
}

void SPI3_IRQHandler()
{
 	if (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == SET)
		{
			SPI_I2S_SendData(SPI3,sample_mic);
			I2S_TX=1;
		}	
}

/******************* (C) COPYRIGHT 2017 STMicroelectronics ***************/
