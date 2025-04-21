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

#ifndef __COMM_UART_H__
#define __COMM_UART_H__

int comm_uart_init(void);
uint8_t comm_uart_getchar(void);
int comm_uart_putchar(const rt_uint8_t ch);
int comm_uart_sendstr(const char *str, size_t len);
int comm_uart_deinit(void);

#endif//__COMM_UART_H__/