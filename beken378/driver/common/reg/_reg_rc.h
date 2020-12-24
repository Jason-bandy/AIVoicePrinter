#ifndef __REG_RC_H_
#define __REG_RC_H_

#include "sys_config.h"

#define REG_RC_SIZE           428

#if (CFG_SOC_NAME == SOC_BK7271)
#define REG_RC_BASE_ADDR      0x00950000
#else
#define REG_RC_BASE_ADDR      0x01050000
#endif

#endif // __REG_RC_H_

