#ifndef mcu_cfg_h_
#define mcu_cfg_h_

#ifndef mcu_cfg_c_
#define mcu_cfg_cx_ extern
#else
#define mcu_cfg_cx_ 
#endif

#include "hc32_ddl.h"

void mcu_cfg_sys_clk_init(void);

#endif

