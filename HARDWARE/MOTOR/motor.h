#ifndef MOTOR_H
#define MOTOR_H
#include "mystr.h"

enum MSTA{STOP, CW, CCW};

extern enum MSTA motor_sta;

void Motor_Init(void);
void motor_cw(void);
void motor_ccw(void);
void motor_stop(void);
void motor_ctl(u8 *str);

#endif
