#ifndef _DRIVER_AUDIO_H_
#define _DRIVER_AUDIO_H_
#pragma anon_unions

//#include "app_equ.h"
//#include "driver_dma_fft.h"
#include "Common/include/config.h"
//#include "driver_ringbuff.h"

#define ADC_FIRST_DELAY         //to control whether discard the first adc data to avoid openning noise
#define PAMUTE_GPIO_PIN         15

#define AUDIO_BUFF_LEN  4096//3072//2560	/**< 2.5k */
#define PCM_BUFF_LEN    1536	/**< 1k */

#define AUDIO_CLK_DIV_441K   2
#define AUDIO_CLK_DIV_48K     3
#define AUDIO_CLK_DIV_8K       0
#define AUDIO_CLK_DIV_16K     1

#define AUDIO_VOLUME_MIN    	0//aud_min_volume_get()
#define AUDIO_VOLUME_MAX        16

#define AUDIO_DIV_441K          0x049B2368
#define AUDIO_DIV_441K_SLOW     0x049B2970
#define AUDIO_DIV_441K_FAST     0x049B1D5C

#define AUDIO_DIV_48K           0x043B5554
#define AUDIO_DIV_48K_SLOW      0x043B5AE0
#define AUDIO_DIV_48K_FAST      0x043B4FC8

#ifdef CONFIG_TWS
#define AUDIO_SYNC_INTVAL 		3000
#define AUDIO_DIV_COVER 		1
#define AUDIO_DIV_441K_Dot ((175*AUDIO_SYNC_INTVAL/10000)*AUDIO_DIV_COVER)
#define AUDIO_DIV_48K_Dot ((142*AUDIO_SYNC_INTVAL/10000)*AUDIO_DIV_COVER)
#define AUDIO_DIV_INIT ((AUDIO_DIV_441K_SLOW-AUDIO_DIV_441K)/AUDIO_DIV_441K_Dot)
#define AUDIO_DIV_MAX (AUDIO_DIV_INIT*3)
#endif

#if A2DP_ROLE_SOURCE_CODE
#define AUDIO_SYNC_INTVAL 		3000
#endif

enum
{
    DMA_BUF_TYPE_MUSIC = 0,
    DMA_BUF_TYPE_VOICECALL = 1,
    DMA_BUF_TYPE_RING = 2
};

typedef struct{
	union {
	    struct sync_start_s{
			uint32 start_time : 28;
			uint32 flag : 4;
			uint32 clk_val :28;
			uint32 vol :4;
	    } __PACKED_POST__ sync_start;

	    struct sync_send_s{
			uint32 bt_clk : 28;
			uint32 clk_mode : 4;
			uint32 aud_num;
	    } __PACKED_POST__ sync_send;
	}u;
	int16 aud_num_tmp;
}__PACKED_POST__ sync_data_TypeDef;

int   aud_initial(uint32 freq, uint32 channels, uint32 bits_per_sample);
void aud_open( void );
int   aud_close(void);
uint16 aud_get_buffer_size(void);
void aud_fill_buffer( uint8 *buff, uint16 size );

void aud_mic_open( uint8 enable );
void aud_mic_volume_set( uint8 volume );
void aud_mic_mute( uint8 enable );

void aud_volume_mute( uint8 enable );
void aud_volume_set( int8 volume );

#ifdef CONFIG_TWS

extern sync_data_TypeDef sync_data;
void   dac_set_clk( uint32_t clk_val);
void   dac_clk_adjust_tws( int mode );
void show_dac_clk(int pos);

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE

typedef struct _driver_lineindata_s
{
    driver_ringbuff_t   data_rb;
}DRIVER_LINEINDATA_T;
DRIVER_LINEINDATA_T linein_data_blk;

driver_ringbuff_t *get_line_ringbuf(void);
void line_in_fill_aud_buf(uint8 *buff, uint16 size );

#endif

#endif


#if A2DP_ROLE_SOURCE_CODE
extern sync_data_TypeDef sync_data;
#endif

#endif
