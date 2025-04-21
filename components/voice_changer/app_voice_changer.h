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

#ifndef __APP_VOICE_CHANGER_H__
#define __APP_VOICE_CHANGER_H__

#define VC_OUT_SIZE                   (40)
#define VC_DEF_SAMPLE_SIZE            (40)

#define VC_DEF_IDX                     3

typedef enum {
    VC_OK,
    VC_NOMEMORY,
} VC_ERR;

VC_ERR voice_changer_initial(uint32_t freq);
void voice_changer_exit(void);
void voice_changer_start(void);
void voice_changer_stop(void);
void voice_changer_set_change_flag(void);
int voice_changer_get_need_mic_data(void);
int voice_changer_set_cost_data(int cost_len);
int voice_changer_data_handle(uint8_t *mic_in, int mic_len, uint8_t **vc_out);

#endif  // __APP_VOICE_CHANGER_H__
