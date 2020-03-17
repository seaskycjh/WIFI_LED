#include "motor.h"
#include "delay.h"
#include "sys.h"


//MOTOR IN1~4 -> GPIOC0~3
#define MOTOR_IN(x) GPIOC->ODR = x;

#define IN1 PCout(0)
#define IN2 PCout(1)
#define IN3 PCout(2)
#define IN4 PCout(3)

//��������С��ʱԼΪ10ms����С�Ļ����ֻ���𶯶�����ת��
u8 motor_speed = 5;

//���˳ʱ��ת�����1->2->3->4
//��ʱ��ת��ֻ��ı�����ȡ˳�򼴿ɣ�4->3->2->1
const u8 phase[4] = {0x01, 0x02, 0x04, 0x08};

//���״̬
enum MSTA motor_sta = STOP;

//������ſ���
void motor_cmd(u8 val)
{
	IN1 = val & 0x01;
	IN2 = (val >> 1) & 0x01;
	IN3 = (val >> 2) & 0x01;
	IN4 = (val >> 3) & 0x01;
}

//������ų�ʼ��
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
}

//���˳ʱ��ת��
void motor_cw(void)
{
	u8 i;
	for(i = 0; i < 4; i++){
		motor_cmd(phase[i]);
		delay_ms(motor_speed);
	}
}

//�����ʱ��ת��
void motor_ccw(void)
{
	s8 i;
	for(i = 3; i >= 0; i--){
		motor_cmd(phase[i]);
		delay_ms(motor_speed);
	}
}

//���ֹͣת��
void motor_stop(void)
{
	motor_cmd(0x00);
	delay_ms(motor_speed);
}

void motor_ctl(u8 *str)
{
	if(mstrcmp(str, "stop\n") == 0) motor_sta = STOP;
	else if(mstrcmp(str, "cw\n") == 0) motor_sta = CW;
	else if(mstrcmp(str, "ccw\n") == 0) motor_sta = CCW;
	else return;
}


