#ifndef _USART_H_
#define _USART_H_
#include "stdio.h"
#include "sys.h"

#define RX_LEN 128

//定义串口接收结构体
typedef struct
{
	u8 sta;
	u8 size;
	u8 buf[RX_LEN];
}URX_t;

extern URX_t u1rx;
extern URX_t u2rx;

void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void rx_clr(URX_t *prx);
u8 USART_SendString(USART_TypeDef *USARTx, u8 *str);

#endif
