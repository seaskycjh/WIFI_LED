#ifndef _MYSTR_H_
#define _MYSTR_H_

typedef unsigned char u8;

void mstrclr(u8 *str);
u8 mstrcmp(u8 *s1, u8 *s2);
u8 *mstrchr(u8 *str, u8 c);
u8 hex2int(u8 c);

#endif
