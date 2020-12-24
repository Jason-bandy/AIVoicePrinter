#ifndef __REG_AGC_H_
#define __REG_AGC_H_

#include "sys_config.h"

#define REG_AGC_SIZE 172

#if (CFG_SOC_NAME == SOC_BK7271)
#define REG_AGC_BASE_ADDR 0x00900000
#else
#define REG_AGC_BASE_ADDR 0x01000000
#endif

#endif // __REG_AGC_H_

