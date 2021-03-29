/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _PORT_GENERIC_H_
#define _PORT_GENERIC_H_
#include <stdio.h>
#include "typedef.h"
#include "str_pub.h"
#include "uart_pub.h"

#define os_printf  bk_printf

result_t os_port_init(void);
void os_port_uninit(void);

#endif
// eof

