#ifndef _BK_PWR_TBL_H_
#define _BK_PWR_TBL_H_

enum {
    PWR_MODE_11B = 0,
    PWR_MODE_11G,
    PWR_MODE_HT20,
    PWR_MODE_HT40,
    PWR_MODE_NUM
};

int pwr_tbl_get_pwr_idx_backoff(uint8_t channel, uint8_t pwr_mode);

#endif /* _BK_PWR_TBL_H_ */
