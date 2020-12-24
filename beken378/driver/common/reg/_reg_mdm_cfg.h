#ifndef __REG_MDM_CFG_H_
#define __REG_MDM_CFG_H_

#include "sys_config.h"

#define REG_MDM_CFG_SIZE            152

#if (CFG_SOC_NAME == SOC_BK7271)
#define REG_MDM_CFG_BASE_ADDR       0x00900000
#else
#define REG_MDM_CFG_BASE_ADDR       0x01000000
#endif

#endif // __REG_MDM_CFG_H_
