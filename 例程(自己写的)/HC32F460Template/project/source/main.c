#include "hc32_ddl.h"
#include "mcu_cfg.h"
#include "timer0.h"

#include "led.h"
#include "usart.h"


int32_t main(void)
{
	
	mcu_cfg_sys_clk_init();//初始化时钟
	
	timer0A_init();
	
	usart_init();
	while(1)
	{
		Ddl_Delay1ms(500);
		//发送字符1
		USART_SendData(M4_USART1, '1');
		//等待串口发送完成
		while (Reset == USART_GetStatus(M4_USART1, UsartTxEmpty));
	}
}
