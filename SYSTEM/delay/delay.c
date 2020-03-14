#include "delay.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#include "task.h"
#endif

static u8  fac_us=0;		//us延时倍乘数			   
static u16 fac_ms=0;		//ms延时倍乘数,在ucos下,代表每个节拍的ms数
extern void xPortSysTickHandler(void);

//systick中断服务函数,使用ucos时用到
void SysTick_Handler(void)
{	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED){
		xPortSysTickHandler();
	}
}
			   
//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	fac_us = SystemCoreClock/1000000;
	reload = SystemCoreClock/1000000;
	reload *= 1000000/configTICK_RATE_HZ;
	
	fac_ms = 1000/configTICK_RATE_HZ;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->LOAD = reload;
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}								    

//延时nus
//nus为要延时的us数.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//LOAD的值	    	 
	ticks=nus*fac_us; 							//需要的节拍数	  		 
	told=SysTick->VAL;        					//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told) tcnt+=told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks) break;				//时间超过/等于要延迟的时间,则退出.
		}  
	};								    
}
//延时nms
//nms:要延时的ms数
void delay_ms(u32 nms)
{	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		if(nms >= fac_ms){
			vTaskDelay(nms/fac_ms);
		}
		nms %= fac_ms;
	}
	delay_us((u32)(nms*1000));
}

void delay_xms(u32 nms)
{
	u32 i;
	for(i = 0; i < nms; i++)
		delay_us(1000);
}
