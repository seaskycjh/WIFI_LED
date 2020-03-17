#ifndef OLED_H
#define OLED_H

typedef unsigned char u8;
typedef unsigned int u32;

void OLED_Init(void);
void OLED_Refresh_Gram(void);
void OLED_Clear(u8 t);
void OLED_ShowChar(u8 x, u8 y, u8 c, u8 size, u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);

#endif
