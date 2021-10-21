#define led_c_

#include "led.h"


/*���ÿ��Ƶ�����*/
#define  LED_GPIO_PORT        (PortC) //PC��
#define  LED_GPIO_PIN         (Pin13) //PC13

void led_init(void)
{
	/*����IO�Ľṹ��*/
	stc_port_init_t stcPortInit;
	/*����ṹ�����*/
	MEM_ZERO_STRUCT(stcPortInit);
	
	/*���ò���*/
	stcPortInit.enPinMode = Pin_Mode_Out;//���ģʽ
	
	/*��������*/
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
	


