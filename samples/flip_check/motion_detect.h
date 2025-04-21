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

#ifndef _MOTION_DETECT_H
#define _MOTION_DETECT_H


#include "imgproc.h"

typedef struct _MotionDetection_ {
    int dstHeight;
    int min_thr;
    int max_thr;
    int number_of_changes;
    int state;     //  0-before max thr  1-after max thr 2 after min thr
    int send_state;
    int lastSend;
    int diff_thr;//judge standard
    long serverWaitTime;
    long lastMdTimer;
    IplImage *frame1;
} MotionDetection;


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
void initMotionDetection(MotionDetection *md, int min_thr, int max_thr,int diff_thr,int dstHeight,int serverWaitTime, long nowTime);
void uinitMotionDetection(MotionDetection *md);
int getMotionResult(MotionDetection *md, unsigned char *image, int width, int height, long nowTime);

#ifdef __cplusplus
}
#endif

#endif //_MOTION_DETECT_H
