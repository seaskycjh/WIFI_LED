#include "sys.h"
#include "oled.h"
#include "delay.h"
#include "font.h"

#define IIC_SCL PAout(6)
#define IIC_SDA PAout(7)
#define READ_SDA PAin(7)

#define SDA_IN() {GPIOA->CRL &= 0x0fffffff; GPIOA->CRL |= (u32)8<<28;}
#define SDA_OUT() {GPIOA->CRL &= 0x0fffffff; GPIOA->CRL |= (u32)3<<28;}

//OLED显存
u8 OLED_GRAM[128][8];

//使用软件模拟IIC
//SCL->PA6 SDA->PA7
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	//使能GPIOB时钟
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);	//PB6,7输出高
}

//IIC开始信号，SCL: 1, SDA: 1->0
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(4);
	IIC_SDA = 0;
	delay_us(4);
	IIC_SCL = 0;
}

//IIC停止信号，SCL: 1, SDA: 0->1
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL = 0;
	IIC_SDA = 0;
	delay_us(4);
	IIC_SCL = 1;
	IIC_SDA = 1;
	delay_us(4);
}

//IIC等待应答信号
//返回值：1(接收应答失败) 0(接收应答成功)
u8 IIC_Wait_Ack(void)
{
	u8 t = 0;
	SDA_IN();
	IIC_SDA = 1;
	delay_us(1);
	IIC_SCL = 1;
	delay_us(1);
	while(READ_SDA){
		t++;
		if(t > 250){
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL = 0;
	return 0;
}

//IIC产生应答信号
void IIC_Ack(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}

//IIC不产生应答信号
void IIC_NAck(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}

//IIC发送一个字节
void IIC_Send_Byte(u8 txd)
{
	u8 i;
	SDA_OUT();
	IIC_SCL = 0;	//拉低时钟开始数据传输
	for(i = 0; i < 8; i++){
		//从字节的最高位开始读
		if((txd&0x80)>>7) IIC_SDA = 1;
		else IIC_SDA = 0;
		txd <<= 1;
		delay_us(2);
		IIC_SCL = 1;
		delay_us(2);
		IIC_SCL = 0;
		delay_us(2);
	}
}

//IIC读一个字节
u8 IIC_Read_Byte(u8 ack)
{
	u8 i, recv = 0;
	SDA_IN();
	for(i = 0; i < 8; i++){
		IIC_SCL = 0;
		delay_us(2);
		IIC_SCL = 1;
		recv <<= 1;
		if(READ_SDA) recv++;
		delay_us(1);
	}
	if(!ack) IIC_NAck();
	else IIC_Ack();
	return recv;
}

//向OLED写命令
void OLED_WR_Cmd(u8 cmd)
{
	IIC_Start();
	IIC_Send_Byte(0x78);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);
	IIC_Wait_Ack();
	IIC_Send_Byte(cmd);
	IIC_Wait_Ack();
	IIC_Stop();
}

//向OLED写数据
void OLED_WR_Data(u8 data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x40);
	IIC_Wait_Ack();
	IIC_Send_Byte(data);
	IIC_Wait_Ack();
	IIC_Stop();
}
	
//OLED初始化
void OLED_Init(void)
{
	IIC_Init();
	delay_ms(1);
	
	OLED_WR_Cmd(0xae);	//关闭显示
	
	OLED_WR_Cmd(0x00);
	OLED_WR_Cmd(0x10);
	OLED_WR_Cmd(0x40);
	OLED_WR_Cmd(0xb0);
	
	OLED_WR_Cmd(0x20);	//设置内存地址模式
	OLED_WR_Cmd(0x02);
	
	OLED_WR_Cmd(0x81);
	OLED_WR_Cmd(0xff);
	OLED_WR_Cmd(0xa1);
	OLED_WR_Cmd(0xa6);
	OLED_WR_Cmd(0xa8);
	OLED_WR_Cmd(0x3f);
	
	OLED_WR_Cmd(0xc0);	//设置扫描方向，0xc0~0xc8
	OLED_WR_Cmd(0xd3);
	OLED_WR_Cmd(0x00);
	
	OLED_WR_Cmd(0xd5);
	OLED_WR_Cmd(0x80);
	
	OLED_WR_Cmd(0xd8);
	OLED_WR_Cmd(0x05);
	
	OLED_WR_Cmd(0xd9);
	OLED_WR_Cmd(0xf1);
	
	OLED_WR_Cmd(0xda);
	OLED_WR_Cmd(0x12);
	
	OLED_WR_Cmd(0xdb);
	OLED_WR_Cmd(0x30);
	
	OLED_WR_Cmd(0x8d);
	OLED_WR_Cmd(0x14);
	
	OLED_WR_Cmd(0xaf);
}

//更新显存到OLED
void OLED_Refresh_Gram(void)
{
	u8 i, j;
	for(i = 0; i < 8; i++){
		OLED_WR_Cmd(0xb0+i);
		OLED_WR_Cmd(0x00);
		OLED_WR_Cmd(0x10);
		for(j = 0; j < 128; j++){
			OLED_WR_Data(OLED_GRAM[j][i]);
		}
	}
}

//OLED开启显示
void OLED_Display_On(void)
{
	OLED_WR_Cmd(0x8d);
	OLED_WR_Cmd(0x14);
	OLED_WR_Cmd(0xaf);
}

//OLED关闭显示
void OLED_Display_Off(void)
{
	OLED_WR_Cmd(0x8d);
	OLED_WR_Cmd(0x10);
	OLED_WR_Cmd(0xae);
}

//OLED清屏，1为显示，0为清空
void OLED_Clear(u8 t)
{
	u8 i, j;
	for(i = 0; i < 8; i++){
		for(j = 0; j < 128; j++){
			OLED_GRAM[j][i] = t;
		}
	}
	OLED_Refresh_Gram();
}

//OLED设置坐标
void OLED_SetPos(u8 x, u8 y)
{
	if(x>127||y>63) return;	//超出范围
	OLED_WR_Cmd(0xb0+y);
	OLED_WR_Cmd(((x&0xf0)>>4)|0x10);
	OLED_WR_Cmd(x&0x0f);
}

//OLED画点
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
	u8 pos, bx, tmp = 0;
	if(x>127||y>63) return;
	pos = 7-y/8;
	bx = y%8;
	tmp = 1<<(7-bx);
	if(t) OLED_GRAM[x][pos] |= tmp;
	else OLED_GRAM[x][pos] &= ~tmp;
}

//OLED显示字符
//mode：0为反白显示，1为正常显示
void OLED_ShowChar(u8 x, u8 y, u8 c, u8 size, u8 mode)
{
	u8 tmp, i, j;
	u8 y0 = y;
	u8 csize = (size/8+((size%8)?1:0))*(size/2);
	c = c - ' ';
	for(i = 0; i < csize; i++){
		if(size == 12) tmp = asc2_1206[c][i];
		else if(size == 16) tmp = asc2_1608[c][i];
		else if(size == 24) tmp = asc2_2412[c][i];
		else return;
		for(j = 0; j < 8; j++){
			if(tmp&0x80) OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			tmp <<= 1;
			y++;
			if((y-y0)==size){
				y = y0;
				x++;
				break;
			}
		}
	}
}

//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//OLED显示数字
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
}

//OLED显示字符串
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
  while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
  {       
    if(x>(128-(size/2))){
			x=0;
			y+=size;
		}
    if(y>(64-size)){
			y=x=0;
			OLED_Clear(0x00);
		}
    OLED_ShowChar(x,y,*p,size,1);
    x+=size/2;
    p++;
  }
}






