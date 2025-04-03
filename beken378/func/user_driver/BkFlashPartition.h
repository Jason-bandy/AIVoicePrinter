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

#ifndef CFG_FLASH_SELECTION_TYPE
#include "sys_config.h"
#endif

#ifndef __BKFLASHPARTITION_H__
#define __BKFLASHPARTITION_H__

/* Logic partition on flash devices */
#if (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_1M) || (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_DYNAMIC)
const bk_logic_partition_t bk7231_partitions_1M[BK_PARTITION_MAX] =
{
    [BK_PARTITION_BOOTLOADER] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Bootloader",
        .partition_start_addr      = 0x00000000,
        /* Boot size with crc should less than this length.
           Max boot size is 0x11000, but considering that the RF or usr partition may be placed
           before APP for save flash, So reserved 8K for this reason */
        .partition_length          = 0x0F000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_APPLICATION] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Application",
        .partition_start_addr      = 0x11000,
        .partition_length          = 0x94C00, // (560K data + 35K crc) = 595K
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_OTA] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "ota",
        .partition_start_addr      = 0xA6000, // 0x11000 + 0x94C00 = 0xA5C00, but should 4k aligned
        .partition_length          = 0x58000, // 352K, 352K/560K = 0.628. lzma larger than 0.6 is better
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_RF_FIRMWARE] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "RF Firmware",
        .partition_start_addr      = 0xFE000,// for rf related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_NET_PARAM] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "NET info",
        .partition_start_addr      = 0xFF000,// for net related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
};
#endif

#if (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_2M) || (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_DYNAMIC)
const bk_logic_partition_t bk7231_partitions_2M[BK_PARTITION_MAX] =
{
    [BK_PARTITION_BOOTLOADER] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Bootloader",
        .partition_start_addr      = 0x00000000,
        /* Boot size with crc should less than this length.
           Max boot size is 0x11000, but considering that the RF or usr partition may be placed
           before APP for save flash, So reserved 8K for this reason */
        .partition_length          = 0x0F000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_APPLICATION] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Application",
        .partition_start_addr      = 0x11000,
        /* 0x121000 = 1156K = 1088K data + 68K crc */
        .partition_length          = 0x121000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_OTA] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "ota",
        .partition_start_addr      = 0x132000,
        /* because of encoded data, OTA_size/APP_size ≈ 0.6 */
        /* 0xAE000 = 696K = (crc + zip + enc) */
        .partition_length          = 0xAE000, //696KB, 696/1088 =  0.639
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_RF_FIRMWARE] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "RF Firmware",
        #if (CFG_SOC_NAME == SOC_BK7221U)
        .partition_start_addr      = 0x1e0000,// bootloader unused space for rf cal+mac related info.
        #else
        .partition_start_addr      = 0x1e0000,// for rf related info
        #endif
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_NET_PARAM] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "NET info",
        #if (CFG_SOC_NAME == SOC_BK7221U)
        .partition_start_addr      = 0x1FF000,// for net related info
        #else
        .partition_start_addr      = 0x1e1000,// for net related info
        #endif
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    #if (CFG_SUPPORT_MATTER)
    [BK_PARTITION_MATTER_FLASH] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "matter info",
        .partition_start_addr      = 0x1e2000,// for matter
        .partition_length          = 0x9000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_MATTER_FACTORY_FLASH] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "matter factory info",
        .partition_start_addr      = 0x1eb000,// for matter factory
        .partition_length          = 0x3000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    }
    #endif
    #if AT_SERVICE_CFG
    [BK_PARTITION_USR_CONFIG] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "USER info",
        .partition_start_addr      = 0x1ee000,// for net related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_BLE_SVR_CONFIG] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "BLE SERVER",
        .partition_start_addr      = 0x1ef000,// for ble svr in ATSVR
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    #endif
    [BK_PARTITION_BLE_BONDING_FLASH] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "ble bonding info",
        .partition_start_addr      = 0x1f0000,
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
};
#endif

#if (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_4M) || (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_DYNAMIC)
const bk_logic_partition_t bk7231_partitions_4M[BK_PARTITION_MAX] =
{
    [BK_PARTITION_BOOTLOADER] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Bootloader",
        .partition_start_addr      = 0x00000000,
        /* Boot size with crc should less than this length.
           Max boot size is 0x11000, but considering that the RF or usr partition may be placed
           before APP for save flash, So reserved 8K for this reason */
        .partition_length          = 0x0F000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_APPLICATION] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Application",
        .partition_start_addr      = 0x11000,
        /* 0x275000 = 2516KB = 2368K data + 148K crc */
        .partition_length          = 0x275000,  //2516KB
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_OTA] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "ota",
        .partition_start_addr      = 0x286000,
        /* because of encoded data, OTA_size/APP_size ≈ 0.6 */
        /* 0x15A000 = 1384KB = (crc + zip + enc), 1384K / 2368K = 0.584, OTA in risk*/
        .partition_length          = 0x15A000, //1384KB
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_RF_FIRMWARE] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "RF Firmware",
        .partition_start_addr      = 0x3e0000,// for rf related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_NET_PARAM] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "NET info",
        .partition_start_addr      = 0x3e1000,// for net related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    #if (CFG_SUPPORT_MATTER)
    [BK_PARTITION_MATTER_FLASH] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "matter info",
        .partition_start_addr      = 0x3e2000,// for matter
        .partition_length          = 0x9000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_MATTER_FACTORY_FLASH] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "matter factory info",
        .partition_start_addr      = 0x3eb000,// for matter factory
        .partition_length          = 0x3000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    }
    #endif
    #if AT_SERVICE_CFG
    [BK_PARTITION_USR_CONFIG] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "USER info",
        .partition_start_addr      = 0x3ee000,// for net related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_BLE_SVR_CONFIG] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "BLE SERVER",
        .partition_start_addr      = 0x3ef000,// for ble svr in ATSVR
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    #endif
    [BK_PARTITION_BLE_BONDING_FLASH] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "ble bonding info",
        .partition_start_addr      = 0x3f0000,
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
};
#endif

#if (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_8M) || (CFG_FLASH_SELECTION_TYPE == FLASH_SELECTION_TYPE_DYNAMIC)
const bk_logic_partition_t bk7231_partitions_8M[BK_PARTITION_MAX] =
{
    [BK_PARTITION_BOOTLOADER] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Bootloader",
        .partition_start_addr      = 0x00000000,
        /* Boot size with crc should less than this length.
           Max boot size is 0x11000, but considering that the RF or usr partition may be placed
           before APP for save flash, So reserved 8K for this reason */
        .partition_length          = 0x0F000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_APPLICATION] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "Application",
        .partition_start_addr      = 0x11000,
        .partition_length          = 0x121000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_OTA] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "ota",
        .partition_start_addr      = 0x132000,
        .partition_length          = 0xAE000, //696KB
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_RF_FIRMWARE] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "RF Firmware",
        .partition_start_addr      = 0x709000,// for rf related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [BK_PARTITION_NET_PARAM] =
    {
        .partition_owner           = BK_FLASH_EMBEDDED,
        .partition_description     = "NET info",
        .partition_start_addr      = 0x70A000,// for net related info
        .partition_length          = 0x1000,
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
};
#endif

#endif // __BKFLASHPARTITION_H__

// EOF
