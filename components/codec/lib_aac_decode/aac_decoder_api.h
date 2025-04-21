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

#ifndef __AAC_DECODER_API_H__
#define __AAC_DECODER_API_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include <stdint.h>

/**
 * @brief  Get AAC decoder context size
 * @return AAC decoder context size
 */
uint32_t aac_decoder_get_ram_size(void);

uint32_t aac_decoder_get_sample_rate(void* aac_decoder);
uint32_t aac_decoder_get_num_channels(void* aac_decoder);
uint32_t aac_decoder_get_pcm_samples(void* aac_decoder);
uint8_t* aac_decoder_get_pcm_buffer(void* aac_decoder);
uint32_t aac_decoder_get_filled_buffer(void* aac_decoder);

/**
 * @brief  Initialize AAC decoder
 * @return initialize result, 0: success, others: failed with error code.
 */
int32_t aac_decoder_initialize(void* aac_decoder, void* handle, void* read, void* seek, void* tell);

/**
 * @brief  Decode an AAC frame
 * @return decode result, 0: no error, <0: fatal error, Cannot not be ignored, >0: Slight error, can be ignored.
 */
int32_t aac_decoder_decode(void* aac_decoder);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__AAC_DECODER_API_H__
