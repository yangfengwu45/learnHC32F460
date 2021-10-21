#define mcu_cfg_c_

#include "mcu_cfg.h"


void mcu_cfg_sys_clk_init(void)
{
	en_clk_sys_source_t     enSysClkSrc;
	stc_clk_sysclk_cfg_t    stcSysClkCfg;
	stc_clk_xtal_cfg_t      stcXtalCfg;
	stc_clk_mpll_cfg_t      stcMpllCfg;

	MEM_ZERO_STRUCT(enSysClkSrc);
	MEM_ZERO_STRUCT(stcSysClkCfg);
	MEM_ZERO_STRUCT(stcXtalCfg);
	MEM_ZERO_STRUCT(stcMpllCfg);

	/* Set bus clk div. */
	stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;   // Max 168MHz
	stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;  // Max 84MHz
	stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // Max 168MHz
	stcSysClkCfg.enPclk1Div = ClkSysclkDiv4;  // Max 42MHz
	stcSysClkCfg.enPclk2Div = ClkSysclkDiv4;  // Max 60MHz
	stcSysClkCfg.enPclk3Div = ClkSysclkDiv4;  // Max 42MHz
	stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  // Max 84MHz
	CLK_SysClkConfig(&stcSysClkCfg);

	/* Switch system clock source to MPLL. */
	/* Use Xtal as MPLL source. */
	stcXtalCfg.enMode = ClkXtalModeOsc;//选择外部高速时钟

	/*根据高速时钟的频率设置这个参数
	ClkXtalHighDrv                  = 0u,   ///< High drive capability.20MHz~24MHz.
	ClkXtalMidDrv                   = 1u,   ///< Middle drive capability.16MHz~20MHz.
	ClkXtalLowDrv                   = 2u,   ///< Low drive capability.8MHz~16MHz.
	ClkXtalTinyDrv                  = 3u,   ///< Tiny drive capability.8MHz.
	*/
	stcXtalCfg.enDrv = ClkXtalLowDrv;//我使用的8M的
	stcXtalCfg.enFastStartup = Enable;//超高速驱动(禁止的话可以低功耗)
	CLK_XtalConfig(&stcXtalCfg);//配置
	CLK_XtalCmd(Enable);//使能

	/* MPLL config. */
	stcMpllCfg.pllmDiv = 1u;//时钟进到MPLL里面的时候不分频
	stcMpllCfg.plln =42u;//时钟进到MPLL之前倍频42倍(8*42=336MHz)
	stcMpllCfg.PllpDiv = 2u;//设置p这一路分频2(336/2=168MHz)
	stcMpllCfg.PllqDiv = 2u;//设置q这一路分频2(336/2=168MHz)
	stcMpllCfg.PllrDiv = 2u;//设置r这一路分频2(336/2=168MHz)

	CLK_SetPllSource(ClkPllSrcXTAL);//
	CLK_MpllConfig(&stcMpllCfg);

	/* flash read wait cycle setting */
	EFM_Unlock();
	EFM_SetLatency(EFM_LATENCY_5);
	EFM_Lock();

	/* Enable MPLL. */
	CLK_MpllCmd(Enable);

	/* Wait MPLL ready. */
	while(Set != CLK_GetFlagStatus(ClkFlagMPLLRdy))
	{
	;
	}

	/* Switch system clock source to MPLL. */
	CLK_SetSysClkSource(CLKSysSrcMPLL);
}













































