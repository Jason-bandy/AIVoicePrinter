#include "low_voltage_ps.h"
#include "power_save_pub.h"
#include "power_save.h"
#include "calendar_pub.h"
#include "mcu_ps.h"
#include "low_voltage_compensation.h"
#include "ps.h"

#define LV_PS_BEACON_LOSS_TIME_S            (30)

uint32_t lv_ps_beacon_interval = 0;
uint32_t lv_ps_start_flag = 0;
uint32_t lv_ps_enable_print = 0;
uint32_t lv_anchor_flag = 0;
uint32_t lv_ps_current_sleep_duration = 0;
uint64_t lv_ps_bcn_tsf_field = 0;
uint32_t lv_ps_beacon_cnt_after_wakeup = 0;
uint32_t lv_ps_bcn_loss_flag_after_wakeup = 0;
uint64_t lv_ps_wakeup_mac_timepoint = 0;
int32_t lv_ps_bcn_delay_duration = 0;
uint32_t lv_ps_bcn_frame_duration = 0;
uint64_t lv_ps_bcn_rxd_local_time = 0;
uint32_t lv_ps_tbtt_to_rxd_time = 0;
#if (AFTER_MISSING_STRATEGY == WAIT_UNTIL_RECVED)
uint32_t lv_ps_loss_bcn_count = 0;
#endif
int32_t lv_ps_pre_lead_wakeup_duration = 0;
uint32_t lv_ps_win_pri_compensation_factor;
uint32_t lv_ps_win_post_compensation_factor;
#if (AFTER_MISSING_STRATEGY == WAIT_ONCE)
uint32_t lv_ps_bcn_has_been_waiting = 0;
#endif
uint32_t lv_ps_bcn_cont_miss_bcn_count = 0;
uint64_t lv_ps_last_beacon_rev_timepoint = 0;
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
PS_INFO_T ps_info;
#endif
extern uint32_t lvc_general_sleep_flag;

extern void ps_send_connection_loss(void);
static uint32_t lv_ps_check_beacon_loss(void);

void lv_ps_init(void)
{
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    lv_ps_beacon_interval = 0;
    lv_ps_start_flag = 0;
    lv_ps_enable_print = 0;
    lv_anchor_flag = 0;
    lv_ps_current_sleep_duration = 0;
    lv_ps_bcn_tsf_field = 0;
    lv_ps_beacon_cnt_after_wakeup = 0;
    lv_ps_bcn_loss_flag_after_wakeup = 0;
    lv_ps_wakeup_mac_timepoint = 0;
    lv_ps_bcn_delay_duration = 0;
    lv_ps_bcn_frame_duration = 0;
    lv_ps_bcn_rxd_local_time = 0;
    lv_ps_tbtt_to_rxd_time = 0;
#if (AFTER_MISSING_STRATEGY == WAIT_UNTIL_RECVED)
    lv_ps_loss_bcn_count = 0;
#endif
    lv_ps_pre_lead_wakeup_duration = 0;
    lv_ps_win_pri_compensation_factor = 0;
    lv_ps_win_post_compensation_factor = 0;
#if (AFTER_MISSING_STRATEGY == WAIT_ONCE)
    lv_ps_bcn_has_been_waiting = 0;
#endif
    lv_ps_bcn_cont_miss_bcn_count = 0;
    lv_ps_last_beacon_rev_timepoint = 0;
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
    PS_INFO_T ps_info_backup;
    memcpy(&ps_info_backup,&ps_info,sizeof(PS_INFO_T));
    memset(&ps_info,0,sizeof(PS_INFO_T));
    ps_info.ps_print_enable = ps_info_backup.ps_print_enable;
    ps_info.ps_print_period = ps_info_backup.ps_print_period;
    ps_info.ps_statistical_period = ps_info_backup.ps_statistical_period;
    ps_info.ps_arp_enable = ps_info_backup.ps_arp_enable;
    ps_info.ps_arp_period = ps_info_backup.ps_arp_period;
#endif
    GLOBAL_INT_RESTORE();
}

uint64_t lv_ps_wakeup_set_timepoint(void)
{
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
    if(lv_ps_wakeup_mac_timepoint ==0)
    {
        ps_info.ps_start_time = cal_get_time_us();
        ps_info.ps_last_print_time = ps_info.ps_start_time;
        ps_info.ps_last_statistical_time = ps_info.ps_start_time;
//        power_save_print_timer_start();
//        power_save_statistical_timer_start();
    }
    ps_info.sleep_count_in_total ++;
    if(lv_ps_beacon_cnt_after_wakeup <= 1)
        ps_info.sleep_count_in_total_without_receive_data ++;
    ps_info.sleep_count_in_current_statistical_period++;
#endif
    lv_ps_wakeup_mac_timepoint = cal_get_time_us();

    return lv_ps_wakeup_mac_timepoint;
}

uint32_t lv_ps_set_bcn_int(uint32_t interval)
{
	lv_ps_beacon_interval = interval;

	return interval;
}

void lv_ps_set_bcn_data(uint64_t bcn_tsf, uint32_t bcn_int,
		uint32_t duration_of_frame, uint32_t duration_to_timestamp)
{
	uint64_t tbtt_tsf = (bcn_tsf / bcn_int) * bcn_int;

	lv_ps_bcn_tsf_field = bcn_tsf;
	lv_ps_beacon_interval = bcn_int;
	lv_ps_bcn_delay_duration = (int32_t)(bcn_tsf - tbtt_tsf - duration_to_timestamp);
	lv_ps_bcn_frame_duration = duration_of_frame;
}

void lv_ps_set_bcn_timing(uint64_t local_time, uint64_t duration_tbtt_to_rxd)
{
	lv_ps_bcn_rxd_local_time = local_time;
	if (duration_tbtt_to_rxd < LV_PS_TBTT_TO_RXD_MAX)
		lv_ps_tbtt_to_rxd_time = (uint32_t)duration_tbtt_to_rxd;
	else
    {
        lv_ps_tbtt_to_rxd_time = lv_ps_bcn_frame_duration + lv_ps_bcn_delay_duration + LV_PS_NORMAL_BCN_RX_OFFSET;
    }
}

uint32_t lv_ps_recv_beacon(void)
{
	lv_ps_beacon_cnt_after_wakeup ++;

#if (AFTER_MISSING_STRATEGY == WAIT_UNTIL_RECVED)
	lv_ps_loss_bcn_count = 0;
#endif

	lv_ps_last_beacon_rev_timepoint = cal_get_time_us();
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
	if((lv_ps_beacon_cnt_after_wakeup == 1)&&(lv_ps_wakeup_mac_timepoint != 0))
	{
		ps_info.wakeup_to_beacon_time_in_total += (lv_ps_last_beacon_rev_timepoint - lv_ps_wakeup_mac_timepoint);
	}
#endif
	return lv_ps_beacon_cnt_after_wakeup;
}

uint32_t lv_ps_set_start_flag(void)
{
	if(ps_may_sleep())
	{
	    os_null_printf("nxmac_tsf_mgt_enable:0x%x\r\n", nxmac_mac_cntrl_1_get());
		nxmac_tsf_mgt_disable_setf(0);

		lv_ps_start_flag += 1;
	}

	return 0;
}

void lv_ps_clear_start_flag(void)
{
	lv_ps_start_flag = 0;
}

uint32_t lv_ps_get_start_flag(void)
{
	return lv_ps_start_flag;
}

uint32_t lv_ps_is_super_anchor_point(void)
{
	return ((lv_anchor_flag > 2) ? 1:0);
}

void lv_ps_set_anchor_point(void)
{
	lv_anchor_flag += 1;
}

void lv_ps_clear_anchor_point(void)
{
	lv_anchor_flag = 0;
	lv_ps_beacon_cnt_after_wakeup = 0;
	lv_ps_bcn_loss_flag_after_wakeup = 0;
}

uint32_t lv_ps_is_got_anchor_point(void)
{
	return (0 != lv_anchor_flag);
}

uint32_t lv_ps_get_sleep_duration(void)
{
	return lv_ps_current_sleep_duration;
}

uint32_t lv_ps_beacon_missing_handler(void)
{
	lv_ps_bcn_loss_flag_after_wakeup = 1;
#if (AFTER_MISSING_STRATEGY == WAIT_UNTIL_RECVED)
	lv_ps_loss_bcn_count ++;
#endif
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
	ps_info.beacon_missing_count_in_total++;
	ps_info.beacon_missing_count_in_current_statistical_period++;
	ps_info.wakeup_to_beacon_time_in_total += cal_get_time_us() - lv_ps_wakeup_mac_timepoint;
#endif
	lvc_general_sleep_flag = 0;

	lv_ps_set_anchor_point();

	lv_ps_check_beacon_loss();

	return 0;
}

uint32_t lv_ps_get_keep_timer_duration(void)
{
	uint32_t value;

//    bk_printf ("%d, %d\r\n",lv_ps_pre_lead_wakeup_duration,lv_ps_win_post_compensation_factor);
//	ASSERT(lv_ps_pre_lead_wakeup_duration);
	value = (lv_ps_pre_lead_wakeup_duration + PS_KEEP_TIMER_VALID_DURATION_MS * 1000
				+ lv_ps_win_post_compensation_factor * CELL_DURATION
				- DURATION_WAKEUP_STABILIZATION_US) / 1000;
	return value;
}

#define LVPS_DURA_CALC_DEBUG	0

#if LVPS_DURA_CALC_DEBUG
#define LVPS_DATA_TBL_SIZE	64
struct lvps_dura_calc_s {
#define LVPS_F_LEAD_OVER_WKUP_TIME	0x10
#define LVPS_F_TSF_OVER_BCN_INT		0x20
#define LVPS_F_DURATION_ZERO		0x40
#define LVPS_F_WAIT_BCMC		0x80
	uint32_t flags;
	uint32_t pri_tagt;
	uint32_t tot_time;
	uint32_t pri_tbtt;
	uint32_t post_tbtt;
	int32_t bcn_delay;
	uint32_t tbtt_rxd;
	uint32_t post_rxd;
	int32_t next_lead;
};
struct lvps_dura_calc_s lvps_dura_data[LVPS_DATA_TBL_SIZE];
uint32_t lvps_dura_index = 0;
uint32_t lvps_debug_count = 0;
int32_t g_prev_duration_target_lead = 8000;

void dump_lvps_dura_data(void)
{
	int i;
	struct lvps_dura_calc_s *p_dura_calc_data;
	uint32_t pri_tbtt;
	uint32_t tot_cnt = 0, devi_cnt = 0;
	uint32_t tot_sum = 0, devi_sum = 0;
	uint32_t tot_avg = 0, devi_avg = 0;
	uint32_t tot_max = 0, tot_min = 0xFFFFFFFF;
	uint32_t devi_max = 0, devi_min = 0xFFFFFFFF;
	uint32_t lead_devi;
	int32_t lead_devi_orig;

	for (i = 0; i < LVPS_DATA_TBL_SIZE; i++) {
		p_dura_calc_data = &lvps_dura_data[i];
		pri_tbtt = p_dura_calc_data->tot_time - p_dura_calc_data->post_tbtt;
		if (i != 0) {
			tot_cnt++;
			tot_sum += p_dura_calc_data->tot_time;
			if (tot_max < p_dura_calc_data->tot_time)
				tot_max = p_dura_calc_data->tot_time;
			if (tot_min > p_dura_calc_data->tot_time)
				tot_min = p_dura_calc_data->tot_time;
		}
		if (p_dura_calc_data->pri_tagt) {
			devi_cnt++;
			lead_devi_orig = pri_tbtt - p_dura_calc_data->pri_tagt;
			lead_devi = (lead_devi_orig <= 0)? (0 - lead_devi_orig) : lead_devi_orig;
			devi_sum += lead_devi;
			if (lead_devi > devi_max)
				devi_max = lead_devi;
			if (lead_devi < devi_min)
				devi_min = lead_devi;
		} else {
			lead_devi = 0;
			lead_devi_orig = 0;
		}
		bk_printf("=== %u: flags %08x, total %u, prit %u, tagt %u, diff %d, post %u, nlead %d;",
				i,
				p_dura_calc_data->flags,
				p_dura_calc_data->tot_time,
				pri_tbtt,
				p_dura_calc_data->pri_tagt,
				lead_devi_orig,
				p_dura_calc_data->post_tbtt,
				p_dura_calc_data->next_lead);
		bk_printf(" bcn_dly %d, post_rx %u\r\n",
				p_dura_calc_data->bcn_delay,
				p_dura_calc_data->post_rxd);
	}
	if (devi_cnt)
		devi_avg = devi_sum / devi_cnt;
	tot_avg = tot_sum / tot_cnt;

	bk_printf("=== tot: sum %u, cnt %u, max %u, min %u, avg %u\r\n",
			tot_sum, tot_cnt, tot_max, tot_min, tot_avg);
	bk_printf("=== devi: sum %u, cnt %u, max %u, min %u, avg %u\r\n",
			devi_sum, devi_cnt, devi_max, devi_min, devi_avg);
	bk_printf("==================================================\r\n");
	bk_printf("==================================================\r\n");
	bk_printf("==================================================\r\n");
}
extern int32_t g_duration_target_lead;
#endif

uint32_t lv_ps_calc_sleep_duration(void)
{
	uint32_t distance_2_prv_tbtt = 0;
#if LVPS_DURA_CALC_DEBUG
	uint32_t case_type = 0;
#endif
	int32_t lead_value;
	int32_t duration;
	int32_t delta_time;
	uint64_t curr_local_time;
	uint32_t listen_interval = PS_DTIM_COUNT;

#if LVPS_DURA_CALC_DEBUG
	if (lvps_dura_index == 0 && lvps_debug_count) {
		GLOBAL_INT_DECLARATION();
		GLOBAL_INT_DISABLE();
		lv_ps_enable_print = 1;
		dump_lvps_dura_data();
		lv_ps_enable_print = 0;
		GLOBAL_INT_RESTORE();
	}
	struct lvps_dura_calc_s *p_dura_calc_data = &lvps_dura_data[lvps_dura_index];
	p_dura_calc_data->flags = 0;
#endif
#if CFG_USE_STA_PS
	listen_interval = power_save_get_listen_int();
#endif

	curr_local_time = cal_get_time_us();
	if (lv_ps_beacon_cnt_after_wakeup) {
		/*The first case: recv beacon after wakeup of low voltage*/
		if (lv_ps_bcn_cont_miss_bcn_count)
			lv_ps_win_pri_compensation_factor = 1;
		else
			lv_ps_win_pri_compensation_factor = 0;
		lv_ps_win_post_compensation_factor = 0;

		distance_2_prv_tbtt = (curr_local_time - lv_ps_bcn_rxd_local_time) + lv_ps_tbtt_to_rxd_time;
#if LVPS_DURA_CALC_DEBUG
		case_type = 1;

		if (distance_2_prv_tbtt > lv_ps_beacon_interval) {
			case_type = 2;
		}

		if (lv_ps_bcn_loss_flag_after_wakeup) {
			case_type = 3;
		}
		delta_time = curr_local_time - lv_ps_wakeup_mac_timepoint;
		if ((case_type == 1 || case_type == 3)
				&& p_dura_calc_data->flags == 0
				&& !lv_ps_bcn_cont_miss_bcn_count && lvps_dura_index) {
			p_dura_calc_data->pri_tagt = g_prev_duration_target_lead;
		} else {
			p_dura_calc_data->pri_tagt = 0;
		}
		p_dura_calc_data->post_tbtt = distance_2_prv_tbtt;
#endif

		lead_value = lvc_get_lead_duration();
		lv_ps_pre_lead_wakeup_duration = lead_value + LEAD_FORCE_TIME +
						lv_ps_win_pri_compensation_factor * CELL_DURATION;
		distance_2_prv_tbtt = distance_2_prv_tbtt % lv_ps_beacon_interval;
		duration = (32 * (listen_interval * lv_ps_beacon_interval - distance_2_prv_tbtt
									- lv_ps_pre_lead_wakeup_duration) / 1000);

		lvc_general_sleep_flag = 1;
#if LVPS_DURA_CALC_DEBUG
		p_dura_calc_data->bcn_delay = lv_ps_bcn_delay_duration;
		p_dura_calc_data->tbtt_rxd = lv_ps_tbtt_to_rxd_time;
		p_dura_calc_data->post_rxd = curr_local_time - lv_ps_bcn_rxd_local_time;
#endif
#if (AFTER_MISSING_STRATEGY == WAIT_ONCE)
		lv_ps_bcn_has_been_waiting = 0;
#endif
		lv_ps_bcn_cont_miss_bcn_count = 0;
	} else {

#if LVPS_DURA_CALC_DEBUG
		/*The case: beacon is missing all the while after wakeup of low voltage*/
		case_type = 4;
#endif
		delta_time = curr_local_time - lv_ps_wakeup_mac_timepoint;
		if (lv_ps_pre_lead_wakeup_duration > delta_time) {
			distance_2_prv_tbtt = 0;
#if LVPS_DURA_CALC_DEBUG
			p_dura_calc_data->flags |= LVPS_F_LEAD_OVER_WKUP_TIME;
#endif
		} else {
			distance_2_prv_tbtt = delta_time - lv_ps_pre_lead_wakeup_duration;
		}

#if LVPS_DURA_CALC_DEBUG
		p_dura_calc_data->post_tbtt = distance_2_prv_tbtt;
		if (distance_2_prv_tbtt >= lv_ps_beacon_interval) {
			p_dura_calc_data->flags |= LVPS_F_TSF_OVER_BCN_INT;
		}
#endif

		distance_2_prv_tbtt = distance_2_prv_tbtt % lv_ps_beacon_interval;

		lead_value = lvc_get_lead_duration();

#if (AFTER_MISSING_STRATEGY == WAIT_UNTIL_RECVED)
		if (lv_ps_loss_bcn_count < 5) {
			lv_ps_win_pri_compensation_factor = lv_ps_loss_bcn_count;
		} else {
			lv_ps_win_pri_compensation_factor = 10;
            bk_printf("bcn loss cnt = %d, line = %d\r\n",lv_ps_loss_bcn_count,__LINE__);
		}
		lv_ps_win_post_compensation_factor = lv_ps_win_pri_compensation_factor;

		lv_ps_pre_lead_wakeup_duration = lead_value + LEAD_FORCE_TIME + lv_ps_win_pri_compensation_factor * CELL_DURATION;
		duration = (32 * (DTIM_COUNT_WHEN_MISSING_BEACON * lv_ps_beacon_interval - distance_2_prv_tbtt
										- lv_ps_pre_lead_wakeup_duration) / 1000);

#elif (AFTER_MISSING_STRATEGY == WAIT_ONCE_ON_CONT_LOSS)
		if (lv_ps_bcn_cont_miss_bcn_count == 0) {
			lv_ps_win_pri_compensation_factor = 2;
			lv_ps_win_post_compensation_factor = 2;
			lv_ps_pre_lead_wakeup_duration = lead_value + LEAD_FORCE_TIME + lv_ps_win_pri_compensation_factor * CELL_DURATION;
			duration = (32 * (listen_interval * lv_ps_beacon_interval - distance_2_prv_tbtt
										- lv_ps_pre_lead_wakeup_duration) / 1000);

		PS_DBG("%d,%d,%d,%d,%d line = %d\r\n", duration, distance_2_prv_tbtt,lv_ps_pre_lead_wakeup_duration,lead_value,lv_ps_win_pri_compensation_factor,__LINE__);
		} else if (lv_ps_bcn_cont_miss_bcn_count == 1) {
			lv_ps_win_pri_compensation_factor = 4;
			lv_ps_win_post_compensation_factor = 4;
			lv_ps_pre_lead_wakeup_duration = lead_value + LEAD_FORCE_TIME + lv_ps_win_pri_compensation_factor * CELL_DURATION;
			duration = (32 * (DTIM_COUNT_WHEN_MISSING_BEACON * lv_ps_beacon_interval - distance_2_prv_tbtt
										- lv_ps_pre_lead_wakeup_duration) / 1000);

			PS_DBG("%d,%d,%d,%d,%d line = %d\r\n", duration, distance_2_prv_tbtt,lv_ps_pre_lead_wakeup_duration,lead_value,lv_ps_win_pri_compensation_factor,__LINE__);
		} else {
			if (lv_ps_bcn_cont_miss_bcn_count & 0x1) {
				lv_ps_win_pri_compensation_factor = 2;
				lv_ps_win_post_compensation_factor = 2;
			} else {
				lv_ps_win_pri_compensation_factor = 4;
				lv_ps_win_post_compensation_factor = 0;
			}
			lv_ps_pre_lead_wakeup_duration = lead_value + LEAD_FORCE_TIME + lv_ps_win_pri_compensation_factor * CELL_DURATION;
			duration = (32 * (listen_interval * lv_ps_beacon_interval - distance_2_prv_tbtt
										- lv_ps_pre_lead_wakeup_duration) / 1000);

		PS_DBG("%d,%d,%d,%d,%d line = %d\r\n", duration, distance_2_prv_tbtt,lv_ps_pre_lead_wakeup_duration,lead_value,lv_ps_win_pri_compensation_factor,__LINE__);
		}
#elif (AFTER_MISSING_STRATEGY == WAIT_ONCE)
		lv_ps_win_pri_compensation_factor = 4;
		lv_ps_win_post_compensation_factor = 4;
		lv_ps_pre_lead_wakeup_duration = lead_value + LEAD_FORCE_TIME + lv_ps_win_pri_compensation_factor * CELL_DURATION;
		if (0 == lv_ps_bcn_has_been_waiting) {
			duration = (32 * (DTIM_COUNT_WHEN_MISSING_BEACON * lv_ps_beacon_interval - distance_2_prv_tbtt
										- lv_ps_pre_lead_wakeup_duration) / 1000);
			lv_ps_bcn_has_been_waiting = 1;
	        } else {
			duration = (32 * (listen_interval * lv_ps_beacon_interval - distance_2_prv_tbtt
										- lv_ps_pre_lead_wakeup_duration) / 1000);
			lv_ps_bcn_has_been_waiting = 0;
		}
#elif (AFTER_MISSING_STRATEGY == NO_WAIT)
		lv_ps_win_pri_compensation_factor = 4;
		lv_ps_win_post_compensation_factor = 4;
		lv_ps_pre_lead_wakeup_duration = lead_value + LEAD_FORCE_TIME + lv_ps_win_pri_compensation_factor * CELL_DURATION;
		duration = (32 * (listen_interval * lv_ps_beacon_interval - distance_2_prv_tbtt
							- lv_ps_pre_lead_wakeup_duration) / 1000);
#endif

		lvc_general_sleep_flag = 0;
		lv_ps_bcn_cont_miss_bcn_count++;

#if LVPS_DURA_CALC_DEBUG
		p_dura_calc_data->bcn_delay = 0;
		p_dura_calc_data->tbtt_rxd = 0;
		p_dura_calc_data->post_rxd = 0;
		p_dura_calc_data->pri_tagt = 0;
#endif
	}

	if (duration < 0) {
		duration = 0;
#if LVPS_DURA_CALC_DEBUG
		p_dura_calc_data->flags |= LVPS_F_DURATION_ZERO;
#endif
	}
	lv_ps_current_sleep_duration = duration;

#if LVPS_DURA_CALC_DEBUG
	g_prev_duration_target_lead = g_duration_target_lead;
	p_dura_calc_data->next_lead = lv_ps_pre_lead_wakeup_duration;
	p_dura_calc_data->flags |= case_type;
	p_dura_calc_data->tot_time = delta_time;
	lvps_dura_index = (++lvps_dura_index) % LVPS_DATA_TBL_SIZE;
	lvps_debug_count++;
#endif

	return duration;
}
extern UINT8 power_save_set_all_vif_prevent_sleep ( UINT32 prevent_bit );

static uint32_t lv_ps_check_beacon_loss(void)
{
    uint64_t current_timepoint = 0, loss_during;

    if(lv_ps_get_start_flag() == 0)
        return 0;

#if (AFTER_MISSING_STRATEGY == WAIT_UNTIL_RECVED)
    if(lv_ps_loss_bcn_count <= 1)
        return 0;
#endif

    current_timepoint = cal_get_time_us();

    if(current_timepoint > lv_ps_last_beacon_rev_timepoint) {
        loss_during = current_timepoint - lv_ps_last_beacon_rev_timepoint;
    } else {
//        loss_during = (0xffffffffffffffffu - lv_ps_last_beacon_rev_timepoint) + current_timepoint;
        ASSERT(0);
    }
    loss_during = loss_during / 1000000;

    //os_printf("loss: %u, %u\r\n", loss_during, LV_PS_BEACON_LOSS_TIME_S);
    if (loss_during >= LV_PS_BEACON_LOSS_TIME_S) {
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
        ps_info.connection_loss_flag = 1;
        ps_info.connection_loss_count ++;
        ps_info.connection_loss_time_start = cal_get_time_us();
#endif
        ps_send_connection_loss();
        os_printf("low voltage detect beacon loss\r\n");

        return 1;
    }

    return 0;
}

void us_to_readable_value(uint64_t us,uint32_t * h,uint32_t *m,uint32_t *s)
{
    *h = us/1000000/3600;//us->s->hour
    *m = us/1000000%3600/60;//min
    *s = us/1000000%60;//second
}
// eof

