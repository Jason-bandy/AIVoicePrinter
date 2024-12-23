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

#ifndef _JPEG_DEC_H_
#define _JPEG_DEC_H_
int jpg_decoder_init(int width,int heigth,int ratio);
void jpg_decoder_deinit(void);
int jpg_decoder_fun(unsigned char *jpg_buf,unsigned char ** Y_buf,int pic_size);
void jpg_get_pic_size(unsigned int *width,unsigned int *heigth);
#endif