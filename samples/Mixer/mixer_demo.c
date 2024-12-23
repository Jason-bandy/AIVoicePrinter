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


#include "rtconfig.h"
#include "samples_config.h"

#if CONFIG_SOUND_MIXER

#ifdef MIXER_DEMO
#include "mixer.h"
void mixer_set_value(int argc, char** argv)
{
    int val;

    val = atoi(argv[1]);

    if(val == 1) {
        rt_kprintf("mixer_set_value:%d pause\r\n", val);
        mixer_pause();
    } else if(val == 0) {
        rt_kprintf("mixer_set_value:%d replay\r\n", val);
        mixer_replay();
    }
}

MSH_CMD_EXPORT(mixer_set_value, mixer_set_value test);
#endif
#endif