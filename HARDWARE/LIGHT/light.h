#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "sys.h"

extern u8 rgb[3];

void Light_Init(void);
void Light_RGB(void);
void Light_String(u8 *str);

#endif
