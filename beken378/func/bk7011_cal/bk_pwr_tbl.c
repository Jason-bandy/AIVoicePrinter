#include "include.h"
#include <string.h>
#include "arm_arch.h"
#include "bk7011_cal.h"
#include "bk_pwr_tbl.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "cal_log.h"

#if CFG_POWER_TABLE

#define	WLAN_2G_CHAN_NUM			14
#define IS_VALID_2G_CHAN(chan)		((chan) > 0 && (chan) <= WLAN_2G_CHAN_NUM)

#define BK_PWR_IDX_MAX				0x7F
#define PWR_TABLE_SUPPORT_MAX_PWR	0

/* unit 0.1dBm */
static uint16_t g_cal_pwr_base[PWR_MODE_NUM] = {
    BK_PWR_BASE_11B,
    BK_PWR_BASE_11G,
    BK_PWR_BASE_HT20,
    BK_PWR_BASE_HT40,
};

/* unit 0.1dBm */
static uint16_t g_chan_pwr_tbl[WLAN_2G_CHAN_NUM][PWR_MODE_NUM] = {
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
    {BK_PWR_BASE_11B, BK_PWR_BASE_11G, BK_PWR_BASE_HT20, BK_PWR_BASE_HT40},
};

static int8_t g_chan_pwr_idx_backoff[WLAN_2G_CHAN_NUM][PWR_MODE_NUM] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

static uint8_t g_pwr_step_size[PWR_MODE_NUM] = {
    BK_PWR_11B_STEP_SIZE,
    BK_PWR_11G_STEP_SIZE,
    BK_PWR_HT20_STEP_SIZE,
    BK_PWR_HT40_STEP_SIZE
};

static void pwr_tbl_calc_pwr_idx_backoff(uint8_t chan_idx)
{
    int32_t mode;
    int32_t diff;
    int32_t backoff;
    int32_t step_size;

    for (mode = 0; mode < PWR_MODE_NUM; mode++)
    {
        if (g_cal_pwr_base[mode] == 0)
        {
            g_chan_pwr_idx_backoff[chan_idx][mode] = 0;
            continue;
        }

        if (g_chan_pwr_tbl[chan_idx][mode] == 0)
        {
            /* disable this channel by using maximum backoff */
            g_chan_pwr_idx_backoff[chan_idx][mode] = BK_PWR_IDX_MAX;
            continue;
        }

        /* The unit of power table is 0.1dBm while the unit of step_size is 0.01dBm */
        diff = (g_cal_pwr_base[mode] - g_chan_pwr_tbl[chan_idx][mode]) * 10;
        step_size = g_pwr_step_size[mode];
        if (diff >= 0)
        {
            backoff = (diff + step_size - 1) / step_size;
        }
        else
        {
            #if PWR_TABLE_SUPPORT_MAX_PWR
            backoff = diff / step_size;
            #else
            backoff = 0;
            #endif
        }
        g_chan_pwr_idx_backoff[chan_idx][mode] = (int8_t)backoff;
    }
}

int pwr_tbl_get_pwr_idx_backoff(uint8_t channel, uint8_t pwr_mode)
{
    if (!IS_VALID_2G_CHAN(channel) || pwr_mode >= PWR_MODE_NUM)
        return 0;

    return g_chan_pwr_idx_backoff[channel - 1][pwr_mode];
}

int pwr_tbl_set_cal_pwr_base(uint16_t pwr_cal_11b, uint16_t pwr_cal_11g,
                        uint16_t pwr_cal_ht20, uint16_t pwr_cal_ht40)
{
    uint8_t chan_idx;

    CAL_LOGI("pwr_tbl: set cal pwr base: %u %u %u %u\r\n",
                    pwr_cal_11b, pwr_cal_11g, pwr_cal_ht20, pwr_cal_ht40);

    g_cal_pwr_base[PWR_MODE_11B] = pwr_cal_11b;
    g_cal_pwr_base[PWR_MODE_11G] = pwr_cal_11g;
    g_cal_pwr_base[PWR_MODE_HT20] = pwr_cal_ht20;
    g_cal_pwr_base[PWR_MODE_HT40] = pwr_cal_ht40;

    for (chan_idx = 0; chan_idx < WLAN_2G_CHAN_NUM; chan_idx++)
    {
        pwr_tbl_calc_pwr_idx_backoff(chan_idx);
    }

    return 0;
}

int pwr_tbl_set_chan_pwr(uint8_t channel, uint16_t pwr_11b,
                        uint16_t pwr_11g, uint16_t pwr_ht20, uint16_t pwr_ht40)
{
    uint8_t chan_idx;

    if (!IS_VALID_2G_CHAN(channel))
        return -1;

    CAL_LOGI("pwr_tbl: set chan pwr - chan %u, pwr: %u %u %u %u\r\n",
                    channel, pwr_11b, pwr_11g, pwr_ht20, pwr_ht40);

    chan_idx = channel - 1;
    g_chan_pwr_tbl[chan_idx][PWR_MODE_11B] = pwr_11b;
    g_chan_pwr_tbl[chan_idx][PWR_MODE_11G] = pwr_11g;
    g_chan_pwr_tbl[chan_idx][PWR_MODE_HT20] = pwr_ht20;
    g_chan_pwr_tbl[chan_idx][PWR_MODE_HT40] = pwr_ht40;

    pwr_tbl_calc_pwr_idx_backoff(chan_idx);

    return 0;
}

int pwr_tbl_get_chan_pwr(uint8_t channel, uint16_t *pwr_11b,
                        uint16_t *pwr_11g, uint16_t *pwr_ht20, uint16_t *pwr_ht40)
{
    uint8_t chan_idx;

    if (!IS_VALID_2G_CHAN(channel))
        return -1;

    chan_idx = channel - 1;
    if (pwr_11b)
    {
        *pwr_11b = g_chan_pwr_tbl[chan_idx][PWR_MODE_11B];
    }
    if (pwr_11g)
    {
        *pwr_11g = g_chan_pwr_tbl[chan_idx][PWR_MODE_11G];
    }
    if (pwr_ht20)
    {
        *pwr_ht20 = g_chan_pwr_tbl[chan_idx][PWR_MODE_HT20];
    }
    if (pwr_ht40)
    {
        *pwr_ht40 = g_chan_pwr_tbl[chan_idx][PWR_MODE_HT40];
    }

    return 0;
}

void pwr_tbl_dump_power_table(void)
{
    int i;

    CAL_LOGI("\r\n  cal base: %u  %u  %u  %u\r\n",
        g_cal_pwr_base[0],
        g_cal_pwr_base[1],
        g_cal_pwr_base[2],
        g_cal_pwr_base[3]);

    CAL_LOGI("\r\n  bk power table:\r\n");
    for (i = 0; i < 14; i++)
    {
        CAL_LOGI("    chan %u:  %u  %u  %u  %u\r\n",
            i + 1,
            g_chan_pwr_tbl[i][0],
            g_chan_pwr_tbl[i][1],
            g_chan_pwr_tbl[i][2],
            g_chan_pwr_tbl[i][3]);
    }

    CAL_LOGI("\r\n  bk pwr backoff:\r\n");
    for (i = 0; i < 14; i++)
    {
        CAL_LOGI("    chan %u:  %d  %d  %d  %d\r\n",
            i + 1,
            g_chan_pwr_idx_backoff[i][0],
            g_chan_pwr_idx_backoff[i][1],
            g_chan_pwr_idx_backoff[i][2],
            g_chan_pwr_idx_backoff[i][3]);
    }
}

#else
int pwr_tbl_get_pwr_idx_backoff(uint8_t channel, uint8_t pwr_mode)
{
    return 0;
}

int pwr_tbl_set_cal_pwr_base(uint16_t pwr_cal_11b, uint16_t pwr_cal_11g,
                        uint16_t pwr_cal_ht20, uint16_t pwr_cal_ht40)
{
    return -1;
}

int pwr_tbl_set_chan_pwr(uint8_t channel, uint16_t pwr_11b,
                        uint16_t pwr_11g, uint16_t pwr_ht20, uint16_t pwr_ht40)
{
    return -1;
}

int pwr_tbl_get_chan_pwr(uint8_t channel, uint16_t *pwr_11b,
                        uint16_t *pwr_11g, uint16_t *pwr_ht20, uint16_t *pwr_ht40)
{
    return -1;
}

void pwr_tbl_dump_power_table(void)
{

}
#endif /* CFG_POWER_TABLE */

#define PWR_TBL_CMD_SUPPORT         0
void pwrtbl(int argc, char **argv)
{
#if PWR_TBL_CMD_SUPPORT
    uint16_t channel;
    uint16_t pwr_11b;
    uint16_t pwr_11g;
    uint16_t pwr_ht20;
    uint16_t pwr_ht40;

    if (argc < 3)
        goto usage;

    if (os_strncmp(argv[1], "cal", 3) == 0)
    {
        if (argc < 6)
            goto usage;
        pwr_11b = os_strtoul(argv[2], NULL, 0);
        pwr_11g = os_strtoul(argv[3], NULL, 0);
        pwr_ht20 = os_strtoul(argv[4], NULL, 0);
        pwr_ht40 = os_strtoul(argv[5], NULL, 0);
        if (pwr_tbl_set_cal_pwr_base(pwr_11b, pwr_11g, pwr_ht20, pwr_ht40))
            CAL_LOGI("set cal pwr base error!");
    }
    else if (os_strncmp(argv[1], "get", 3) == 0)
    {
        #if 1 //xl_yue
        if (os_strncmp(argv[2], "all", 3) == 0)
        {
            pwr_tbl_dump_power_table();
            return;
        }
        #endif
        channel = os_strtoul(argv[2], NULL, 0);
        if (!pwr_tbl_get_chan_pwr(channel, &pwr_11b, &pwr_11g, &pwr_ht20, &pwr_ht40))
        {
            CAL_LOGI("chan %u, pwr: %u %u %u %u\r\n", channel, pwr_11b, pwr_11g, pwr_ht20, pwr_ht40);
        }
        else
        {
            CAL_LOGI("get chan pwr error!");
        }
    }
    else if (os_strncmp(argv[1], "set", 3) == 0)
    {
        if (argc < 7)
            goto usage;
        channel = os_strtoul(argv[2], NULL, 0);
        pwr_11b = os_strtoul(argv[3], NULL, 0);
        pwr_11g = os_strtoul(argv[4], NULL, 0);
        pwr_ht20 = os_strtoul(argv[5], NULL, 0);
        pwr_ht40 = os_strtoul(argv[6], NULL, 0);
        if (pwr_tbl_set_chan_pwr(channel, pwr_11b, pwr_11g, pwr_ht20, pwr_ht40))
            CAL_LOGI("set chan pwr error!\r\n");
    }
    else
    {
        goto usage;
    }

    return;

 usage:
    CAL_LOGI("Usage:\r\n");
    CAL_LOGI("  pwrtbl cal <pwr_11b> <pwr_11g> <pwr_ht20> <pwr_ht40>\r\n");
    CAL_LOGI("  pwrtbl set <channel> <pwr_11b> <pwr_11g> <pwr_ht20> <pwr_ht40>\r\n");
    CAL_LOGI("  pwrtbl get <channel>\r\n");
#else
    CAL_LOGI("pwrtbl cmd no support!\r\n");
#endif // PWR_TBL_CMD_SUPPORT
}

#if CFG_SUPPORT_RTT
FINSH_FUNCTION_EXPORT_ALIAS(pwrtbl, __cmd_pwrtbl, pwrtbl test);
#else
void pwr_tbl_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    pwrtbl(argc, argv);
}
#endif

