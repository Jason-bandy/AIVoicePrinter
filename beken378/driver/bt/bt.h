#ifndef _BT_H_
#define _BT_H_

#define BT_DEBUG

#ifdef BT_DEBUG
#define BT_PRT      os_printf
#else
#define BT_PRT      null_prf
#endif

#define BK7271_SHAREMEM_BASE_ADDR          (0x04000000)
#define BK7271_SHAREMEM_CAPACITY           (128 * 1024)
#define BK7271_SHAREMEM_END_ADDR           (BK7271_SHAREMEM_BASE_ADDR + BK7271_SHAREMEM_CAPACITY)

#define HOST2CTRL_BUFFER_SIZE              (0x800)
#define HOST2CTRL_BUFFER_ADDR              (BK7271_SHAREMEM_END_ADDR - HOST2CTRL_BUFFER_SIZE)
#define CTRL2HOST_BUFFER_SIZE              (0x800)
#define CTRL2HOST_BUFFER_ADDR              (HOST2CTRL_BUFFER_ADDR - CTRL2HOST_BUFFER_SIZE)

#endif /* _BT_H_ */
