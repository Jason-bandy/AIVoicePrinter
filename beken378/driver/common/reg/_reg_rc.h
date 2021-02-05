#ifndef __REG_RC_H_
#define __REG_RC_H_

#include "sys_config.h"

#define REG_RC_SIZE           428

#if (CFG_SOC_NAME == SOC_BK7271)
#define REG_RC_BASE_ADDR                0x00950000
#define REG_RC_BASE_ADDR_MSK            0x00FF0000
#define REG_RC_POWER_TABLE_ADDR         0

#define REG_TRX_BASE_ADDR               0x00950080
#elif (CFG_SOC_NAME == SOC_BK7236)
#define REG_RC_BASE_ADDR                0x0100C000
#define REG_RC_BASE_ADDR_MSK            0x0FFFF000
#define REG_RC_POWER_TABLE_ADDR         0x0100C400

#define REG_TRX_BASE_ADDR               0x0100C200
#else
#define REG_RC_BASE_ADDR                0x01050000
#define REG_RC_BASE_ADDR_MSK            0x0FFF0000
#define REG_RC_POWER_TABLE_ADDR         0x01050200
#define REG_TRX_BASE_ADDR               0x01050080
#endif

#endif // __REG_RC_H_

