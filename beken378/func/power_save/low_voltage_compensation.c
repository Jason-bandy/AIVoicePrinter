#include "low_voltage_compensation.h"
#include "power_save_pub.h"
#include "power_save.h"
#include "calendar_pub.h"
#include "low_voltage_ps.h"

#define INIT_TARGET_LEAD_VALUE_US               (8 * 1000)

#if SYS_CTRL_USE_VDD_BULK
#define GOAL_TARGET_LEAD_VALUE_US               (3000)
#define GOAL_TARGET_LEAD_VALUE_US_LITTLE_DTIM   (2800)   //(2300)
#else
#if ( 1 == CFG_LOW_VOLTAGE_PS_32K_DIV)
#define GOAL_TARGET_LEAD_VALUE_US               (2000)
#define GOAL_TARGET_LEAD_VALUE_US_LITTLE_DTIM     (1800)
#else
#define GOAL_TARGET_LEAD_VALUE_US               (2500)
#define GOAL_TARGET_LEAD_VALUE_US_LITTLE_DTIM     (2300)
#endif
#endif
uint32_t g_bundle_id = 0;
BCN_BUNDLE_T g_bundles[BUNDLE_MAX_COUNT] = {{0}};
uint32_t lvc_general_sleep_flag = 0;
int32_t g_duration_target_lead = INIT_TARGET_LEAD_VALUE_US;
int32_t g_duration_config_lead = INIT_TARGET_LEAD_VALUE_US;

uint32_t lvc_calc_compensation(void);
void lvc_init(void)
{
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    lvc_general_sleep_flag = 0;
    g_duration_target_lead = INIT_TARGET_LEAD_VALUE_US;
    g_duration_config_lead = INIT_TARGET_LEAD_VALUE_US;

    memset(g_bundles, 0, sizeof(BCN_BUNDLE_T) * BUNDLE_MAX_COUNT);
    g_bundle_id = 0;
    GLOBAL_INT_RESTORE();
}

static int32_t lvc_get_targe_lead_value(void)
{
	uint32_t listen_interval = PS_DTIM_COUNT;
	int32_t duration_of_lead;

#if CFG_USE_STA_PS
	listen_interval = power_save_get_listen_int();
#endif

	if(listen_interval > 15) {
		duration_of_lead = GOAL_TARGET_LEAD_VALUE_US;
	} else {
		duration_of_lead = GOAL_TARGET_LEAD_VALUE_US_LITTLE_DTIM;
	}

	return duration_of_lead;
}

int32_t lvc_get_lead_duration(void)
{
	return g_duration_config_lead;
}

extern int32_t lv_ps_pre_lead_wakeup_duration;
uint32_t lvc_record_into_item(BCN_ITEM_T *item, uint64_t delta_wakeup, uint64_t delta_tbtt)
{
	ASSERT(item);

	item->wakeup_time_up_to_bcn =(uint32_t) delta_wakeup;
	item->wakeup_time_up_to_tbtt =(int32_t) (delta_wakeup - delta_tbtt);
#if SMOOTHED_LEAD_VALUE_CALC
	item->duration_clock_drift = lv_ps_pre_lead_wakeup_duration - item->wakeup_time_up_to_tbtt;
#endif

	return LVC_SUCCESS;
}

uint32_t lvc_record_into_bundle(uint64_t delta_wakeup, uint64_t delta_tbtt)
{
	uint32_t ret = LVC_SUCCESS;
	uint32_t bundle_index;
	uint32_t item_id;
	BCN_BUNDLE_T *bundle_ptr;
	BCN_ITEM_T *item_ptr;

	bundle_index = g_bundle_id % BUNDLE_MAX_COUNT;
	bundle_ptr = &g_bundles[bundle_index];
	item_id = bundle_ptr->item_index;
	item_ptr = &bundle_ptr->lv_item[item_id];

	ASSERT(ITEM_COUNT_IN_BUNDLE > item_id);
	lvc_record_into_item(item_ptr, delta_wakeup, delta_tbtt);

	item_id += 1;
	if(ITEM_COUNT_IN_BUNDLE == item_id)
	{
		bundle_ptr->item_index = 0;
		ret = LVC_CELL_FULL;
	}
	else
	{
		bundle_ptr->item_index = item_id;
	}

	LV_PSC_NULL_PRT("item_id:%d\r\n", item_id);

	return ret;
}

#if LV_PS_BUNDLE_DEBUG
void lvc_dump_rec_info(void)
{
	int i, j;
	BCN_BUNDLE_T *p_buldle;
	BCN_ITEM_T *p_item;
	uint32_t count = 0;
	uint32_t sum = 0;
	int32_t devi;
	int32_t max_devi = 0;
	int32_t min_devi = 0x7fffffff;

	for (i = 0; i < BUNDLE_MAX_COUNT; i ++) {
		p_buldle = &g_bundles[i];

		bk_printf("    ======================================================================\r\n");
		bk_printf("    [%d] - target:%d, config:%d, compen %d, spur %d\r\n",
				i,
				p_buldle->duration_of_lead,
				p_buldle->duration_of_setting,
				p_buldle->duration_of_compensation,
				p_buldle->drift_spur_detected);
		for (j = 0; j < ITEM_COUNT_IN_BUNDLE; j ++) {
			p_item = &p_buldle->lv_item[j];
			devi = (int32_t)(p_item->wakeup_time_up_to_tbtt - p_buldle->duration_of_lead);
#if SMOOTHED_LEAD_VALUE_CALC
			bk_printf("        [%d] - total %u, lead %d, devi %d, clk_drift %d\r\n",
					j,
					p_item->wakeup_time_up_to_bcn,
					p_item->wakeup_time_up_to_tbtt,
					devi,
					p_item->duration_clock_drift);
#else
			bk_printf("        [%d] - total %u, lead %d, devi %d\r\n",
					j,
					p_item->wakeup_time_up_to_bcn,
					p_item->wakeup_time_up_to_tbtt,
					devi);
#endif
			if (p_buldle->duration_of_lead) {
				count++;
				if (devi < 0)
					devi = 0 - devi;
				sum += devi;
				if (devi > max_devi)
					max_devi = devi;
				if (devi < min_devi)
					min_devi = devi;
			}
		}
	}

	if (count) {
		bk_printf("    devi: count %u, max %u, min %u, sum %u, avg %u\r\n", count, max_devi, min_devi, sum, sum / count);
	}
	bk_printf("    ======================================================================\r\n");
	bk_printf("    ======================================================================\r\n");
}
#endif

extern uint32_t lv_ps_enable_print;
uint32_t lvc_record_delta_time_info(uint64_t delta_wakeup, uint64_t delta_tbtt)
{
	if(LVC_CELL_FULL == lvc_record_into_bundle(delta_wakeup, delta_tbtt))
	{
//		os_printf("lvc_record_delta_time_info(), line = %d\r\n", __LINE__);
		/*calc compensation value*/
		lvc_calc_compensation();

		g_bundle_id ++;
		LV_PSC_NULL_PRT("g_bundle_id:%d\r\n", g_bundle_id);

#if LV_PS_BUNDLE_DEBUG
		if ((g_bundle_id % BUNDLE_MAX_COUNT) == 0) {
			lv_ps_enable_print = 1;
			lvc_dump_rec_info();
			lv_ps_enable_print = 0;
		}
#endif
	}

	return LVC_SUCCESS;
}

#if SMOOTHED_LEAD_VALUE_CALC

#define SMOOTHED_DEPTH		3

#if 1

#define SMOOTHED_DRIFT_SPUR_THRESH	800

int32_t lvc_calc_smoothed_clock_drift(void)
{
	int32_t bundle_index;
//	int32_t weight;
	int32_t clock_drift;
	int32_t diff;
	int32_t i;
	int32_t smoothed_clock_drift = 0;
#if LV_PS_BUNDLE_DEBUG
	BCN_BUNDLE_T *bundle_ptr;
#endif
	for (i = 0; i <= SMOOTHED_DEPTH; i++) {
		bundle_index = (g_bundle_id - i) % BUNDLE_MAX_COUNT;
		smoothed_clock_drift += g_bundles[bundle_index].lv_item[0].duration_clock_drift;
		if (i == 0) {
			clock_drift = g_bundles[bundle_index].lv_item[0].duration_clock_drift;
#if LV_PS_BUNDLE_DEBUG
			bundle_ptr = &g_bundles[bundle_index];
#endif
		}
	}
	smoothed_clock_drift /= (SMOOTHED_DEPTH + 1);
	diff = (smoothed_clock_drift > clock_drift) ? (smoothed_clock_drift - clock_drift)
							: (clock_drift - smoothed_clock_drift);
	if (diff > SMOOTHED_DRIFT_SPUR_THRESH) {
		if (smoothed_clock_drift > clock_drift)
			smoothed_clock_drift += (SMOOTHED_DRIFT_SPUR_THRESH >> 2);
		else if (lv_ps_pre_lead_wakeup_duration == g_duration_config_lead)
			/* don't reduce the lead vaue if there is a beacon loss just now */
			smoothed_clock_drift -= (SMOOTHED_DRIFT_SPUR_THRESH >> 2);
#if LV_PS_BUNDLE_DEBUG
		bundle_ptr->drift_spur_detected = 1;
#endif
	} else {
#if LV_PS_BUNDLE_DEBUG
		bundle_ptr->drift_spur_detected = 0;
#endif
	}

	return smoothed_clock_drift;
}

#else

#define SMOOTHED_KEEP_WEIGHT_LO			50
#define SMOOTHED_KEEP_WEIGHT_HI			80
#define SMOOTHED_DRIFT_SPUR_THRESH		1000
#define INVALID_CLOCK_DRIFT			0x7FFFFFFF

static int32_t g_smoothed_weight_orig[SMOOTHED_DEPTH] = {50, 30, 20};
static int32_t g_smoothed_clock_drift = INVALID_CLOCK_DRIFT;

int32_t lvc_calc_smoothed_clock_drift(void)
{
	int32_t bundle_index;
	int32_t weight;
	int32_t clock_drift;
	uint32_t diff;
	uint8_t i;

	if (g_smoothed_clock_drift == INVALID_CLOCK_DRIFT) {
		clock_drift = 0;
		for (i = 0; i < SMOOTHED_DEPTH; i++) {
			bundle_index = (g_bundle_id - i) % BUNDLE_MAX_COUNT;
			clock_drift += g_bundles[bundle_index].lv_item[0].duration_clock_drift * g_smoothed_weight_orig[i];
		}
		g_smoothed_clock_drift = clock_drift / 100;
	} else {
		bundle_index = g_bundle_id % BUNDLE_MAX_COUNT;
		clock_drift = g_bundles[bundle_index].lv_item[0].duration_clock_drift;
		diff = (g_smoothed_clock_drift > clock_drift) ? (g_smoothed_clock_drift - clock_drift)
								: (clock_drift - g_smoothed_clock_drift);
		weight = (diff > SMOOTHED_DRIFT_SPUR_THRESH) ? SMOOTHED_KEEP_WEIGHT_HI : SMOOTHED_KEEP_WEIGHT_LO;
		g_smoothed_clock_drift = ((clock_drift * (100 - weight)) + (g_smoothed_clock_drift * weight)) / 100;
	}

	return g_smoothed_clock_drift;
}

#endif

uint32_t lvc_calc_compensation(void)
{
	int32_t duration_to_config_lead;
	int32_t duration_target_lead;
	int32_t smoothed_clock_drift;
#if LV_PS_BUNDLE_DEBUG
	uint32_t bundle_index = g_bundle_id % BUNDLE_MAX_COUNT;
	BCN_BUNDLE_T *bundle_ptr = &g_bundles[bundle_index];
#endif

	if (g_bundle_id < SMOOTHED_DEPTH)
		return LVC_SUCCESS;

	/* update global parameters*/
	smoothed_clock_drift = lvc_calc_smoothed_clock_drift();
	duration_target_lead = lvc_get_targe_lead_value();
	duration_to_config_lead = duration_target_lead + smoothed_clock_drift;

#if LV_PS_BUNDLE_DEBUG
	bundle_ptr->duration_of_setting = g_duration_config_lead;
	bundle_ptr->duration_of_lead = g_duration_target_lead;
	bundle_ptr->duration_of_compensation = smoothed_clock_drift;
#endif

	if (0) { //duration_to_config_lead < 0) {
		g_duration_config_lead = INIT_TARGET_LEAD_VALUE_US;
		g_duration_target_lead = INIT_TARGET_LEAD_VALUE_US;
		g_bundle_id = 0;
		LV_PSC_NULL_PRT("lead recover\r\n");
	} else {
		g_duration_config_lead =  duration_to_config_lead;
		g_duration_target_lead = duration_target_lead;
	}

	return LVC_SUCCESS;
}

#else

uint32_t lvc_calc_average_lead_value(BCN_BUNDLE_T * bundle_ptr)
{
	uint32_t value, i, sum = 0;

	for(i = 0; i < ITEM_COUNT_IN_BUNDLE; i ++)
	{
		sum += bundle_ptr->lv_item[i].wakeup_time_up_to_tbtt;
	}

	value = sum / ITEM_COUNT_IN_BUNDLE;

	return value;
}

uint32_t lvc_calc_compensation(void)
{
	int32_t duration_to_compensate;
	uint32_t cur_average_lead;
	int32_t duration_to_config_lead;
	int32_t duration_target_lead;
	uint32_t bundle_index = g_bundle_id % BUNDLE_MAX_COUNT;
	BCN_BUNDLE_T *bundle_ptr = &g_bundles[bundle_index];

	duration_target_lead = lvc_get_targe_lead_value();
	cur_average_lead = lvc_calc_average_lead_value(bundle_ptr);
	duration_to_compensate = ((int32_t)((cur_average_lead - g_duration_target_lead) * 3) >> 2);
	duration_to_config_lead = g_duration_config_lead - (g_duration_target_lead - duration_target_lead) - duration_to_compensate;

#if LV_PS_BUNDLE_DEBUG
	bundle_ptr->duration_of_setting = g_duration_config_lead;
	bundle_ptr->duration_of_lead = g_duration_target_lead;
	bundle_ptr->duration_of_compensation = duration_to_compensate;
#endif

	/* update global parameters*/
	g_duration_config_lead = duration_to_config_lead;
	g_duration_target_lead = duration_target_lead;

	if ((int32_t)g_duration_config_lead < 0) {
		g_duration_config_lead = INIT_TARGET_LEAD_VALUE_US;
		g_duration_target_lead = INIT_TARGET_LEAD_VALUE_US;
		g_bundle_id = 0;
		LV_PSC_NULL_PRT("lead recover\r\n");
	}

	return LVC_SUCCESS;
}
#endif

extern uint32_t lv_ps_beacon_interval;
uint32_t lvc_recv_bcn_handler(uint64_t tsf)
{
	uint64_t delta_tbtt;
	uint64_t delta_wakeup;
	uint64_t duration;
	uint64_t tbtt_tp;
	uint64_t current_tsf_timer_point;
	uint64_t local_time;
	GLOBAL_INT_DECLARATION();

	current_tsf_timer_point = (uint64_t)nxmac_tsf_lo_get() + ((uint64_t)nxmac_tsf_hi_get() << 32);
	local_time = cal_get_time_us();
	delta_wakeup = local_time - lv_ps_wakeup_mac_timepoint;//time for mcu wakeup
	tbtt_tp = (tsf / lv_ps_beacon_interval) * lv_ps_beacon_interval;
	duration = current_tsf_timer_point - tbtt_tp;
	lv_ps_set_bcn_timing(local_time, duration);
	if((0 == lv_ps_get_start_flag())
		|| (0 == lvc_general_sleep_flag))
	{
		return LVC_FAILURE;
	}

	lvc_general_sleep_flag = 0;

	/* tsf: beacon timestamp; current_tsf_timer_point: tsf timer's value*/
	if((delta_wakeup > GENERAL_BEACON_INTERVAL_US)
		|| (duration > GENERAL_BEACON_INTERVAL_US))
	{
		LV_PSC_PRT("LVC:0x%llx, 0x%llx\r\n", current_tsf_timer_point, tsf);
		LV_PSC_PRT("lvc:0x%llx, 0x%llx\r\n", delta_wakeup, duration);
		return LVC_FAILURE;
	}

	GLOBAL_INT_DISABLE();
	delta_tbtt = duration ;

	lvc_record_delta_time_info(delta_wakeup, delta_tbtt);

	GLOBAL_INT_RESTORE();

	return LVC_SUCCESS;
}
// eof

