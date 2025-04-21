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

#ifndef __DRV_FLASH_H__
#define __DRV_FLASH_H__

#include "typedef.h"
#include "flash_pub.h"

int beken_flash_init(void);
void beken_flash_read(rt_uint32_t address, void *data, rt_uint32_t size);
void beken_flash_write(rt_uint32_t address, const void *data, rt_uint32_t size);
void beken_flash_erase(rt_uint32_t address);

#endif