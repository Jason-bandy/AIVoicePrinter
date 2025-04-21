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

#ifndef __DRV_SPI_FLASH_H__
#define __DRV_SPI_FLASH_H__

typedef struct bk_spi_flash_erase_st {
    rt_uint32_t addr;
    rt_uint32_t size;
} BK_SPIFLASH_ERASE_ST, *BK_SPIFLASH_ERASE_PTR;

#define BK_SPI_FLASH_ERASE_CMD          (0x01)
#define BK_SPI_FLASH_PROTECT_CMD        (0x02)
#define BK_SPI_FLASH_UNPROTECT_CMD      (0x03)


#endif
