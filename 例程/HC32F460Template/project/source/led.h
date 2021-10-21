#ifndef led_h_
#define led_h_

#ifndef led_c_
#define led_cx_ extern
#else
#define led_cx_ 
#endif

#include "hc32_ddl.h"


void gpio_set_get(char setOrget, int value);

#endif

