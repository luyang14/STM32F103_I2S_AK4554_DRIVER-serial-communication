#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

extern uint8_t USART1_Tx_Buffer[];
extern uint8_t USART1_Rx_Buffer[];
extern uint8_t Flag_USART1_RX_Finish;
extern uint8_t Flag_USART1_TX_Finish;

void USART1_Config(u32 Baud_rate);
void NVIC_Configuration(void);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif /* __USART1_H */
