#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>

/********************宏定义********************/
#define WIFI_Put_Str      UART3_Put_Str
#define WIFI_Put_Char	    UART3_Put_Char


#define LEN_WIFI_USART_RX 		300           //WIFI串口接收缓存长度
#define LEN_IP_USART_RX 		50              //WIFI串口接收缓存长度

#define DEVICE_ID 0x02
#define USART3_Remap_USART1
/**********************************************/

void USART3_Config(void);
int str2int(const char *str);
void UART3_Put_Str(char *s);
void UART3_Put_Char(char s);
uint8_t WIFI_SendCommand(char *CommandStr,char *CheckStr,u16 retry,u16 WaitTimeForEvery_1S);
uint8_t Wifi_Init(void);

extern u8 Air_RX[10];
#endif /* __USART3_H */
