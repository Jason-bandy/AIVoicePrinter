#ifndef __REG_MAC_PL_H_
#define __REG_MAC_PL_H_

#include "sys_config.h"

#define REG_MAC_PL_SIZE      1404

#if (CFG_SOC_NAME == SOC_BK7271)
#define REG_MAC_PL_BASE_ADDR 0x00A08000
#else
#define REG_MAC_PL_BASE_ADDR 0xC0008000
#endif

#endif // __REG_MAC_PL_H_

