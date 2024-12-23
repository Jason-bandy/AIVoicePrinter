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

#include <rtthread.h>

#include <speex/speex.h>

void speex_warning_int(const char *str, int val)
{
    rt_kprintf ("warning: %s %d\n", str, val);
}

void speex_warning(const char *str)
{
    rt_kprintf ("warning: %s \n", str);
}

void speex_notify(const char *str)
{
    rt_kprintf ("speex_notify: %s \n", str);
}

/**
  * @brief  Ovveride the _speex_fatal function of the speex library
  * @param  None
  * @retval : None
  */
void _speex_fatal(const char *str, const char *file, int line)
{
    rt_kprintf ("_speex_fatal: %s \n", str);
}

/**
  * @brief  Ovveride the _speex_putc function of the speex library
  * @param  None
  * @retval : None
  */
void _speex_putc(int ch, void *file)
{
    rt_kprintf ("_speex_putc: %c \n", ch);
}
