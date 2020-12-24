#ifndef _DSP__H_
#define _DSP__H_
#include "uart_pub.h"

#define DSP_DEBUG        1

#if DSP_DEBUG
#define DSP_PRT      os_printf
#define DSP_WARN     warning_prf
#define DSP_FATAL    fatal_prf
#else
#define DSP_PRT      null_prf
#define DSP_WARN     null_prf
#define DSP_FATAL    null_prf
#endif

#endif //_DSP__H_

