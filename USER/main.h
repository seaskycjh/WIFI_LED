#ifndef _MAIN_H_
#define _MAIN_H_

#include "FreeRTOS.h"
#include "task.h"

void draw_page(void);

#define START_TASK_PRIO 1
#define START_STK_SIZE 128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

#define MOTOR_TASK_PRIO 3
#define MOTOR_STK_SIZE 64
TaskHandle_t MotorTask_Handler;
void motor_task(void *pvParameters);

#define UART1_TASK_PRIO 4
#define UART1_STK_SIZE 128
TaskHandle_t Uart1Task_Handler;
void uart1_task(void *pvParameters);

#define UART2_TASK_PRIO 5
#define UART2_STK_SIZE 128
TaskHandle_t Uart2Task_Handler;
void uart2_task(void *pvParameters);

#define KEY_TASK_PRIO 6
#define KEY_STK_SIZE 64
TaskHandle_t KeyTask_Handler;
void key_task(void *pvParameters);

#endif
