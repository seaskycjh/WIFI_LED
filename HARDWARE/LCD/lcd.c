#include "lcd.h"
#include "stdlib.h"
#include "usart.h"
#include "delay.h"	 

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//������ɫ,������ɫ
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;
u16 DeviceCode;

//******************************************************************
//��������  LCD_WR_REG
//���ܣ�    ��Һ��������д��д16λָ��
//���������Reg:��д���ָ��ֵ
//����ֵ��  ��
//******************************************************************
void LCD_WR_REG(u16 data)
{ 
	LCD_CS_CLR; 
	LCD_RD_SET; 
	LCD_RS_CLR;	//д��ַ
	
	LCD_WR_CLR;
	DATAOUT(data);

	__NOP();__NOP();__NOP();__NOP();__NOP();
	LCD_WR_SET;
 	LCD_CS_SET;
}

//******************************************************************
//��������  LCD_WR_DATA
//���ܣ�    ��Һ��������д��д16λ����
//���������Data:��д�������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WR_DATA(u16 data)
{
	LCD_CS_CLR; 
	LCD_RD_SET; 
	LCD_RS_SET;
	
	LCD_WR_CLR;
	DATAOUT(data);

	__NOP();__NOP();__NOP();__NOP();__NOP();
	LCD_WR_SET;
	
	LCD_CS_SET;
}

//******************************************************************
//������LCD_WriteReg
//���ܣ�д�Ĵ�������
//������LCD_Reg:�Ĵ�����ַ
//			LCD_RegValue:Ҫд�������
//����ֵ����
//******************************************************************
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   
	 
//******************************************************************
//��������  LCD_WriteRAM_Prepare
//���ܣ�    ��ʼдGRAM
//			�ڸ�Һ��������RGB����ǰ��Ӧ�÷���дGRAMָ��
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

//******************************************************************
//��������  LCD_DrawPoint
//���ܣ�    ��ָ��λ��д��һ�����ص�����
//���������(x,y): �������
//					c: ��ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD_WR_DATA(POINT_COLOR); 
}

//******************************************************************
//��������  LCD_Clear
//���ܣ�    LCDȫ�������������
//���������Color:Ҫ���������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_Clear(u16 color)
{
	u16 i,j;      
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	for(i=0;i<lcddev.height;i++)
	{
		for(j=0;j<lcddev.width;j++)
		//LCD_WR_DATA(Color>>8);
		LCD_WR_DATA(color);   
	}
} 


void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	//ILI9481
	LCD_WR_REG(0x2a00);   
	LCD_WR_DATA(Xstart>>8);
	LCD_WR_REG(0x2a01); 
	LCD_WR_DATA(Xstart&0xff);
	LCD_WR_REG(0x2a02); 
	LCD_WR_DATA(Xend>>8);
	LCD_WR_REG(0x2a03); 
	LCD_WR_DATA(Xend&0xff);

	LCD_WR_REG(0x2b00);   
	LCD_WR_DATA(Ystart>>8);
	LCD_WR_REG(0x2b01); 
	LCD_WR_DATA(Ystart&0xff);
	LCD_WR_REG(0x2b02); 
	LCD_WR_DATA(Yend>>8);
	LCD_WR_REG(0x2b03); 
	LCD_WR_DATA(Yend&0xff);

	LCD_WR_REG(0x2c00);
}


void DispColor(u16 color)
{
	unsigned int i,j;

	BlockWrite(0,480-1,0,800-1);

	LCD_CS_CLR;
	LCD_RS_SET;
	LCD_RD_SET; 
	DATAOUT(color);
	for(i=0;i<480;i++)
	{
	  for(j=0;j<800;j++)
		{    
			LCD_WR_CLR;
			LCD_WR_SET;
		}
	}

	LCD_CS_SET; 
}

//******************************************************************
//��������  LCD_GPIOInit
//���ܣ�    Һ����IO��ʼ����Һ����ʼ��ǰҪ���ô˺���
//�����������
//����ֵ��  ��
//******************************************************************
void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF, ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;	   //GPIO_Pin_10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE, GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7);

	//PF0~15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_SetBits(GPIOF,GPIO_Pin_All);
}

//******************************************************************
//��������  LCD_Reset
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    LCD��λ������Һ����ʼ��ǰҪ���ô˺���
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_RESET(void)
{
	LCD_RST_SET;
	delay_ms(100);
	LCD_RST_CLR;
	delay_ms(500);	
	LCD_RST_SET;
	delay_ms(200);
}
 	 
//******************************************************************
//��������  LCD_Init
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    LCD��ʼ��
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_Init(void)
{  
     										 
	LCD_GPIOInit();
 	LCD_RESET();//Ӳ����λ
	//============ OTM8009A+HSD3.97 20140613 ===============================================//
	LCD_WR_REG(0xff00);   LCD_WR_DATA(0x80);		//enable access command2
	LCD_WR_REG(0xff01);   LCD_WR_DATA(0x09);		//enable access command2
	LCD_WR_REG(0xff02);   LCD_WR_DATA(0x01);		//enable access command2
	LCD_WR_REG(0xff80);   LCD_WR_DATA(0x80);		//enable access command2
	LCD_WR_REG(0xff81);   LCD_WR_DATA(0x09);		//enable access command2
	LCD_WR_REG(0xff03);   LCD_WR_DATA(0x01);		//
	LCD_WR_REG(0xc5b1);   LCD_WR_DATA(0xA9);		//power control

	LCD_WR_REG(0xc591);   LCD_WR_DATA(0x0F);               //power control
	LCD_WR_REG(0xc0B4);   LCD_WR_DATA(0x50);
			
	//panel driving mode : column inversion

	//////	gamma
	LCD_WR_REG(0xE100);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xE101);   LCD_WR_DATA(0x09);
	LCD_WR_REG(0xE102);   LCD_WR_DATA(0x0F);
	LCD_WR_REG(0xE103);   LCD_WR_DATA(0x0E);
	LCD_WR_REG(0xE104);   LCD_WR_DATA(0x07);
	LCD_WR_REG(0xE105);   LCD_WR_DATA(0x10);
	LCD_WR_REG(0xE106);   LCD_WR_DATA(0x0B);
	LCD_WR_REG(0xE107);   LCD_WR_DATA(0x0A);
	LCD_WR_REG(0xE108);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xE109);   LCD_WR_DATA(0x07);
	LCD_WR_REG(0xE10A);   LCD_WR_DATA(0x0B);
	LCD_WR_REG(0xE10B);   LCD_WR_DATA(0x08);
	LCD_WR_REG(0xE10C);   LCD_WR_DATA(0x0F);
	LCD_WR_REG(0xE10D);   LCD_WR_DATA(0x10);
	LCD_WR_REG(0xE10E);   LCD_WR_DATA(0x0A);
	LCD_WR_REG(0xE10F);   LCD_WR_DATA(0x01);	
	LCD_WR_REG(0xE200);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xE201);   LCD_WR_DATA(0x09);
	LCD_WR_REG(0xE202);   LCD_WR_DATA(0x0F);
	LCD_WR_REG(0xE203);   LCD_WR_DATA(0x0E);
	LCD_WR_REG(0xE204);   LCD_WR_DATA(0x07);
	LCD_WR_REG(0xE205);   LCD_WR_DATA(0x10);
	LCD_WR_REG(0xE206);   LCD_WR_DATA(0x0B);
	LCD_WR_REG(0xE207);   LCD_WR_DATA(0x0A);
	LCD_WR_REG(0xE208);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xE209);   LCD_WR_DATA(0x07);
	LCD_WR_REG(0xE20A);   LCD_WR_DATA(0x0B);
	LCD_WR_REG(0xE20B);   LCD_WR_DATA(0x08);
	LCD_WR_REG(0xE20C);   LCD_WR_DATA(0x0F);
	LCD_WR_REG(0xE20D);   LCD_WR_DATA(0x10);
	LCD_WR_REG(0xE20E);   LCD_WR_DATA(0x0A);
	LCD_WR_REG(0xE20F);   LCD_WR_DATA(0x01);	
	LCD_WR_REG(0xD900);   LCD_WR_DATA(0x4E);		//vcom setting

	LCD_WR_REG(0xc181);   LCD_WR_DATA(0x66);		//osc=65HZ

	LCD_WR_REG(0xc1a1);   LCD_WR_DATA(0x08);		
	LCD_WR_REG(0xc592);   LCD_WR_DATA(0x01);		//power control

	LCD_WR_REG(0xc595);   LCD_WR_DATA(0x34);		//power control

	LCD_WR_REG(0xd800);   LCD_WR_DATA(0x79);		//GVDD / NGVDD setting

	LCD_WR_REG(0xd801);   LCD_WR_DATA(0x79);		//GVDD / NGVDD setting

	LCD_WR_REG(0xc594);   LCD_WR_DATA(0x33);		//power control

	LCD_WR_REG(0xc0a3);   LCD_WR_DATA(0x1B);       //panel timing setting
	LCD_WR_REG(0xc582);   LCD_WR_DATA(0x83);		//power control

	LCD_WR_REG(0xc481);   LCD_WR_DATA(0x83);		//source driver setting

	LCD_WR_REG(0xc1a1);   LCD_WR_DATA(0x0E);
	LCD_WR_REG(0xb3a6);   LCD_WR_DATA(0x20);
	LCD_WR_REG(0xb3a7);   LCD_WR_DATA(0x01);
	LCD_WR_REG(0xce80);   LCD_WR_DATA(0x85);		// GOA VST

	LCD_WR_REG(0xce81);   LCD_WR_DATA(0x01); 	// GOA VST

	LCD_WR_REG(0xce82);   LCD_WR_DATA(0x00);		// GOA VST	

	LCD_WR_REG(0xce83);   LCD_WR_DATA(0x84);		// GOA VST
	LCD_WR_REG(0xce84);   LCD_WR_DATA(0x01);		// GOA VST
	LCD_WR_REG(0xce85);   LCD_WR_DATA(0x00);		// GOA VST

	LCD_WR_REG(0xcea0);   LCD_WR_DATA(0x18);		// GOA CLK
	LCD_WR_REG(0xcea1);   LCD_WR_DATA(0x04);		// GOA CLK
	LCD_WR_REG(0xcea2);   LCD_WR_DATA(0x03);		// GOA CLK 
	LCD_WR_REG(0xcea3);   LCD_WR_DATA(0x39);		// GOA CLK 
	LCD_WR_REG(0xcea4);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xcea5);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xcea6);   LCD_WR_DATA(0x00);		// GOA CLK
	LCD_WR_REG(0xcea7);   LCD_WR_DATA(0x18);		// GOA CLK 
	LCD_WR_REG(0xcea8);   LCD_WR_DATA(0x03);		// GOA CLK
	LCD_WR_REG(0xcea9);   LCD_WR_DATA(0x03);		// GOA CLK 
	LCD_WR_REG(0xceaa);   LCD_WR_DATA(0x3a);		// GOA CLK
	LCD_WR_REG(0xceab);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xceac);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xcead);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xceb0);   LCD_WR_DATA(0x18);		// GOA CLK
	LCD_WR_REG(0xceb1);   LCD_WR_DATA(0x02);		// GOA CLK 
	LCD_WR_REG(0xceb2);   LCD_WR_DATA(0x03); 		// GOA CLK
	LCD_WR_REG(0xceb3);   LCD_WR_DATA(0x3b);		// GOA CLK 
	LCD_WR_REG(0xceb4);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xceb5);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xceb6);   LCD_WR_DATA(0x00);		// GOA CLK
	LCD_WR_REG(0xceb7);   LCD_WR_DATA(0x18);		// GOA CLK
	LCD_WR_REG(0xceb8);   LCD_WR_DATA(0x01);		// GOA CLK 
	LCD_WR_REG(0xceb9);   LCD_WR_DATA(0x03);		// GOA CLK 
	LCD_WR_REG(0xceba);   LCD_WR_DATA(0x3c);		// GOA CLK 
	LCD_WR_REG(0xcebb);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xcebc);   LCD_WR_DATA(0x00);		// GOA CLK 
	LCD_WR_REG(0xcebd);   LCD_WR_DATA(0x00);		// GOA CLK
	LCD_WR_REG(0xcfc0);   LCD_WR_DATA(0x01);		// GOA ECLK 
	LCD_WR_REG(0xcfc1);   LCD_WR_DATA(0x01);		// GOA ECLK
	LCD_WR_REG(0xcfc2);   LCD_WR_DATA(0x20); 		// GOA ECLK

	LCD_WR_REG(0xcfc3);   LCD_WR_DATA(0x20); 		// GOA ECLK

	LCD_WR_REG(0xcfc4);   LCD_WR_DATA(0x00); 		// GOA ECLK

	LCD_WR_REG(0xcfc5);   LCD_WR_DATA(0x00); 		// GOA ECLK

	LCD_WR_REG(0xcfc6);   LCD_WR_DATA(0x01); 		// GOA other options

	LCD_WR_REG(0xcfc7);   LCD_WR_DATA(0x00); 
				
	// GOA signal toggle option setting

	LCD_WR_REG(0xcfc8);   LCD_WR_DATA(0x00); 		//GOA signal toggle option setting
	LCD_WR_REG(0xcfc9);   LCD_WR_DATA(0x00); 
		 
	//GOA signal toggle option setting

	LCD_WR_REG(0xcfd0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb80);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb81);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb82);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb83);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb84);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb85);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb86);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb87);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb88);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb89);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb90);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb91);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb92);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb93);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb94);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb95);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb96);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb97);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb98);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb99);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb9a);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb9b);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb9c);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb9d);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcb9e);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba1);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba2);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba3);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba4);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba5);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcba9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbaa);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbab);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbac);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbad);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbae);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb1);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb2);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb3);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb4);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb5);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbb9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbc0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbc1);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbc2);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbc3);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbc4);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbc5);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbc6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbc7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbc8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbc9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbca);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbcb);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbcc);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbcd);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbce);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbd0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbd1);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbd2);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbd3);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbd4);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbd5);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbd6);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbd7);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbd8);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbd9);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbda);   LCD_WR_DATA(0x04);
	LCD_WR_REG(0xcbdb);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbdc);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbdd);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbde);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe1);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe2);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe3);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe4);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe5);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbe9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcbf0);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf1);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf2);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf3);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf4);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf5);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf6);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf7);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf8);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcbf9);   LCD_WR_DATA(0xFF);
	LCD_WR_REG(0xcc80);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc81);   LCD_WR_DATA(0x26);
	LCD_WR_REG(0xcc82);   LCD_WR_DATA(0x09);
	LCD_WR_REG(0xcc83);   LCD_WR_DATA(0x0B);
	LCD_WR_REG(0xcc84);   LCD_WR_DATA(0x01);
	LCD_WR_REG(0xcc85);   LCD_WR_DATA(0x25);
	LCD_WR_REG(0xcc86);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc87);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc88);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc89);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc90);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc91);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc92);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc93);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc94);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc95);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc96);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc97);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc98);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc99);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc9a);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcc9b);   LCD_WR_DATA(0x26);
	LCD_WR_REG(0xcc9c);   LCD_WR_DATA(0x0A);
	LCD_WR_REG(0xcc9d);   LCD_WR_DATA(0x0C);
	LCD_WR_REG(0xcc9e);   LCD_WR_DATA(0x02);
	LCD_WR_REG(0xcca0);   LCD_WR_DATA(0x25);
	LCD_WR_REG(0xcca1);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca2);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca3);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca4);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca5);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcca9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccaa);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccab);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccac);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccad);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccae);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccb0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccb1);   LCD_WR_DATA(0x25);
	LCD_WR_REG(0xccb2);   LCD_WR_DATA(0x0C);
	LCD_WR_REG(0xccb3);   LCD_WR_DATA(0x0A);
	LCD_WR_REG(0xccb4);   LCD_WR_DATA(0x02);
	LCD_WR_REG(0xccb5);   LCD_WR_DATA(0x26);
	LCD_WR_REG(0xccb6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccb7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccb8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccb9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc0);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc1);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc2);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc3);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc4);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc5);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccc9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccca);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xcccb);   LCD_WR_DATA(0x25);
	LCD_WR_REG(0xcccc);   LCD_WR_DATA(0x0B);
	LCD_WR_REG(0xcccd);   LCD_WR_DATA(0x09);
	LCD_WR_REG(0xccce);   LCD_WR_DATA(0x01);
	LCD_WR_REG(0xccd0);   LCD_WR_DATA(0x26);
	LCD_WR_REG(0xccd1);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd2);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd3);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd4);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd5);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd6);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd7);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd8);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccd9);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccda);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccdb);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccdc);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccdd);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0xccde);   LCD_WR_DATA(0x00);
	LCD_WR_REG(0x3A00);   LCD_WR_DATA(0x55);

	LCD_WR_REG(0x1100);
	delay_ms(100);
	LCD_WR_REG(0x2900);
	delay_ms(50);
	LCD_WR_REG(0x2C00);
	LCD_WR_REG(0x2A00);  LCD_WR_DATA(0x00);
	LCD_WR_REG(0x2A01);  LCD_WR_DATA(0x00);
	LCD_WR_REG(0x2A02);  LCD_WR_DATA(0x01);
	LCD_WR_REG(0x2A03);  LCD_WR_DATA(0xe0);
	LCD_WR_REG(0x2B00);  LCD_WR_DATA(0x00);
	LCD_WR_REG(0x2B01);  LCD_WR_DATA(0x00);
	LCD_WR_REG(0x2B02);  LCD_WR_DATA(0x03);
	LCD_WR_REG(0x2B03);  LCD_WR_DATA(0x20);

	LCD_SetParam();//����LCD����
	//LCD_LED=1;//��������
	LCD_Clear(WHITE);
}
  		  
/*************************************************
��������LCD_SetWindows
���ܣ�����lcd��ʾ���ڣ��ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{
	LCD_WR_REG(0x2A00);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_REG(0x2A01);	
	LCD_WR_DATA(xStar);
	
	LCD_WR_REG(0x2A02);	
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_REG(0x2A03);	
	LCD_WR_DATA(xEnd);
		
	LCD_WR_REG(0x2B00);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_REG(0x2B01);	
	LCD_WR_DATA(yStar);
	
	LCD_WR_REG(0x2B02);	
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_REG(0x2B03);	
	LCD_WR_DATA(yEnd);

	LCD_WriteRAM_Prepare();	//��ʼд��GRAM				
}   

/*************************************************
��������LCD_SetCursor
���ܣ����ù��λ��
��ڲ�����xy����
����ֵ����
*************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

//����LCD����
//������к�����ģʽ�л�
void LCD_SetParam(void)
{ 	
	lcddev.width=480;
	lcddev.height=800;
	lcddev.wramcmd=0x2c00;
	/*
#if USE_HORIZONTAL==1	//ʹ�ú���	  
	lcddev.dir=1;//����
	lcddev.width=800;
	lcddev.height=480;
			
	LCD_WriteReg(0x3600,0x65);


#else//����
	lcddev.dir=0;//����				 	 		
	lcddev.width=480;
	lcddev.height=800;
	
	LCD_WriteReg(0x3600,0x00);
#endif*/
}	

