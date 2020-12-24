#ifndef __REG_MAC_CORE_H_
#define __REG_MAC_CORE_H_

#include "sys_config.h"

#define REG_MAC_CORE_SIZE         1376

#if (CFG_SOC_NAME == SOC_BK7271)
#define REG_MAC_CORE_BASE_ADDR 	  0x00A00000
#else
#define REG_MAC_CORE_BASE_ADDR    0xC0000000
#endif

#endif // __REG_MAC_CORE_H_
// eof

