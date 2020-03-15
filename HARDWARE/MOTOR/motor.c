#include "motor.h"
#include "delay.h"
#include "sys.h"


//MOTOR IN1~4 -> GPIOC0~3
#define MOTOR_IN(x) GPIOC->ODR=x;

u8 motor_speed = 25;
const u8 phase[4] = {0x08, 0x04, 0x02, 0x01};

void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void motor_cw(void)
{
	u8 i;
	for(i = 0; i < 4; i++){
		MOTOR_IN((u32)phase[i]);
		delay_ms(motor_speed);
	}
	
}

void motor_ccw(void)
{
	s8 i;
	for(i = 3; i >= 0; i++){
		MOTOR_IN((u32)phase[i]);
		delay_ms(motor_speed);
	}
}

void motor_stop(void)
{
	MOTOR_IN(0x00);
}

