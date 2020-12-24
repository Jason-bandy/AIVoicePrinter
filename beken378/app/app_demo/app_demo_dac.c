#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <finsh.h>

#include <drivers/audio.h>

#include <rtthread.h>
#include "audio_adpcm.h"
#include "co_list.h"
#include "rtos_pub.h"
#include "error.h"
#include "mem_pub.h"
#include "uart_pub.h"

// pcm = L + R
// Ldata = 4 * adpcm_sample
// so, pcm_len = 2 * 4 * adpcm_len = 8 * adpcm_len

// use for audio node: mp
#define ADPCM_NODE_SIZE            (1024)
#define ADPCM_NODE_NUM             (2)

// use for buf adpcm data
#define ADPCM_ELEM_SIZE            (ADPCM_NODE_SIZE / 8)
#define ADPCM_ELEM_NUM             (32)

typedef struct adpcm_elem_st
{
    struct co_list_hdr hdr;
    void *buf_start;
    UINT32 buf_len;
} ADPCM_ELEM_ST, *ADPCM_ELEM_PTR;

struct app_dac_player
{
    rt_uint8_t pool[ADPCM_ELEM_SIZE * ADPCM_ELEM_NUM];
    ADPCM_ELEM_ST elem[ADPCM_ELEM_NUM];
    struct co_list free;
    struct co_list ready;

    UINT32 is_run;
    struct rt_mempool mp;
    rt_uint8_t *mempool;
    rt_device_t dac_device;
    beken_thread_t thread;
    beken_semaphore_t aready_semaphore; 
};

static struct app_dac_player *appdac_player = NULL;

static rt_err_t dapcm_play_write_done(struct rt_device *device, void *ptr)
{
    if (!ptr)
    {
        os_printf("device buf_release NULL\n");
        return -RT_ERROR;
    }

    //rt_kprintf("write_done \n");
    rt_mp_free(ptr);
    return RT_EOK;
}

int dapcm_play_write_data(unsigned char * buffer, int buffer_bytes)
{
    ADPCM_ELEM_PTR elem = NULL;
    rt_size_t read_bytes = 0;
    rt_uint8_t *src_ptr = buffer, *decode_ptr;

    if(appdac_player == NULL)
        return 0;

    while(read_bytes < buffer_bytes)
    {
        rt_size_t cost_size;
        int16_t *src, *dst;
        
        cost_size = buffer_bytes - read_bytes;
        elem = (ADPCM_ELEM_PTR)co_list_pick(&appdac_player->free);
        
        if(elem) 
        {
            if(cost_size > ADPCM_ELEM_SIZE)
                cost_size = ADPCM_ELEM_SIZE;

            os_memcpy(elem->buf_start, src_ptr, cost_size);

            elem->buf_len = cost_size;

            read_bytes += cost_size;
            src_ptr += cost_size;
            
            co_list_pop_front(&appdac_player->free);
            co_list_push_back(&appdac_player->ready, (struct co_list_hdr *)&elem->hdr);            
        } 
        else
        {
            //rt_kprintf("adpcm no node, %d lost\r\n", cost_size);
            break;
        }
    }

    rtos_set_semaphore(&appdac_player->aready_semaphore);

    return read_bytes;
}

static void dapcm_play_pool_init(void)
{
    UINT32 i = 0;

    co_list_init(&appdac_player->free);
    co_list_init(&appdac_player->ready);

    for(i = 0; i < ADPCM_ELEM_NUM; i++)
    {
        appdac_player->elem[i].buf_start =
            (void *)&appdac_player->pool[i * ADPCM_ELEM_SIZE];
        appdac_player->elem[i].buf_len = 0;

        co_list_push_back(&appdac_player->free,
                          (struct co_list_hdr *)&appdac_player->elem[i].hdr);
    }
}

static void dapcm_play_thread_entry(beken_thread_arg_t arg)
{
    appdac_player->is_run = 1;
    rt_device_open(appdac_player->dac_device, RT_DEVICE_OFLAG_WRONLY);
    
    while (appdac_player->is_run)
    {
        ADPCM_ELEM_PTR elem = NULL;

        rtos_get_semaphore(&appdac_player->aready_semaphore, 500);

        do{
            elem = (ADPCM_ELEM_PTR)co_list_pick(&appdac_player->ready);        
            if(elem) 
            {
                rt_uint8_t *decode_ptr, *buffer;
                int16_t *src, *dst;
                int pcm_len;
                    
                buffer = (rt_uint8_t *)rt_mp_alloc(&(appdac_player->mp), 20);
                if(!buffer)
                {
                     //rt_kprintf(" no mp for adpcm, wait\n");
                }
                else
                {
                    pcm_len = elem->buf_len * 4;
                    decode_ptr = buffer + pcm_len;
                    
                    ADPCM_DecodeData(0, elem->buf_start, elem->buf_len, decode_ptr);
                    
                    // convert to two channel
                    src = (int16_t *)(decode_ptr);
                    dst = (int16_t *)(buffer);
                    
                    for(int i=0; i < pcm_len; i++) 
                    {
                        dst[2*i] = src[i];
                        dst[2*i + 1] = src[i];
                    }

                    co_list_pop_front(&appdac_player->ready);
                    co_list_push_back(&appdac_player->free, (struct co_list_hdr *)&elem->hdr);
                    
                    rt_device_write(appdac_player->dac_device, 0, buffer, pcm_len*2);
                }
            }
        }
        while(elem);
    }

mic_dac_exit:

    os_printf("exit adpcm main\n");

    if(appdac_player->dac_device)
        rt_device_close(appdac_player->dac_device);
    appdac_player->dac_device = RT_NULL;
    
    appdac_player->thread = NULL;
    rtos_delete_thread(NULL);
}

int dapcm_play_deinit(void)
{
    GLOBAL_INT_DECLARATION();
    
    if(appdac_player)
    {
        os_printf("start app dac stop play\n");

        GLOBAL_INT_DISABLE();
        appdac_player->is_run = 0;
        GLOBAL_INT_RESTORE();

        while(appdac_player->thread)
            rtos_delay_milliseconds(40);

        if(appdac_player->aready_semaphore)
            rtos_deinit_semaphore(&appdac_player->aready_semaphore);
        appdac_player->aready_semaphore = NULL;

        if(appdac_player->mempool)
        {
            rt_mp_detach(&(appdac_player->mp));
            rt_free(appdac_player->mempool);
        }
        
        rt_free(appdac_player);
        appdac_player = NULL;
    }
}

int dapcm_play_init(int sample_rate)
{
    rt_thread_t tid = RT_NULL;
    int result, rate, channel, mp_block_size, mp_cnt;
    
    if(appdac_player != RT_NULL)
    {
        //rt_kprintf("play adpcm aready open\n");
        os_printf("play adpcm aready open\n");
  
        return RT_EOK;
    }
    //rt_kprintf("play adpcm init\n");
    os_printf("play adpcm init\n");

    appdac_player = rt_malloc(sizeof(struct app_dac_player));
    if(!appdac_player)
    {
        os_printf("play adpcm no memory\n");
        return -RT_ERROR;
    }
    memset(appdac_player, 0, sizeof(struct app_dac_player));

    appdac_player->dac_device = rt_device_find("sound");
    if (!appdac_player->dac_device)
    {
        os_printf("play adpcm no device\n");
        dapcm_play_deinit();
        return -RT_ERROR;
    }

    mp_block_size = ADPCM_NODE_SIZE;
    mp_cnt = ADPCM_NODE_NUM;
    appdac_player->mempool = rt_malloc(mp_block_size * mp_cnt);
    if (!appdac_player->mempool)
    {
        os_printf("play adpcm no memory\n");
        dapcm_play_deinit();
        return -RT_ERROR;
    }
    rt_mp_init(&(appdac_player->mp), "adpcm_mp", appdac_player->mempool, 
        mp_block_size * mp_cnt, mp_block_size);
    
    rate = sample_rate;
    rt_device_control(appdac_player->dac_device, CODEC_CMD_SAMPLERATE, (void *)&rate);
    rt_device_set_tx_complete(appdac_player->dac_device, dapcm_play_write_done);  

    dapcm_play_pool_init();

    if(rtos_init_semaphore(&appdac_player->aready_semaphore, 10) != kNoErr)
    {
        os_printf("play adpcm semap init err\r\n");
        dapcm_play_deinit();
        return -RT_ERROR;
    }

    result = rtos_create_thread(&appdac_player->thread, 5, (const char*)"adpcm_play", 
        (beken_thread_function_t)dapcm_play_thread_entry, 1024, NULL);
    if (result != kNoErr)
    {
        os_printf("play adpcm thread err\r\n");
        dapcm_play_deinit();
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

#include "dfs_posix.h"

int dapcm_play_decode(void)
{
    int fd = open("sd/abc_160.wav", O_RDONLY);
    if(fd < 0)
    {
        os_printf("open fd fail\r\n");
        return 0;
    }

    #define RD_BUF_SIZE     4*1024
    rt_uint8_t *rd_buf;
    
    rd_buf = rt_malloc(RD_BUF_SIZE);
    if(rd_buf == NULL)
    {
        os_printf("no buf\r\n");
        return 0;
    }

    read(fd, rd_buf, 44);
    
    while(1)
    {
        int write_len, rd_ret;
        rd_ret = read(fd, rd_buf, RD_BUF_SIZE); 
        if(rd_ret < 0)
        {
            os_printf("read err:%d \r\n", rd_ret);
            break;
        }

        if(rd_ret != RD_BUF_SIZE)
        {
            os_printf("rd %d-%d\r\n", rd_ret, RD_BUF_SIZE);
            if(rd_ret == 0)
            {
                os_printf("read end\r\n");
                break;
            }
        }

        write_len = 0;
        while(rd_ret != write_len)
        {
            rt_uint8_t *buf = rd_buf + write_len;
            int left = rd_ret - write_len, ret;
            
            ret = dapcm_play_write_data(buf, left);
            if(ret != left)
            {
                rtos_delay_milliseconds(200);
            }

            write_len += ret;
        }
    }

    os_printf("loop fin\r\n");
    
    rt_free(rd_buf);
}

#if CFG_SUPPORT_RTT
void adpcm(int argc, char **argv)
{
    if(0 == strcmp(argv[1], "init"))
    {
        dapcm_play_init(8000);
    }
    else if(0 == strcmp(argv[1], "deinit"))
    {
        dapcm_play_deinit();
    }
    else if(0 == strcmp(argv[1], "decode"))
    {
        dapcm_play_decode();
    }
    else if(0 == strcmp(argv[1], "encode"))
    {
        
    }
    else
    {
        os_printf("adpcm init/deinit/decode/encode\r\n");
    }
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(adpcm, __cmd_adpcm, adpcm);
