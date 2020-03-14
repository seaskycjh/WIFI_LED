#include "usart.h"
#include "timer.h"
#include "mystr.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#endif

//加入以下代码,支持printf函数
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle;
};

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x;
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
		; //循环发送,直到发送完毕
  USART1->DR = (u8) ch;
	return ch;
}
#endif

URX_t u1rx;
URX_t u2rx;
  
void USART1_Init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //USART1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;		//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;		//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//收发模式

  USART_Init(USART1, &USART_InitStructure); 	//初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1
}

void USART2_Init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //USART1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;		//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口1
}

void rx_clr(URX_t *prx)
{
	prx->sta = 0;
	prx->size = 0;
}
	
u8 USART_SendString(USART_TypeDef *USARTx, u8 *str)
{
	u8 i;
	for(i = 0; str[i]; i++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) != SET);  //先判断，可防止漏发第一个字节
		USART_SendData(USARTx, str[i]);
	}
	return i;
}

void USART1_IRQHandler(void)  //串口1中断服务程序
{
	u8 res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART1);	//收到数据直接发给ESP8266
		if(u1rx.sta != 0xff){
			u1rx.buf[u1rx.size++] = res;
			if(res == 0x0d) u1rx.sta = 0x0f;
			if(res == 0x0a && u1rx.sta == 0x0f){
				u1rx.buf[u1rx.size] = 0;
				u1rx.sta = 0xff;
			}
		}
  }
}

void USART2_IRQHandler(void)  //串口2中断服务程序
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART2);
		if(u2rx.sta != 0xff){
			u2rx.buf[u2rx.size++] = res;
			timeout = 5;
		}
  }
}
