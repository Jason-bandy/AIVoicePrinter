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

#ifndef __DRV_PSRAM_H__
#define __DRV_PSRAM_H__

/*
 * line_mode      0: 1 line; 1: 4 line
 * voltage_level  0: 1.8v; 1: 2.5v; 2: 3.3v
 */
void psram_init(uint8_t line_mode,uint8_t voltage_level);
void *psram_malloc(unsigned long size);
void psram_free(void *ptr);
void *psram_calloc(unsigned int n, unsigned int size);
void *psram_realloc(void *ptr, unsigned long size);

#endif