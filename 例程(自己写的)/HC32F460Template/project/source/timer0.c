#define timer0_c_

#include "timer0.h"
#include "led.h"


/*定时器中断优先级*/
#define Timer0_INI_IRQ_PRIORITY      (DDL_IRQ_PRIORITY_15)
/*定时器中断向量*/
#define Timer0_INI_IRQ               (Int025_IRQn)


/* Define Timer Unit for example */
#define TMR_UNIT            (M4_TMR02)
#define TMR_INI_GCMA        (INT_TMR02_GCMA)
#define TMR_INI_GCMB        (INT_TMR02_GCMB)

#define ENABLE_TMR0()       (PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM02, Enable))

void Timer0A_CallBack(void);


/*******************************************************************************
 ** \brief 初始化(定时器0,A通道定时1ms)
 ** \param [in] None
 ** \retval None
 ******************************************************************************/
void timer0A_init(void)
{
	stc_tim0_base_init_t stcTimerCfg;
	stc_irq_regi_conf_t stcIrqRegiConf;
	
	uint32_t u32Pclk1;
	stc_clk_freq_t stcClkTmp;
	
	MEM_ZERO_STRUCT(stcTimerCfg);
	MEM_ZERO_STRUCT(stcIrqRegiConf);
	/*获取pclk1时钟频率 */
	CLK_GetClockFreq(&stcClkTmp);
	u32Pclk1 = stcClkTmp.pclk1Freq;
	//使能外部低速时钟(32.768K) XTAL32
	CLK_Xtal32Cmd(Enable);
	//接通定时器的时钟线
	ENABLE_TMR0();
	/*config register for channel A */
	#if 1 //同步计数
		stcTimerCfg.Tim0_CounterMode = Tim0_Sync;//同步计数模式
		stcTimerCfg.Tim0_SyncClockSource = Tim0_Pclk1;//选择时钟(时钟配置的是42M)
		stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv1024;//分频 (41015.625Hz)
		stcTimerCfg.Tim0_CmpValue = (uint16_t)(u32Pclk1/1024/1000 - 1);//(1ms)
	#else //异步计数
		stcTimerCfg.Tim0_CounterMode = Tim0_Async;
		stcTimerCfg.Tim0_AsyncClockSource = Tim0_XTAL32;//选择外部低速时钟(32.768K)
		stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv0;//分频
		stcTimerCfg.Tim0_CmpValue = (uint16_t)(32768/1000 - 1);//(1ms)
	#endif
	TIMER0_BaseInit(TMR_UNIT,Tim0_ChannelA,&stcTimerCfg);
	
	/* Enable channel A interrupt */
	TIMER0_IntCmd(TMR_UNIT,Tim0_ChannelA,Enable);
	/* Register TMR_INI_GCMA Int*/
	stcIrqRegiConf.enIRQn = Timer0_INI_IRQ;
	/* Select I2C Error or Event interrupt function */
	stcIrqRegiConf.enIntSrc = TMR_INI_GCMA;
	/* Callback function */
	stcIrqRegiConf.pfnCallback =&Timer0A_CallBack;
	/* Registration IRQ */
	enIrqRegistration(&stcIrqRegiConf);
	/* Clear Pending */
	NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
	/* Set priority */
	NVIC_SetPriority(stcIrqRegiConf.enIRQn, Timer0_INI_IRQ_PRIORITY);
	/* Enable NVIC */
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
	
	/*start timer0*/
	TIMER0_Cmd(TMR_UNIT,Tim0_ChannelA,Enable);
}


int timer0A_cnt=0;
void Timer0A_CallBack(void)
{
	timer0A_cnt++;
	if(timer0A_cnt>1000)
	{
		timer0A_cnt=0;
		gpio_set_get(1,-1);
	}
}







































