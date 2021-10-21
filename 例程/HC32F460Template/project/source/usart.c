#define USART_C_

#include "usart.h"

/* USART1 baudrate definition */
#define USART1_BAUDRATE                  (115200ul)
/* USART1 TX Port/Pin definition */
#define USART1_TX_PORT                   (PortA)
#define USART1_TX_PIN                    (Pin09)
/* USART1 RX Port/Pin definition */
#define USART1_RX_PORT                   (PortA)
#define USART1_RX_PIN                    (Pin10)

/*串口1接收数据中断优先级*/
#define USART1_RI_IRQ_PRIORITY       (DDL_IRQ_PRIORITY_00)
/*串口1接收数据中断向量*/
#define USART1_RI_IRQ                (Int000_IRQn)


/*串口1接收数据错误中断优先级*/
#define USART1_EI_IRQ_PRIORITY       (DDL_IRQ_PRIORITY_00)
/*串口1数据接收错误中断向量*/
#define USART1_EI_IRQ                (Int001_IRQn)


/*******************************************************************************
 ** \brief USART RX irq callback function.//串口接收中断函数
 ** \param [in] None
 ** \retval None
 ******************************************************************************/
static void Usart1RxIrqCallback(void)
{
	uint16_t m_u16RxData;
	m_u16RxData = USART_RecData(M4_USART1);//获取串口接收的数据
}

/*******************************************************************************
 ** \brief USART RX error irq callback function.(串口接收错误中断处理函数)
 ** \param [in] None
 ** \retval None
 ******************************************************************************/
static void Usart1ErrIrqCallback(void)
{
	if (Set == USART_GetStatus(M4_USART1, UsartFrameErr)){ USART_ClearStatus(M4_USART1, UsartFrameErr);}
	else{}

	if (Set == USART_GetStatus(M4_USART1, UsartParityErr)) {USART_ClearStatus(M4_USART1, UsartParityErr);}
	else{}

	if (Set == USART_GetStatus(M4_USART1, UsartOverrunErr)) {USART_ClearStatus(M4_USART1, UsartOverrunErr);}
	else{}
}

/*******************************************************************************
 ** \brief 串口初始化
 ** \param [in] None
 ** \retval None
 ******************************************************************************/
void usart_init(void)
{
	en_result_t enRet = Ok;
	stc_irq_regi_conf_t stcIrqRegiCfg;
	
	/* Enable peripheral clock *//*打开时钟*/
	PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_USART1 | PWC_FCG1_PERIPH_USART2 | \
	PWC_FCG1_PERIPH_USART3 | PWC_FCG1_PERIPH_USART4, Enable);
	
	/* Initialize USART IO */ /*配置相应的IO作为串口的TX,RX引脚*/
	PORT_SetFunc(USART1_RX_PORT, USART1_RX_PIN, Func_Usart1_Rx, Disable);
	PORT_SetFunc(USART1_TX_PORT, USART1_TX_PIN, Func_Usart1_Tx, Disable);
	
	/*配置串口使用的时钟和基本通信配置*/
	const stc_usart_uart_init_t stcInitCfg = {
		UsartIntClkCkNoOutput, //使用内部时钟源,不需要在其时钟输出IO上输出通信的时钟信号
		UsartClkDiv_1,         //时钟不分频
		UsartDataBits8,        //一个字节数据用8位数据位表示
		UsartDataLsbFirst,     //先传输低位
		UsartOneStopBit,       //停止位1位
		UsartParityNone,       //无奇偶校验
		UsartSamleBit8,        //每次传输8位(1字节),也可以传输 UsartSamleBit16(16位,2字节)
		UsartStartBitFallEdge, 
		UsartRtsEnable,        //使能RTS (串口开始传输前让RTS输出一个高脉冲信号)
	};
	/* Initialize UART *//*初始化串口配置*/
	enRet = USART_UART_Init(M4_USART1, &stcInitCfg);
	if (enRet != Ok)while (1);
	/* Set baudrate *//*设置串口波特率*/
	enRet = USART_SetBaudrate(M4_USART1, USART1_BAUDRATE);
	if (enRet != Ok)while (1);

	/* Set USART RX IRQ *//*设置串口接收中断*/
	stcIrqRegiCfg.enIRQn = USART1_RI_IRQ;//设置中断向量
	stcIrqRegiCfg.pfnCallback = &Usart1RxIrqCallback;//设置中断回调函数
	stcIrqRegiCfg.enIntSrc = INT_USART1_RI;//中断名称(串口1接收中断)
	enIrqRegistration(&stcIrqRegiCfg);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, USART1_RI_IRQ_PRIORITY);//设置中断优先级
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
	
	/* Set USART RX error IRQ *//*设置串口接收错误中断*/
	stcIrqRegiCfg.enIRQn = USART1_EI_IRQ;//设置中断向量
	stcIrqRegiCfg.pfnCallback = &Usart1ErrIrqCallback;
	stcIrqRegiCfg.enIntSrc = INT_USART1_EI;//中断名称(串口1接收错误中断)
	enIrqRegistration(&stcIrqRegiCfg);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, USART1_EI_IRQ_PRIORITY);//设置中断优先级
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	/*Enable RX && RX interupt function && UsartTx*/
	USART_FuncCmd(M4_USART1, UsartRx, Enable);//使能接收
	USART_FuncCmd(M4_USART1, UsartRxInt, Enable);//使能接收中断
	USART_FuncCmd(M4_USART1, UsartTx, Enable);//使能发送
}











































