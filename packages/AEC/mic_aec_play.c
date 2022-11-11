
#include <rtthread.h>
#include <rtdevice.h>
#include <finsh.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "audio_device.h"
#include "vad.h"
//#include "test_config.h"
#include "aec.h"
#ifdef MIC_AEC_TEST
static uint8_t *test_buf;
#define TEST_BUFF_LEN 80*1024
#define READ_SIZE 2048

 //8K的FRAME长度为160,16K的长度为320, AEC一次处理FRAME_LEN*2个字节

/********************************************************
 	argv[1]: vad  off:0  on:1
 	argv[2]: work_mode
 	0: one micphone(mic1)
 	argv[3]: sample_rate  8000/16000

	cmd format :mic_aec_play 0/1 0/1/2/3 8000/16000
	eg: mic_aec_play 1 0 8000

**********************************************************/
uint8_t aecbuffer[sizeof(AECContext)+sizeof(int16_t)*AEC_MAX_MIC_DELAY];
void mic_aec_play(int argc,char *argv[])
{
	int mic_read_len = 0;
	int actual_len;
	int dac_wr_len=0;
	uint16_t *buffer = NULL;
	uint16_t read_size;
	int sample_rate;
	int index;
    uint16_t *ptr;
    AECContext* aec = NULL;
    uint32_t val = 0;

    int16_t* ref = NULL;
    int16_t* mic = NULL;
    int16_t* out = NULL;

	int work_mode;
	int vad_on;
	if(argc<4)
	{
		rt_kprintf("parameter errorr\r\n");
	}
	vad_on = atoi(argv[1]);
	work_mode = atoi(argv[2]);
	sample_rate = atoi(argv[3]);


	rt_kprintf("vad_on:%d,work_mode:%d,sample_rate:%d\r\n",vad_on,work_mode,sample_rate);
	test_buf = sdram_malloc(TEST_BUFF_LEN);
	if(test_buf == NULL)
	{
		rt_kprintf("===not enough memory===\r\n");
		return;
	}

	audio_device_init();

	audio_device_mic_open();

	audio_device_mic_set_channel(1);
	read_size = READ_SIZE;
	//read_size = sample_rate * sizeof(int16_t);
	audio_device_mic_set_rate(sample_rate);

	if (vad_on)
	{
	    rt_kprintf("Vad is ON !!!!!!!!\r\n");	/*进入vad检测*/
		wb_vad_enter();
	}

	while(1)
	{
		rt_thread_delay(10);
		if(mic_read_len > TEST_BUFF_LEN - READ_SIZE)
			break;
		if(work_mode ==0){
			actual_len = audio_device_mic_read(test_buf+mic_read_len,read_size);
			mic_read_len += actual_len;
			if(vad_on)
			{
				if(wb_vad_entry((char*)test_buf+mic_read_len, 320))/*vad process*/
				{
					rt_kprintf("------------vad end----------\r\n");
					break;
				}
			}
		}
	}
	rt_kprintf("mic_read_len is %d\r\n", mic_read_len);
	audio_device_mic_close();
	if (vad_on)
	{
		wb_vad_deinit();			/*关闭vad检测*/
	}


	audio_device_open();
	audio_device_set_rate(sample_rate);

    //需要buffer大约二十多kByte，具体值和AEC_MAX_MIC_DELAY有关, 宏设置需要大于一帧并不小于实际延迟，默认写的1500
     aec = (AECContext*)aecbuffer;

     //采样率可以配置8000或者16000
     aec_init(aec, sample_rate);

     //获取处理帧长，16000采样率320点(640字节)，8000采样率160点(320字节)  (对应20毫秒数据)
    // aec_ctrl(aec, AEC_CTRL_CMD_GET_FRAME_SAMPLE, (uint32_t)(&frame_len));




     //获取结构体内部可以复用的ram作为每帧tx,rx,out数据的临时buffer; ram很宽裕的话也可以在外部单独申请获取输入输出buffer
     aec_ctrl(aec, AEC_CTRL_CMD_GET_TX_BUF, (uint32_t)(&val)); mic = (int16_t*)val;
     aec_ctrl(aec, AEC_CTRL_CMD_GET_RX_BUF, (uint32_t)(&val)); ref = (int16_t*)val;
     aec_ctrl(aec, AEC_CTRL_CMD_GET_OUT_BUF,(uint32_t)(&val)); out = (int16_t*)val;


     //以下是参数调节示例,aec_init中都已经有默认值,不设置也能跑起来
     aec_ctrl(aec, AEC_CTRL_CMD_SET_FLAGS, 0x1f);                              //库内各模块开关; aec_init内默认赋值0x1f;

     ///回声消除相关
     aec_ctrl(aec, AEC_CTRL_CMD_SET_MIC_DELAY, 1000);                            //设置参考信号延迟(采样点数，需要dump数据观察)
     aec_ctrl(aec, AEC_CTRL_CMD_SET_EC_DEPTH, 11);                              //建议取值范围1~50; 后面几个参数建议先用aec_init内的默认值，具体需要根据实际情况调试; 总得来说回声越大需要调的越大
     ///降噪相关
     aec_ctrl(aec, AEC_CTRL_CMD_SET_NS_LEVEL, 2);                              //建议取值范围1~8；值越小底噪越小
     aec_ctrl(aec, AEC_CTRL_CMD_SET_NS_PARA,  1);                              //只能取值0,1,2; 降噪由弱到强，建议默认值
     ///drc(输出音量相关)
     aec_ctrl(aec, AEC_CTRL_CMD_SET_DRC, 0x01);

     read_size=160 * sizeof(int16_t);      //8K一帧数据160个字节



	while(1)
	{
		buffer = (uint16_t *)audio_device_get_buffer(RT_NULL);
		if(dac_wr_len >= mic_read_len)
		{
			audio_device_put_buffer(buffer);
			break;
		}

		memcpy((uint8_t *)mic,test_buf+dac_wr_len,read_size);
		//ref数据为远端传过来通过喇叭播放的数据
		memset((uint8_t *)ref,0,read_size);
		dac_wr_len +=read_size;

		//回声消除处理
		aec_proc(aec, mic, ref, out);

		memcpy(buffer,out,read_size);

		ptr = (uint16_t *)((uint8_t *)buffer + read_size * 2);
		ptr -= 1;

		for (index = 1; index < read_size / 2; index ++)
		{
			*ptr = *(ptr - 1) = buffer[read_size / 2 - index];
			 ptr -= 2;
		}

		audio_device_write((uint8_t *)buffer, read_size);

		rt_thread_delay(10);
	}
	audio_device_close();

	if(test_buf)
		sdram_free(test_buf);

}
MSH_CMD_EXPORT(mic_aec_play, test aec play);
#endif
