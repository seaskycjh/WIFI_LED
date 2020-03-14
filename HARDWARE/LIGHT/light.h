#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "sys.h"

#define RIO PDout(12)
#define GIO PDout(13)
#define BIO PDout(14)

void Light_Init(void);
void Light_RGB(void);
void Light_String(u8 *str);

#endif
