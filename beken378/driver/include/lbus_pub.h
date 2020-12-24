#ifndef _LBUS_PUB_H_
#define _LBUS_PUB_H_
#include "sys_config.h"

#if (SOC_BK7271 == CFG_SOC_NAME)
#define LBUS_BASE_ADDR                        (0x06000000)

#define LBUS_CONF0_REG                        (LBUS_BASE_ADDR + 0x180)
#define DSP_DOWNLOAD_ENABLE                      (1 << 0)
#define BT_DOWNLOAD_ENABLE                       (1 << 1)
#define DSP_RESET_ENABLE                         (1 << 2)
#define BT_RESET_ENABLE                          (1 << 3)

#define LBUS_CONF1_REG                        (LBUS_BASE_ADDR + 0x184)
#endif
#endif //_LBUS_PUB_H_
// eof

