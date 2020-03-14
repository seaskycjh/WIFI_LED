#include "main.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "light.h"
#include "timer.h"
#include "mystr.h"

int main(void)
{
	//硬件初始化
	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    	  //延时函数初始化
	USART1_Init(115200);  //串口1连接电脑，用于调试
	USART2_Init(115200);		//串口2连接ESP8266
	TIM3_Int_Init(999,7199);  //定时器3初始化
	Light_Init();         //彩色LED初始化
	
	
	xTaskCreate((TaskFunction_t)start_task,
							(const char *  )"start_task",
							(uint16_t      )START_STK_SIZE,
							(void *        )NULL,
							(UBaseType_t   )START_TASK_PRIO,
							(TaskHandle_t *)&StartTask_Handler);
	vTaskStartScheduler();
}

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();
	
	xTaskCreate((TaskFunction_t)key_task,
							(const char *  )"key_task",
							(uint16_t      )KEY_STK_SIZE,
							(void *        )NULL,
							(UBaseType_t   )KEY_TASK_PRIO,
							(TaskHandle_t *)&KeyTask_Handler);

	xTaskCreate((TaskFunction_t)uart1_task,
							(const char *  )"uart1_task",
							(uint16_t      )UART1_STK_SIZE,
							(void *        )NULL,
							(UBaseType_t   )UART1_TASK_PRIO,
							(TaskHandle_t *)&Uart1Task_Handler);
							
	xTaskCreate((TaskFunction_t)uart2_task,
							(const char *  )"uart2_task",
							(uint16_t      )UART2_STK_SIZE,
							(void *        )NULL,
							(UBaseType_t   )UART2_TASK_PRIO,
							(TaskHandle_t *)&Uart2Task_Handler);
	
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}

void connect(void)
{
	USART_SendString(USART1, "AT+CIPMUX=0\r\n");
	delay_ms(1000);
	USART_SendString(USART1, "AT+CIPSTART=\"TCP\",\"192.168.0.105\",8090\r\n");
}

void key_task(void *pvParameters)
{
	u8 key = 0;
	while(1){
		key = KEY_Scan(0);
		if(key){
			switch(key){
				case KEY0_PRES:
					break;
				case KEY1_PRES:
					break;
				case WKUP_PRES:
					break;
			}
		}else vTaskDelay(20);
	}
}

void uart1_task(void *pvParameters)
{
	while(1){
		if(u1rx.sta == 0xff){
			USART_SendString(USART2, u1rx.buf);
			rx_clr(&u1rx);
		}
		vTaskDelay(50);
	}
}

void uart2_task(void *pvParameters)
{
	u8 *str = NULL;
	while(1){
		if(u2rx.sta == 0xff){
			USART_SendString(USART1, u2rx.buf);
			if((str = mstrchr(u2rx.buf, 0x3a))){
				str++;
				Light_String(str);
				str = NULL;
			}
			rx_clr(&u2rx);
		}
		vTaskDelay(50);
	}
}

		

