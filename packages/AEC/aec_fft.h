#ifndef AEC_FFT_H
#define AEC_FFT_H

#include <stdint.h>

#define    FS                   (16000)

#if (FS==8000)
#define    BD                   (1)
#else
#define    BD                   (2)
#endif

#define FFT_LEN_NB              (256)
#define FFT_LEN_WB              (FFT_LEN_NB<<1)

#define FFT_LEN_NB_HF           (FFT_LEN_NB>>1)
#define FFT_LEN_WB_HF           (FFT_LEN_WB>>1)

#define FRAME_LEN_NB            (160)
#define FRAME_LEN_WB            (FRAME_LEN_NB<<1)

#define SymWin                  (1)            //
#define AsymWin                 (!SymWin)      // low delay

typedef struct FFTINFO
{
    int16_t  N;
    int16_t  M;
    int16_t  *map;
    int16_t  *fftw;
    int16_t  *rft_win;
    int16_t  *ana_win;
    int16_t  *syn_win;
}FFT_INFO;

void rfft_win (int32_t *cx,int32_t *dx,int16_t *win,FFT_INFO *fftInfo);

void inv_rfft (int32_t *cx,int32_t *dx,FFT_INFO *fftInfo);

void FFT_init (FFT_INFO *FFT_ptr,int16_t band);

#endif
