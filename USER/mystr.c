#include "mystr.h"
#include <stdlib.h>

//�ַ������
void mstrclr(u8 *str)
{
	while(*str){
		*str = 0;
		str++;
	}
}

//�ַ�������ĳ���ַ�
u8 *mstrchr(u8 *str, u8 c)
{
	while(*str)
	{
		if(*str == c) return str;
		str++;
	}
	return NULL;
}

//16�����ַ�ת��Ϊ����
u8 hex2int(u8 c)
{
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return c - '0';
}

