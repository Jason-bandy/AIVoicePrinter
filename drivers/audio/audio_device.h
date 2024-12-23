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

#ifndef AUDIO_DEVICE_H__
#define AUDIO_DEVICE_H__

enum AUDIO_DEVICE_STATE
{
    AUDIO_DEVICE_IDLE,
    AUDIO_DEVICE_PLAYBACK,
    AUDIO_DEVICE_CLOSE,
};

void *audio_device_get_buffer(int *bufsz);
void audio_device_put_buffer(void *ptr);

void audio_device_write(void *buffer, int size);

int audio_device_init(void);
void audio_device_close(void);

void audio_device_open(void);

int audio_device_set_evt_handler(void (*handler)(void *parameter, int state), void *parameter);

void audio_device_set_rate(int sample_rate);
void audio_device_set_volume(int volume);
void audio_device_wait_free(void);
void audio_device_mic_set_volume(int volume);
void audio_device_mic_open(void);
void audio_device_mic_set_rate(int sample_rate);
void audio_device_mic_set_channel(int channel);
int audio_device_mic_read(void *buffer, int size);
void audio_device_mic_close(void);
#endif
