/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file 
/-----------------------------------------------------------------------*/
#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED
#if (CONFIG_APP_MP3PLAYER == 1)
#ifdef __cplusplus
extern "C" {
#endif


typedef uint8	DSTATUS;

typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

DSTATUS disk_initialize (uint8 pdrv);
DSTATUS disk_status (uint8 pdrv);
DRESULT disk_read (uint8 pdrv, uint8* buff, uint32 start_sector, uint32 sector_cnt);

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_APP_MP3PLAYER */
#endif
