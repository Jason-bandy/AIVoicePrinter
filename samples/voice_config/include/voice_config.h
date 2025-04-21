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

#ifndef _VOICE_CONFIG_H_
#define _VOICE_CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct voice_config_result
{
    uint32_t ssid_len;   /* max 32byte */
    uint32_t passwd_len; /* max 63byte */
    uint32_t custom_len; /* max 16byte */
    char ssid[32+1];
    char passwd[63+1];
    char custom[16+1];
};

#define VOICE_CONFIG_STATUS_OK                      (0)
#define VOICE_CONFIG_STATUS_ERROR                   (-1)
#define VOICE_CONFIG_STATUS_ERROR_SAMPLE_RATE       (-2)

/* memory port */
extern void *voice_malloc(int size);
extern void voice_free(void *mem);

/* audio port: read byte from sound. */
extern int voice_read(void *device, void *buffer, int size);

/* voice_config work. */
extern int voice_config_work(void *device, uint32_t sample_rate, uint32_t timeout, struct voice_config_result *result);

/* stop voice config. */
extern void voice_config_stop(void);

/* get voice config version. */
extern const char *voice_config_version(void);

#endif /* _VOICE_CONFIG_H_ */
