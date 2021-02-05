#ifndef _CONFIG_DEBUG_H_
#define _CONFIG_DEBUG_H_

/*Debug*/
#define DEBUG_BA22_EXCEPTION                   1
//#define DEBUG_WILD_POINTER                     1
#ifndef FATAL_PRT
#define FATAL_PRT(fmt,...) os_printf(fmt, ##__VA_ARGS__)
#endif
#define WNG_PRT(fmt,...)   os_printf(fmt, ##__VA_ARGS__)
#define INFO_PRT(fmt,...)  os_printf(fmt, ##__VA_ARGS__)

#endif

//EOF
