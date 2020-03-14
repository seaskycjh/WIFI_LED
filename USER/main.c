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
	//Ӳ����ʼ��
	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    	  //��ʱ������ʼ��
	USART1_Init(115200);  //����1���ӵ��ԣ����ڵ���
	USART2_Init(115200);		//����2����ESP8266
	TIM3_Int_Init(999,7199);  //��ʱ��3��ʼ��
	Light_Init();         //��ɫLED��ʼ��
	
	
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

void uart2_task(void *pvParameters)
{
	u8 *res;
	while(1){
		if(Usart2Rx.RX_STA == 0xFF){
			if((res = mstrchr(Usart2Rx.RX_BUF, ':')) != NULL){
				res++;
				Light_String(res);
			}
			RX_Clr(&Usart2Rx);
		}
		vTaskDelay(20);
	}
}

		
