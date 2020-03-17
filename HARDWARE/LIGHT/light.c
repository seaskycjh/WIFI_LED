#include "light.h"
#include "timer.h"
#include "mystr.h"

u8 rgb[3] = {0};  //三种颜色的比较值

//颜色表
//R：红色(255, 0, 0)
//G：绿色(0, 255, 0)
//B：蓝色(0, 0, 255)
//Y：黄色(255, 255, 0)
//A：水色(0, 255, 255)
//F：紫红色(255, 0, 255)
//W：白色(255, 255, 255)

//R: CH1 PD12, G: CH2 PD13, B: CH4 PD14

//彩色灯初始化
void Light_Init(void)
{
	TIM4_PWM_Init(255, 0);
	Light_RGB();  //关灯
}

void Set_RGB(u8 r, u8 g, u8 b)
{
	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

//根据R、G、B的值设置灯光
void Light_RGB(void)
{
	TIM_SetCompare1(TIM4, rgb[0]);
	TIM_SetCompare2(TIM4, rgb[1]);
	TIM_SetCompare3(TIM4, rgb[2]);
}

//根据字符控制灯光
void Light_Char(u8 c, u8 val)
{
	if(c == 'R' || c == 'r') Set_RGB(val, 0, 0);
	else if(c == 'G' || c == 'g') Set_RGB(0, val, 0);
	else if(c == 'B' || c == 'b') Set_RGB(0, 0, val);
	else if(c == 'Y' || c == 'y') Set_RGB(val, val, 0);
	else if(c == 'A' || c == 'a') Set_RGB(0, val, val);
	else if(c == 'F' || c == 'f') Set_RGB(val, 0, val);
	else if(c == 'W' || c == 'w') Set_RGB(val, val, val);
	else if(c == 'O' || c == 'o') Set_RGB(0, 0, 0);
	//else return;
}
		
//根据字符串控制灯光
void Light_String(u8 *str)
{
	u8 i, j, val = 0;
	u8 flag = str[0];
	if(flag == '#'){  //Hex颜色代码
		for(i = 1, j = 0; i < 7; i += 2, j++){
			rgb[j] = hex2int(str[i]) * 16 + hex2int(str[i+1]);
		}
	}else{
		for(i = 1; str[i]; i++){
			if(str[i] >= '0' && str[i] <= '9'){
				val = val * 10 + str[i] - '0';
			}
		}
		if(val > 255) val = 255;  //亮度值最大为255
		Light_Char(flag, val);
	}
	Light_RGB();
}
