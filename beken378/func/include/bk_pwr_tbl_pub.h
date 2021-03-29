#ifndef _BK_PWR_TBL_PUB_H_
#define _BK_PWR_TBL_PUB_H_

#include "typedef.h"

/*
 * Configure the calibration power bases. these powers are necessary for calculating
 * power backoff when configuring the channel powers with API pwr_tbl_set_chan_pwr().
 *     pwr_cal_11b:  the 11b power in unit 0.1dBm when calibration;
 *     pwr_cal_11g:  the 11g power in unit 0.1dBm when calibration;
 *     pwr_cal_ht20: the ht20 power in unit 0.1dBm when calibration;
 *     pwr_cal_ht40: the ht40 power in unit 0.1dBm when calibration;
 * return 0 if succeed, otherwise return -1.
 */
extern int pwr_tbl_set_cal_pwr_base(uint16_t pwr_cal_11b,
                                    uint16_t pwr_cal_11g,
                                    uint16_t pwr_cal_ht20,
                                    uint16_t pwr_cal_ht40);

/*
 * Configure power table for a channel, power '0' means to disable this channel.
 *     channel:  the 2.4G channel number(1 - 14)
 *     pwr_11b:  the 11b power in unit 0.1dBm
 *     pwr_11g:  the 11g power in unit 0.1dBm
 *     pwr_ht20: the ht20 power in unit 0.1dBm
 *     pwr_ht40: the ht40 power in unit 0.1dBm
 * return 0 if succeed, otherwise return -1.
 */
extern int pwr_tbl_set_chan_pwr(uint8_t channel,
                                uint16_t pwr_11b,
                                uint16_t pwr_11g,
                                uint16_t pwr_ht20,
                                uint16_t pwr_ht40);

/*
 * Get power table of a channel.
 *     channel:  the 2.4G channel number(1 - 14)
 *     pwr_11b:  the memory pointer to save 11b power in unit 0.1dBm
 *     pwr_11g:  the memory pointer to save 11g power in unit 0.1dBm
 *     pwr_ht20: the memory pointer to save ht20 power in unit 0.1dBm
 *     pwr_ht40: the memoyr pointer to save ht40 power in unit 0.1dBm
 * return 0 if succeed, otherwise return -1.
 */
extern int pwr_tbl_get_chan_pwr(uint8_t channel,
                                uint16_t *pwr_11b,
                                uint16_t *pwr_11g,
                                uint16_t *pwr_ht20,
                                uint16_t *pwr_ht40);

#endif /* _BK_PWR_TBL_PUB_H_ */

