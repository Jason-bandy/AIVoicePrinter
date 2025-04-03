// Copyright 2015-2024 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "include.h"
#include "rtos_pub.h"
#include "BkDriverFlash.h"
#include "flash_pub.h"
#include "drv_model_pub.h"
#include "error.h"
#include "uart_pub.h"
#include "mem_pub.h"
#include "BkFlashPartition.h"

static beken_mutex_t hal_flash_mutex;
const bk_logic_partition_t *bk7231_partitions;

#if (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_DYNAMIC)
// Can remove this if don't care OTA
#warning "if CFG_FLASH_SELECTION_TYPE = FLASH_SELECTION_TYPE_DYNAMIC, this may not select right config in bootloader, it may be cuase OTA failed!!!"
#endif

static void BkFlashPartitionAssert( bk_partition_t inPartition )
{
    ASSERT(BK_PARTITION_BOOTLOADER < BK_PARTITION_MAX);
}

static uint32_t BkFlashPartitionIsValid( bk_partition_t inPartition )
{
    if((inPartition >= BK_PARTITION_BOOTLOADER)&&(inPartition < BK_PARTITION_MAX))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bk_logic_partition_t *bk_flash_get_info( bk_partition_t inPartition )
{
    bk_logic_partition_t *pt = NULL;

    BkFlashPartitionAssert(inPartition);

    if(BkFlashPartitionIsValid(inPartition))
    {
        pt = (bk_logic_partition_t *)&bk7231_partitions[inPartition];
    }
    else
        pt =	NULL;
    return pt;
}

OSStatus BkFlashInit(void)
{
    return 0;
}

OSStatus BkFlashUninit(void)
{
    return 0;
}

OSStatus bk_flash_erase(bk_partition_t inPartition, uint32_t off_set, uint32_t size)
{
    uint32_t start_addr;
    bk_logic_partition_t *partition_info;

    partition_info = bk_flash_get_info(inPartition);
    if (NULL == partition_info)
    {
        os_printf("%s partiion not found\r\n", __FUNCTION__);
        return kNotFoundErr;
    }
    start_addr = partition_info->partition_start_addr + off_set;

    return bk_flash_abs_addr_erase(start_addr, size);
}

OSStatus bk_flash_write( bk_partition_t inPartition, volatile uint32_t off_set, uint8_t *inBuffer, uint32_t inBufferLength)
{
    uint32_t start_addr;
    bk_logic_partition_t *partition_info;

    if (NULL == inBuffer)
    {
        os_printf("%s inBuffer=NULL\r\n", __FUNCTION__);
        return kParamErr;
    }

    partition_info = bk_flash_get_info(inPartition);
    if (NULL == partition_info)
    {
        os_printf("%s partiion not found\r\n", __FUNCTION__);
        return kNotFoundErr;
    }

    start_addr = partition_info->partition_start_addr + off_set;

    return bk_flash_abs_addr_write(start_addr, inBuffer, inBufferLength, 0);
}

OSStatus bk_flash_read( bk_partition_t inPartition, volatile uint32_t off_set, uint8_t *outBuffer, uint32_t inBufferLength)
{
    uint32_t start_addr;
    bk_logic_partition_t *partition_info;

    if (NULL == outBuffer)
    {
        os_printf("%s outBuffer=NULL\r\n", __FUNCTION__);
        return kParamErr;
    }

    partition_info = bk_flash_get_info(inPartition);
    if (NULL == partition_info)
    {
        os_printf("%s partiion not found\r\n", __FUNCTION__);
        return kNotFoundErr;
    }

    start_addr = partition_info->partition_start_addr + off_set;

    return bk_flash_abs_addr_read(start_addr, outBuffer, inBufferLength);
}

uint32_t bk_flash_read_otp(uint32_t off_set, uint8_t *out_buf, uint32_t buf_len)
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    flash_otp_t otp_cfg;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    if (DD_HANDLE_UNVALID == flash_hdl) {
        os_printf("%s open failed\r\n", __FUNCTION__);
        return kOpenErr;
    }

    otp_cfg.buf = out_buf;
    otp_cfg.addr = off_set;
    otp_cfg.len = buf_len;
    return ddev_control(flash_hdl, CMD_FLASH_READ_OTP, (void *)&otp_cfg);
}

uint32_t bk_flash_get_uid(uint8_t *uid_buf, uint32_t uid_buf_len)
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    flash_otp_t otp_cfg;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    if (DD_HANDLE_UNVALID == flash_hdl) {
        os_printf("%s open failed\r\n", __FUNCTION__);
        return kOpenErr;
    }

    otp_cfg.buf = uid_buf;
    otp_cfg.addr = 0;
    otp_cfg.len = uid_buf_len;
    return ddev_control(flash_hdl, CMD_FLASH_GET_UID, (void *)&otp_cfg);
}

OSStatus bk_flash_enable_security(PROTECT_TYPE type )
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t param = type;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    if (DD_HANDLE_UNVALID == flash_hdl)
    {
        os_printf("%s open failed\r\n", __FUNCTION__);
        return kOpenErr;
    }
    ddev_control(flash_hdl, CMD_FLASH_SET_PROTECT, (void *)&param);

    return kNoErr;
}

OSStatus bk_flash_get_security(PROTECT_TYPE *protect_flag)
{
    DD_HANDLE flash_hdl;
    UINT32 status, param;

    ASSERT(protect_flag != NULL);

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_GET_PROTECT, &param);
    *protect_flag = param;

    return kNoErr;
}

int bk_flash_abs_addr_read(unsigned int off_set, unsigned char *outBuffer, unsigned int size)
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    GLOBAL_INT_DECLARATION();

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);

    GLOBAL_INT_DISABLE();
    ddev_read(flash_hdl, (char *)outBuffer, size, off_set);
    GLOBAL_INT_RESTORE();

    return kNoErr;
}

int bk_flash_abs_addr_erase(unsigned int flashOffset, unsigned int size)
{
    UINT32 status, i;
    PROTECT_TYPE flag;
    DD_HANDLE flash_hdl;
    UINT32 start_sector, end_sector, param;
    GLOBAL_INT_DECLARATION();

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);

    bk_flash_get_security(&flag);
    start_sector = flashOffset >> 12;
    end_sector = (flashOffset + size - 1) >> 12;

    GLOBAL_INT_DISABLE();
    bk_flash_enable_security(FLASH_PROTECT_NONE);
    for (i = start_sector; i <= end_sector; i ++) {
        param = i << 12;
        ddev_control(flash_hdl, CMD_FLASH_ERASE_SECTOR, (void *)&param);
    }
    bk_flash_enable_security(flag);
    GLOBAL_INT_RESTORE();

    return kNoErr;
}

int bk_flash_abs_addr_write(unsigned int off_set, const unsigned char *inBuffer, unsigned int size, unsigned char eraseflag)
{
    UINT32 status;
    PROTECT_TYPE flag;
    DD_HANDLE flash_hdl;
    GLOBAL_INT_DECLARATION();

    bk_flash_get_security(&flag);

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);

    if (eraseflag)
        bk_flash_abs_addr_erase(off_set, size);

    GLOBAL_INT_DISABLE();
    bk_flash_enable_security(FLASH_PROTECT_NONE);
    ddev_write(flash_hdl, (char *)inBuffer, size, off_set);
    bk_flash_enable_security(flag);
    GLOBAL_INT_RESTORE();

    return kNoErr;
}

OSStatus test_flash_write(volatile uint32_t start_addr, uint32_t len)
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t i;
    u8 buf[256];
    uint32_t addr = start_addr;
    uint32_t length = len;
    uint32_t tmp = addr + length;

    for (i = 0; i < 256; i++)
        buf[i] = i;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    for (; addr < tmp; addr += 256) {
        os_printf("write addr(size:256):%d\r\n", addr);
        ddev_write(flash_hdl, (char *)buf, 256, addr);
    }

    return kNoErr;
}

OSStatus test_flash_erase(volatile uint32_t start_addr, uint32_t len)
{
    UINT32 status;
    DD_HANDLE flash_hdl;

    uint32_t addr = start_addr;
    uint32_t length = len;
    uint32_t tmp = addr + length;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    if (DD_HANDLE_UNVALID == flash_hdl) {
        os_printf("%s open failed\r\n", __FUNCTION__);
        return kOpenErr;
    }
    for (; addr < tmp; addr += 0x1000) {
        os_printf("erase addr:%d\r\n", addr);
        ddev_control(flash_hdl, CMD_FLASH_ERASE_SECTOR, (void *)&addr);

    }
    return kNoErr;
}

OSStatus test_flash_read(volatile uint32_t start_addr, uint32_t len)
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t i,j,tmp;
    u8 buf[256];
    uint32_t addr = start_addr;
    uint32_t length = len;
    tmp = addr+length;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    if (DD_HANDLE_UNVALID == flash_hdl)
    {
        os_printf("%s open failed\r\n", __FUNCTION__);
        return kOpenErr;
    }
    for(; addr<tmp; addr+=256)
    {
        os_memset(buf,0,256);
        ddev_read(flash_hdl, (char*)buf, 256, addr);
        os_printf("read addr:%x\r\n",addr);
        for(i=0; i<16; i++)
        {
            for(j=0; j<16; j++)
            {
                os_printf("%02x ",buf[i*16+j]);
            }
            os_printf("\r\n");
        }
    }

    return kNoErr;
}

OSStatus test_flash_read_without_print(volatile uint32_t start_addr, uint32_t len)
{
    UINT32 status;
    DD_HANDLE flash_hdl;
    uint32_t tmp;
    u8 buf[256];
    uint32_t addr = start_addr;
    uint32_t length = len;
    tmp = addr + length;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    if (DD_HANDLE_UNVALID == flash_hdl) {
        os_printf("%s open failed\r\n", __FUNCTION__);
        return kOpenErr;
    }
    for (; addr < tmp; addr += 256) {
        os_memset(buf, 0, 256);
        ddev_read(flash_hdl, (char *)buf, 256, addr);
    }

    return kNoErr;
}

OSStatus test_flash_read_time(volatile uint32_t start_addr, uint32_t len)
{
    UINT32 status, time_start, time_end;
    DD_HANDLE flash_hdl;
    uint32_t tmp;
    u8 buf[256];
    uint32_t addr = start_addr;
    uint32_t length = len;

    tmp = addr+length;

    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    if (DD_HANDLE_UNVALID == flash_hdl)
    {
        os_printf("%s open failed\r\n", __FUNCTION__);
        return kOpenErr;
    }
    beken_time_get_time((beken_time_t *)&time_start);
    os_printf("read time start:%d\r\n", time_start);

    for(; addr<tmp; addr+=256)
    {
        os_memset(buf,0,256);
        ddev_read(flash_hdl, (char*)buf, 256, addr);
    }
    beken_time_get_time((beken_time_t *)&time_end);
    os_printf("read time end:%d\r\n", time_end);
    os_printf("cost time:%d\r\n", time_end - time_start);

    return kNoErr;
}

int hal_flash_lock(void)
{
    rtos_lock_mutex(&hal_flash_mutex);
    return kNoErr;
}

int hal_flash_unlock(void)
{
    rtos_unlock_mutex(&hal_flash_mutex);
    return kNoErr;
}

int hal_flash_init(void)
{
    int ret = 0;

    #if (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_1M)
    bk7231_partitions = bk7231_partitions_1M;
    #elif (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_2M)
    bk7231_partitions = bk7231_partitions_2M;
    #elif (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_4M)
    bk7231_partitions = bk7231_partitions_4M;
    #elif (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_8M)
    bk7231_partitions = bk7231_partitions_8M;
    #else
    {
        DD_HANDLE flash_hdl;
        UINT32 status;

        /* Flash 26MHz clock select dco clock*/
        flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
        status = 0x200000;
        if (DD_HANDLE_UNVALID != flash_hdl)
        {
            ddev_control(flash_hdl, CMD_FLASH_GET_SIZE, &status);
        }
        if (status >= FLASH_SELECTION_TYPE_8M)
        {
            bk7231_partitions = bk7231_partitions_8M;
        }
        else if (status >= FLASH_SELECTION_TYPE_4M)
        {
            bk7231_partitions = bk7231_partitions_4M;
        }
        else if (status >= FLASH_SELECTION_TYPE_2M)
        {
            bk7231_partitions = bk7231_partitions_2M;
        }
        else if (status >= FLASH_SELECTION_TYPE_1M)
        {
            bk7231_partitions = bk7231_partitions_1M;
        }
        else
        {
            /* select minimal partitions and show an error */
            bk7231_partitions = bk7231_partitions_2M;
            os_printf("no partitions match with flash size 0x%x\r\n", status);
        }
    }
    #endif

    ret = rtos_init_mutex(&hal_flash_mutex);
    if (ret != 0)
        return kGeneralErr;
    return kNoErr;
}

// EOF

