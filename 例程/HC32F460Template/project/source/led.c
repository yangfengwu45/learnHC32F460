#define led_c_

#include "led.h"


/*设置控制的引脚*/
#define  LED_GPIO_PORT        (PortC) //PC口
#define  LED_GPIO_PIN         (Pin13) //PC13

void led_init(void)
{
	/*配置IO的结构体*/
	stc_port_init_t stcPortInit;
	/*清零结构体变量*/
	MEM_ZERO_STRUCT(stcPortInit);
	
	/*配置参数*/
	stcPortInit.enPinMode = Pin_Mode_Out;//输出模式
	
	/*配置引脚*/
	PORT_Init(LED_GPIO_PORT, LED_GPIO_PIN, &stcPortInit);
}


void gpio_set_get(char setOrget, int value)
{
	static char f=0;
	if(f==0)
	{
		f=1;
		led_init();
	}
	if(setOrget==1)//set
	{
		switch(value)
		{
			case 1:
				PORT_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
			break;
			case 0:
				PORT_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
			break;
			case -1:
				PORT_Toggle(LED_GPIO_PORT, LED_GPIO_PIN);
			break;
		}
	}
	else if(setOrget==0) //get
	{
		PORT_GetBit(LED_GPIO_PORT,LED_GPIO_PIN);
	}
}
	


