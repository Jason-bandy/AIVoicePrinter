#ifndef __REG_MDM_STAT_H_
#define __REG_MDM_STAT_H_

#include "sys_config.h"

#define REG_MDM_STAT_SIZE 108

#if (CFG_SOC_NAME == SOC_BK7271)
#define REG_MDM_STAT_BASE_ADDR 0x00900000
#else
#define REG_MDM_STAT_BASE_ADDR 0x01000000
#endif

#endif // __REG_MDM_STAT_H_

