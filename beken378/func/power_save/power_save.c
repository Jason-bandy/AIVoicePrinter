#include "intc_pub.h"
#include "rtos_pub.h"

#include "wdt_pub.h"
#include "gpio_pub.h"
#include "pwm_pub.h"
#include "mem_pub.h"
#include "icu_pub.h"

#include "fake_clock_pub.h"
#include "power_save.h"
#include "target_util_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "arm_arch.h"
#include "rwnx_config.h"
#include "ps.h"
#include "rwnx.h"
#include "uart_pub.h"
#include "mcu_ps_pub.h"
#include "error.h"
#include "start_type_pub.h"
#include "rtos_pub.h"

#if CFG_SUPPORT_BLE
#include "ble_pub.h"
#endif
#include "reg_rc.h"
#include "low_voltage_ps.h"
#include "phy_trident.h"
#include "mcu_ps.h"
#include "calendar_pub.h"

volatile static PS_MODE_STATUS    bk_ps_mode = PS_NO_PS_MODE;
static UINT32 last_wk_tick = 0;
UINT32 last_rw_time = 0;

static STA_PS_INFO bk_ps_info = {
	.ps_dtim_period = 1,
	.ps_dtim_multi = 1,
	.listen_int = PS_DTIM_COUNT,
	.waited_beacon = STA_GET_INIT,
	.sleep_first = 1,
	.ps_can_sleep = 0,
	.ps_real_sleep = 0
};

#if (CFG_SOC_NAME == SOC_BK7231)
static UINT16 r_wakeup_time = 50;
#elif (CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7238)
static UINT16 r_wakeup_time = 90;
#else
static UINT16 r_wakeup_time = 66;
#endif

static UINT32 int_enable_reg_save = 0;
static UINT8 ps_lock = 1;
static PS_FORBID_STATUS bk_forbid_code = 0;
static UINT16 bk_forbid_count = 0;
static UINT32 ps_dis_flag = 0;
static UINT16 beacon_len = 0;

#if CFG_LOW_LATENCY_PS
static UINT8 ps_data_low_latency = 0;
#endif

#if PS_USE_KEEP_TIMER
static beken2_timer_t ps_keep_timer = {0};
static UINT32 ps_keep_timer_status = 0;
static UINT32 ps_wait_timer_status = 0;
static UINT32 ps_keep_timer_period = 0;
static UINT32 ps_reseted_moniter_flag = 0;
static UINT32 ps_bcn_loss_max_count = 0;
static UINT32 ps_keep_timer_flag = 1;
#endif

#if PS_USE_WAIT_TIMER
static beken2_timer_t ps_wait_timer = {0};
#endif

#if PS_USE_KEEP_TIMER
void power_save_keep_timer_handler ( void *data );
#endif
extern void bmsg_null_sender ( void );

int net_if_is_up ( void )
{
	return  ( mhdr_get_station_status() == RW_EVT_STA_GOT_IP );
}


void power_save_wakeup_isr ( void )
{
}

void power_save_dtim_wake ( UINT32 status )
{
	if ( bk_ps_mode == PS_DTIM_PS_MODE &&
	     bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_NONE ) {
		UINT32 reg;

		if ( status ) {
			if ( ( status ) & MAC_ARM_WAKEUP_EN_BIT ) {
				reg = REG_READ ( ICU_INTERRUPT_ENABLE );
				reg &= ~ ( CO_BIT ( FIQ_MAC_WAKEUP ) );
				REG_WRITE ( ICU_INTERRUPT_ENABLE, reg );
				PS_DEBUG_UP_TRIGER;
#if 1
				bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_RW;
				power_save_ieee_dtim_wakeup();
#else
				power_save_ieee_dtim_wakeup();
#endif
			}
		}
	}
}

/*This function will run in mac go to ps fiq,
only an actual emergency can put here,
can't operate wifi tx,rx,modem,rf here*/
void power_save_gops_wait_idle_int_cb ( void )
{
	//rf_ps_wakeup_isr_idle_int_cb();
}

#if CFG_SUPPORT_BLE
extern uint8_t ble_switch_mac_sleeped;
#endif
bool power_save_sleep ( void )
{
	UINT32 ret = false;
	UINT32 reg;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( 1 == bk_ps_info.ps_real_sleep
#if CFG_SUPPORT_BLE
		|| ble_switch_mac_sleeped
#endif
		) {
		GLOBAL_INT_RESTORE();
		return ret;
	}

	if ( ! ( PS_STA_DTIM_CAN_SLEEP ) ) {
		GLOBAL_INT_RESTORE();
		return ret;
	}

	if ( rwnxl_get_status_in_doze() ) {
		GLOBAL_INT_RESTORE();
		return ret;
	}

	nxmac_enable_idle_interrupt_setf ( 1 );
	PS_DEBUG_CK_TRIGER;

	if ( REG_READ ( ( ICU_BASE + 19 * 4 ) )
	     & ( CO_BIT ( FIQ_MAC_TX_RX_MISC )
	         | CO_BIT ( FIQ_MAC_TX_RX_TIMER )
	         | CO_BIT ( FIQ_MAC_RX_TRIGGER )
	         | CO_BIT ( FIQ_MAC_TX_TRIGGER )
	         | CO_BIT ( FIQ_MAC_PROT_TRIGGER )
	       ) ) {
		GLOBAL_INT_RESTORE();
		return ret;
	}

	reg = REG_READ ( ICU_INTERRUPT_ENABLE );
	int_enable_reg_save = reg;
	reg &= ~ ( CO_BIT ( FIQ_MAC_TX_RX_MISC )
	           | CO_BIT ( FIQ_MAC_TX_RX_TIMER )
	           | CO_BIT ( FIQ_MAC_RX_TRIGGER )
	           | CO_BIT ( FIQ_MAC_TX_TRIGGER )
	           | CO_BIT ( FIQ_MAC_GENERAL )
	           | CO_BIT ( FIQ_MAC_PROT_TRIGGER ) );
	REG_WRITE ( ICU_INTERRUPT_ENABLE, reg );
#if (( 1 == CFG_LOW_VOLTAGE_PS)&&(1 == CFG_LOW_VOLTAGE_PS_32K_DIV))
	rc_cntl_stat_set(0x00); //7011
//	REG_WRITE((0x00802800+(20*4)), 0x00);//gpio19
#endif
#if NX_POWERSAVE
	last_rw_time = nxmac_monotonic_counter_2_lo_get();

	if ( last_rw_time == 0xdead5555 ) {
		bk_printf ( "XXXXXXXXXXXXXXXXXXXXXXXX TIME DEAD\r\n" );
	}

	ret = rwnxl_sleep ( power_save_gops_wait_idle_int_cb, power_save_mac_idle_callback );

	if ( false == ret ) {
		PS_PRT ( "can't ps\r\n" );
		REG_WRITE ( ICU_INTERRUPT_ENABLE, int_enable_reg_save );
		GLOBAL_INT_RESTORE();
		return ret;
	}

#endif

	if ( ps_lock )
		ps_lock --;
	else {
		PS_WPRT ( "error ps\r\n" );
		GLOBAL_INT_RESTORE();
		return ret;
	}

	PS_WPRT ( "go ps\r\n" );
#if CFG_USE_STA_PS
#if (1 == CFG_LOW_VOLTAGE_PS)
	lv_ps_calc_sleep_duration();
	sctrl_enable_lvps_rosc_timer();
#endif
	power_save_sleep_status_set();
	sctrl_sta_rf_sleep();
#if (0 == CFG_LOW_VOLTAGE_PS)
	reg = REG_READ ( ICU_INTERRUPT_ENABLE );
	reg |= ( CO_BIT ( FIQ_MAC_WAKEUP ) );
	REG_WRITE ( ICU_INTERRUPT_ENABLE, reg );
#endif
#endif
#if PS_USE_KEEP_TIMER

	if ( 1 == ps_keep_timer_status ) {
		rtos_lock_scheduling();
		bmsg_ps_sender ( PS_BMSG_IOCTL_RF_KP_STOP );
		rtos_unlock_scheduling();
	}

#endif
	GLOBAL_INT_RESTORE();
	return true;
}

/*time = BI*1024*LIST*0.016*/
void power_save_wkup_time_cal ( UINT8 sleep_int )
{
	UINT32 tmp_r_wkup = r_wakeup_time + 12;
	nxmac_radio_wake_up_time_setf ( tmp_r_wkup );
}

int power_save_get_wkup_less_time()
{
	if ( bk_ps_info.listen_mode == PS_LISTEN_MODE_DTIM ) {
		return bk_ps_info.ps_dtim_period * bk_ps_info.ps_dtim_multi \
		       *bk_ps_info.ps_beacon_int * 15;
	}
	else {
		return bk_ps_info.listen_int * bk_ps_info.ps_beacon_int * 15;
	}
}

void power_save_mac_idle_callback ( void )
{
	uint32_t listen_interval = PS_DTIM_COUNT;

	listen_interval = power_save_get_listen_int();
	if ( power_save_if_sleep_first() ) {
		power_save_wkup_time_cal(listen_interval);
		nxmac_tsf_mgt_disable_setf ( 0 );
		nxmac_listen_interval_setf(listen_interval);
		nxmac_atim_w_setf ( 512 );
		nxmac_wake_up_sw_setf ( 0 );
		/*first clear beacon interval,delay,then set beacon interval,to fix rw sleep wakeup time*/
		nxmac_beacon_int_setf ( 0 );
		delay ( 1 );
		nxmac_beacon_int_setf ( bk_ps_info.ps_beacon_int );
		os_printf ( " sleep_first %d\r\n", bk_ps_info.listen_mode );
		os_printf ( " dtim period:%d multi:%d\r\n", bk_ps_info.ps_dtim_period, bk_ps_info.ps_dtim_multi );
		bk_ps_info.sleep_first = 0;
	}
	else {
		if ( bk_ps_info.listen_mode == PS_LISTEN_MODE_DTIM ) {
			power_save_wkup_time_cal(listen_interval);
			nxmac_listen_interval_setf(listen_interval);
		}
		else {
		}
	}

	bk_ps_info.sleep_count ++;
}

UINT32 power_save_get_rf_ps_dtim_time ( void )
{
	UINT32 tm;
	tm = bk_ps_info.ps_dtim_period * bk_ps_info.ps_dtim_multi * bk_ps_info.ps_beacon_int;
	return tm;
}

void power_save_sleep_status_set ( void )
{
	bk_ps_info.ps_real_sleep = 1;
	bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_NONE;
}

UINT8 power_save_set_all_vif_prevent_sleep ( UINT32 prevent_bit )
{
	VIF_INF_PTR vif_entry = NULL;
	UINT32 i;

	for ( i = 0; i < NX_VIRT_DEV_MAX; i++ ) {
		vif_entry = &vif_info_tab[i];

		if ( vif_entry->active && vif_entry->type == VIF_STA ) {
			vif_entry->prevent_sleep |= prevent_bit;
			#if (NX_HW_PARSER_TIM_ELEMENT)
			if(vif_entry->u.sta.beacon_loss_cnt < 10)
			{
				nxmac_ack_tim_set_clearf(1);
				nxmac_gen_int_enable_set(nxmac_gen_int_enable_get() | NXMAC_TIM_SET_BIT);
			}
			#endif
		}
	}

	return 0;
}

/*This function will run in mac wakeup fiq,
only an actual emergency can put here,
can't operate wifi tx,rx,modem,rf here*/
void power_save_wkup_wait_idle_int_cb ( void )
{
#if (CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7238)
    #if (1 == CFG_LOW_VOLTAGE_PS)
        #if (CFG_SOC_NAME == SOC_BK7231N)
        sctrl_fix_dpll_div();
        #endif
        phy_wakeup_rf_reinit();
        phy_wakeup_wifi_reinit();
    #endif
#endif

}

UINT8 power_save_clr_all_vif_prevent_sleep ( UINT32 prevent_bit )
{
	VIF_INF_PTR vif_entry = NULL;
	UINT32 i;

	for ( i = 0; i < NX_VIRT_DEV_MAX; i++ ) {
		vif_entry = &vif_info_tab[i];

		if ( vif_entry->active && vif_entry->type == VIF_STA ) {
			vif_entry->prevent_sleep &= ~ ( prevent_bit );
		}
	}

	return 0;
}
#if CFG_SUPPORT_BLE
extern void ps_recover_ble_switch_mac_status(void);
#endif
void power_save_wakeup ( void )
{
	UINT32 reg;
	PS_DEBUG_UP_TRIGER;
	bk_ps_info.waited_beacon = STA_GET_FALSE;

#if CFG_USE_STA_PS
	sctrl_sta_rf_wakeup();
#if CFG_SUPPORT_BLE
	rf_wifi_used_set();
#endif
	reg = REG_READ ( ICU_ARM_WAKEUP_EN );
	reg &= ~ ( MAC_ARM_WAKEUP_EN_BIT );
	REG_WRITE ( ICU_ARM_WAKEUP_EN, reg );
#endif
#if NX_POWERSAVE
	rwnxl_wakeup ( power_save_wkup_wait_idle_int_cb );
#endif

	if ( bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_RW ) {
		power_save_set_all_vif_prevent_sleep ( ( UINT32 ) ( PS_VIF_WAITING_BCN ) );
	}

	reg = REG_READ ( ICU_INTERRUPT_ENABLE );
	reg |= ( CO_BIT ( FIQ_MAC_TX_RX_MISC )
	         | CO_BIT ( FIQ_MAC_TX_RX_TIMER )
	         | CO_BIT ( FIQ_MAC_RX_TRIGGER )
	         | CO_BIT ( FIQ_MAC_TX_TRIGGER )
	         | CO_BIT ( FIQ_MAC_GENERAL )
	         | CO_BIT ( FIQ_MAC_PROT_TRIGGER ) );
#if (0 == CFG_LOW_VOLTAGE_PS)
	reg &= ~ ( CO_BIT ( FIQ_MAC_WAKEUP ) );
#endif
#if (( 1 == CFG_LOW_VOLTAGE_PS)&&(1 == CFG_LOW_VOLTAGE_PS_32K_DIV))
	rc_cntl_stat_set(0x09);
#endif
	REG_WRITE ( ICU_INTERRUPT_ENABLE, reg );
	PS_DEBUG_UP_TRIGER;
	ASSERT ( !ps_lock );
	ps_lock ++;
}

void power_save_dtim_exit_check()
{
	if ( power_save_wkup_event_get() & NEED_DISABLE_BIT ) {
		power_save_dtim_rf_ps_disable_send_msg();
		power_save_wkup_event_clear ( NEED_DISABLE_BIT );
	}
}

void power_save_ieee_dtim_wakeup ( void )
{
	if ( ( bk_ps_info.ps_arm_wakeup_way >  PS_ARM_WAKEUP_NONE
	       && bk_ps_info.ps_arm_wakeup_way <= PS_ARM_WAKEUP_USER )
	     && bk_ps_info.ps_real_sleep ) {
		PS_DEBUG_UP_TRIGER;
		power_save_wakeup();

		if ( !bk_ps_info.ps_real_sleep )
			os_printf ( "ps r s not 0\r\n" );

		bk_ps_info.ps_real_sleep = 0;
		bk_ps_info.ps_can_sleep = 1;
#if CFG_USE_MCU_PS
		//tick check
		mcu_ps_machw_cal();
#endif
		last_wk_tick = fclk_get_tick();
#if PS_USE_KEEP_TIMER

		if ( !power_save_if_sleep_first()
#if (0 == CFG_LOW_VOLTAGE_PS)
		&& ps_keep_timer_period
#endif		
		) {
			ps_keep_timer_flag = 1;
			bmsg_ps_sender ( PS_BMSG_IOCTL_RF_KP_SET );
			PS_DEBUG_PWM_TRIGER;
		}
		else {
			//os_printf("errr %d %d\r\n", power_save_if_sleep_first(), ps_keep_timer_period);
		}

#endif

#if CFG_SUPPORT_BLE
		if ( !ble_switch_mac_sleeped )
#endif
			power_save_rf_ps_wkup_semlist_set();

		ke_evt_set ( KE_EVT_KE_TIMER_BIT );
		ke_evt_set ( KE_EVT_MM_TIMER_BIT );
		power_save_dtim_exit_check();
	}

#if CFG_SUPPORT_BLE
	if(!power_save_if_rf_sleep()){
		ps_recover_ble_switch_mac_status();
	}
#endif
}
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
void ps_info_dump(void)
{
    uint32_t h,m,s;
    ps_info.ps_print_begin_time = cal_get_time_us() ;
    us_to_readable_value(ps_info.ps_start_time,&h,&m,&s);
    os_printf("ps_start_time :h=%d,m=%d,s=%d\r\n",h,m,s);

    ps_info.running_time_in_total = ps_info.ps_print_begin_time - ps_info.ps_start_time;
    us_to_readable_value(ps_info.running_time_in_total,&h,&m,&s);
    os_printf("running_time_in_total :h=%d,m=%d,s=%d\r\n",h,m,s);
    os_printf("\r\n");
    os_printf("wakeup_time_in_total =%d ms\r\n",(int)(ps_info.wakeup_time_in_total/1000));//ms
    os_printf("wakeup_time_in_total_without_receive_data =%d ms\r\n",(int)(ps_info.wakeup_time_in_total_without_receive_data/1000));//ms
    os_printf("wakeup_to_beacon_time_in_total =%d ms\r\n",(int)(ps_info.wakeup_to_beacon_time_in_total/1000));//ms
    os_printf("\r\n");
    os_printf("sleep_count_in_total =%d\r\n",ps_info.sleep_count_in_total);
    os_printf("sleep_count_in_total_without_receive_data =%d\r\n",ps_info.sleep_count_in_total_without_receive_data);
    os_printf("\r\n");
    os_printf("average wakeup time =%6.2f ms\r\n",(float)(ps_info.wakeup_time_in_total/1000/ps_info.sleep_count_in_total));//ms
    os_printf("average wakeup time_without_receive_data =%6.2f ms\r\n",(float)(ps_info.wakeup_time_in_total_without_receive_data/1000/ps_info.sleep_count_in_total_without_receive_data));//ms
    os_printf("average beacon time =%6.2f ms\r\n",(float)ps_info.wakeup_to_beacon_time_in_total/1000/ps_info.sleep_count_in_total);//ms
    os_printf("\r\n");
    os_printf("connection_loss_count =%d\r\n",ps_info.connection_loss_count);
    os_printf("beacon_missing_count_in_total =%d\r\n",ps_info.beacon_missing_count_in_total);
    os_printf("beacon missing rate =%6.2f%% \r\n",(float)ps_info.beacon_missing_count_in_total/ps_info.sleep_count_in_total*100);
    for(int i =0;i<10;i++)
    {
        os_printf("beacon_missing_count_in_statistical_period[%d] =%d\r\n",i,ps_info.beacon_missing_count_in_statistical_period[i]);
    }

}


void power_save_statistical_timer_real_handler(void)
{
//	UINT32 err;
    static uint32_t record_idx = 0;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
    ps_info.beacon_missing_count_in_statistical_period[record_idx++] = ps_info.beacon_missing_count_in_current_statistical_period;
    if(record_idx == 10)
        record_idx = 0;
    ps_info.beacon_missing_count_in_current_statistical_period = 0;
    ps_info.sleep_count_in_current_statistical_period = 0;
#endif
//    err = rtos_reload_timer(&ps_statistical_timer);
//    ASSERT(kNoErr == err);
    GLOBAL_INT_RESTORE();
}


bool power_save_rf_sleep_check( void )
{
	uint32_t ret = 0;
	uint32_t debug_print_flag = 0;
	
#if (NX_POWERSAVE && CFG_USE_STA_PS && PS_WAKEUP_MOTHOD_RW)

	if(PS_STA_DTIM_CAN_SLEEP)
	{
		GLOBAL_INT_DECLARATION();
		if((ke_evt_get() != 0)
			|| /*(!bmsg_is_empty())
			|| */(0 == lv_ps_is_got_anchor_point())) 
		{
			ret = false;
			debug_print_flag = 1;
			goto check_exit;
		 }

		if(!ps_may_sleep())
		{
			ret = false;
			debug_print_flag = 11;
			goto check_exit;
		}

		GLOBAL_INT_DISABLE();
		ps_sleep_check();


		if(power_save_if_rf_sleep())
		{
			mcu_power_save(50);
		}

		GLOBAL_INT_RESTORE();
	}
	else
	{
		debug_print_flag = 2;
	}
#endif //(NX_POWERSAVE)

check_exit:
	if(debug_print_flag && lv_ps_is_super_anchor_point())
	{
//		os_printf(":%d\r\n", debug_print_flag);
/*		os_printf("debug_print_flag:%d\r\n", debug_print_flag);
		os_printf("debug_print0:%d\r\n", power_save_if_ps_rf_dtim_enabled());
		os_printf("debug_print1:%d\r\n", (ke_evt_get() != 0));
		os_printf("debug_print2:%d\r\n", (!bmsg_is_empty()));
		os_printf("debug_print3:%d\r\n", (power_save_beacon_state_get() == STA_GET_TRUE));
		os_printf("debug_print4:%d\r\n", (power_save_wkup_way_get() == PS_ARM_WAKEUP_USER));
		os_printf("debug_print5:%d\r\n", power_save_if_ps_can_sleep());//bk_ps_info.ps_can_sleep == 1
		os_printf("\r\n");*/
	}
#if ((1 == CFG_LOW_VOLTAGE_PS)&& ( 1 == CFG_LOW_VOLTAGE_PS_TEST ))
	if((ps_info.ps_print_enable == 1)&&(lv_ps_wakeup_mac_timepoint !=0)&&(cal_get_time_us() -ps_info.ps_last_statistical_time >= ps_info.ps_statistical_period *1000000))
	{
		power_save_statistical_timer_real_handler();
		ps_info.ps_last_statistical_time = cal_get_time_us();
	}
	if((ps_info.ps_print_enable == 1)&&(lv_ps_wakeup_mac_timepoint !=0)&&(cal_get_time_us() -ps_info.ps_last_print_time >= ps_info.ps_print_period *1000000))
	{
		ps_info_dump();
		ps_info.ps_last_print_time = cal_get_time_us();
		ps_info.wakeup_time_in_total -= (ps_info.ps_last_print_time -ps_info.ps_print_begin_time);
	}
#endif
	return ret;
}

#else
bool power_save_rf_sleep_check ( void )
{
#if (NX_POWERSAVE)
#if CFG_USE_STA_PS
#if PS_WAKEUP_MOTHOD_RW

	if ( PS_STA_DTIM_CAN_SLEEP ) {
		GLOBAL_INT_DECLARATION();

		if ( ke_evt_get() != 0 ) {
			return false;
		}

		if ( !bmsg_is_empty() ) {
			return false;
		}

		GLOBAL_INT_DISABLE();
		ps_sleep_check();
		GLOBAL_INT_RESTORE();
	}

#endif
#endif
#endif //(NX_POWERSAVE)
	return 0;
}
#endif
void power_save_me_ps_first_set_state ( UINT8 state )
{
	int param_len;
	VIF_INF_PTR vif_entry;
	struct ke_msg *kmsg_dst;
	struct me_set_ps_disable_req *me_ps_ptr;
	os_printf ( "%s:%d \r\n", __FUNCTION__, __LINE__ );
	param_len = sizeof ( struct me_set_ps_disable_req );
	vif_entry = ( VIF_INF_PTR ) rwm_mgmt_is_vif_first_used();

	while ( vif_entry ) {
		if ( vif_entry->type == VIF_STA && vif_entry->active ) {
			kmsg_dst = ( struct ke_msg * ) os_malloc ( sizeof ( struct ke_msg )
			           + param_len );

			if ( 0 == kmsg_dst ) {
				os_printf ( "%s:%d malloc fail\r\n", __FUNCTION__, __LINE__ );
				return ;
			}

			os_memset ( kmsg_dst, 0, ( sizeof ( struct ke_msg ) + param_len ) );
			kmsg_dst->id = ME_PS_REQ;
			kmsg_dst->dest_id = TASK_ME;
			kmsg_dst->src_id  = TASK_NONE;
			kmsg_dst->param_len = param_len;
			me_ps_ptr = ( struct me_set_ps_disable_req * ) kmsg_dst->param;
			me_ps_ptr->ps_disable = state;
			me_ps_ptr->vif_idx = vif_entry->index;
			ke_msg_send ( ke_msg2param ( kmsg_dst ) );
		}

		vif_entry = ( VIF_INF_PTR ) rwm_mgmt_next ( vif_entry );
	}
}


void power_save_me_ps_set_state ( UINT8 state , UINT8 vif_idx )
{
	os_printf ( "%s:%d \r\n", __FUNCTION__, __LINE__ );
	{
		struct me_set_ps_disable_req *me_ps_ptr = KE_MSG_ALLOC ( ME_SET_PS_DISABLE_REQ, TASK_ME, TASK_NONE,
		        me_set_ps_disable_req );
		me_ps_ptr->ps_disable = state;
		me_ps_ptr->vif_idx = vif_idx;
		ke_msg_send ( me_ps_ptr );
	}
}

void power_save_sm_set_bcmc ( UINT8 bcmc , UINT8 vif_idx )
{
	struct mm_set_ps_options_req *req;
	// Get a pointer to the kernel message
	req = KE_MSG_ALLOC ( MM_SET_PS_OPTIONS_REQ, TASK_MM, TASK_NONE, mm_set_ps_options_req );
	// Fill the message parameters
	req->dont_listen_bc_mc = bcmc;
	req->listen_interval = 0;
	req->vif_index = vif_idx;
	os_printf ( "%s %d %d %d\r\n", __FUNCTION__, req->dont_listen_bc_mc,
	            req->listen_interval, req->vif_index );
	// Set the PS options for this VIF
	ke_msg_send ( req );
}

UINT8 power_save_sm_set_all_bcmc ( UINT8 bcmc )
{
	VIF_INF_PTR vif_entry = NULL;
	UINT32 i;

	for ( i = 0; i < NX_VIRT_DEV_MAX; i++ ) {
		vif_entry = &vif_info_tab[i];

		if ( vif_entry->active && vif_entry->type != VIF_STA ) {
			os_printf ( "%s:%d %d is %d not STA!!!!\r\n", __FUNCTION__, __LINE__, i, vif_entry->type );
			return 0;
		}
	}

	for ( i = 0; i < NX_VIRT_DEV_MAX; i++ ) {
		vif_entry = &vif_info_tab[i];

		if ( vif_entry->active && vif_entry->type == VIF_STA ) {
			power_save_sm_set_bcmc ( bcmc, i );
		}
	}

	return 0;
}


UINT8 power_save_me_ps_set_all_state ( UINT8 state )
{
	VIF_INF_PTR vif_entry = NULL;
	UINT32 i;

	if ( state == false ) {
		for ( i = 0; i < NX_VIRT_DEV_MAX; i++ ) {
			vif_entry = &vif_info_tab[i];

			if ( vif_entry->active && vif_entry->type != VIF_STA ) {
				os_printf ( "%s:%d %d is %d not STA!!!!\r\n", __FUNCTION__, __LINE__, i, vif_entry->type );
				return 0;
			}
		}
	}

	for ( i = 0; i < NX_VIRT_DEV_MAX; i++ ) {
		vif_entry = &vif_info_tab[i];

		if ( vif_entry->active && vif_entry->type == VIF_STA ) {
			power_save_me_ps_set_state ( state, i );
		}
	}

	return 0;
}
#if PS_USE_KEEP_TIMER
#if ( 1 == CFG_LOW_VOLTAGE_PS)
void power_save_keep_timer_init ( void )
{
	UINT32 err;

//	os_printf ( "ps_keep_timer init %d\r\n", ps_keep_timer_period );
	if ( rtos_is_oneshot_timer_init ( &ps_keep_timer ) )
	{
		power_save_keep_timer_stop();
		power_save_keep_timer_set();
		err = rtos_change_period_1( &ps_keep_timer ,ps_keep_timer_period);
//		ASSERT ( kNoErr == err );
	}
	else
	{

		if ( ps_keep_timer_period > 0 ) {
		err = rtos_init_oneshot_timer ( &ps_keep_timer,
								ps_keep_timer_period,
								( timer_2handler_t ) power_save_keep_timer_handler,
								NULL,
								NULL );
		ASSERT ( kNoErr == err );
		}
	}
}
#else

void power_save_keep_timer_init ( void )
{
	UINT32 err;

	if ( rtos_is_oneshot_timer_init ( &ps_keep_timer ) )
	{
		power_save_keep_timer_stop();
		err = rtos_deinit_oneshot_timer ( &ps_keep_timer );
		ASSERT ( kNoErr == err );
	}

	os_printf ( "ps_keep_timer init %d\r\n", ps_keep_timer_period );

	if ( ps_keep_timer_period > 0 ) {
		err = rtos_init_oneshot_timer ( &ps_keep_timer,
		                        ps_keep_timer_period,
		                        ( timer_2handler_t ) power_save_keep_timer_handler,
		                        NULL,
		                        NULL );
		ASSERT ( kNoErr == err );
	}
}
#endif

#endif

void power_save_dtim_ps_init ( void )
{
	bk_ps_info.sleep_count = 0;
	bk_ps_info.sleep_first = 1;
	os_printf ( "power_save_dtim_ps_init\r\n" );
	bk_ps_info.ps_can_sleep = 1;
}


void power_save_dtim_ps_exit ( void )
{
#if PS_USE_KEEP_TIMER
	power_save_keep_timer_stop();
#endif
#if PS_USE_WAIT_TIMER
	power_save_wait_timer_stop();
#endif
	nxmac_beacon_int_setf ( 0 );
	delay ( 1 );
	bk_ps_info.sleep_count = 0;
	bk_ps_info.ps_dtim_period = 1;
	bk_ps_info.ps_dtim_multi = 1;
	bk_ps_info.waited_beacon = STA_GET_INIT;
	bk_ps_info.sleep_first = 1;
	bk_ps_info.ps_can_sleep = 0;
	bk_ps_info.ps_real_sleep = 0;
}


int power_save_dtim_enable_handler ( void )
{
	UINT32 ps_time, multi;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( ( mhdr_get_station_status() >=  RW_EVT_STA_CONNECTED ) ) {
		ps_time = power_save_get_rf_ps_dtim_time();

		if ( ps_time > 0 && ps_time < 75 ) {
			multi = 75 / ps_time + 1;
			power_save_set_dtim_multi ( multi );
		}
		else {
			power_save_set_dtim_multi ( 1 );
		}

		os_printf ( "enter %d ps,p:%d m:%d int:%d l:%d!\r\n", bk_ps_info.listen_mode,
		            bk_ps_info.ps_dtim_period, bk_ps_info.ps_dtim_multi,
		            bk_ps_info.ps_beacon_int, bk_ps_info.listen_int );
		power_save_dtim_ps_init();
		bk_ps_mode = PS_DTIM_PS_MODE;
#if PS_USE_WAIT_TIMER
		power_save_wait_timer_init();
#endif
	}
	else {
		os_printf ( "%s:%d %d %d--\r\n", __FUNCTION__, __LINE__, bk_ps_mode, mhdr_get_station_status() );
	}

	GLOBAL_INT_RESTORE();
	return 0;
}


int power_save_dtim_enable ( void )
{
	if ( ! net_if_is_up() ) {
		os_printf ( "net %d not ip up\r\n", mhdr_get_station_status() );
		return -1;
	}

	if ( g_wlan_general_param->role != CONFIG_ROLE_STA ) {
		os_printf ( "can't dtim,role %d not only sta!\r\n", g_wlan_general_param->role );
		return -1;
	}

	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( bk_ps_mode != PS_NO_PS_MODE ) {
		os_printf ( "can't dtim ps,ps in mode %d!\r\n", bk_ps_mode );
		GLOBAL_INT_RESTORE();
		return -1;
	}

	{
		os_printf ( "first enable sleep \r\n" );
		power_save_me_ps_first_set_state ( PS_MODE_ON_DYN );
	}

	GLOBAL_INT_RESTORE();
	return 0;
}

int power_save_dtim_disable_handler ( void )
{
	UINT32 wdt_val = 1;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	bk_ps_mode = PS_NO_PS_MODE;

	if ( bk_ps_info.ps_real_sleep == 1 ) {
		os_printf ( "%s:%d err----\r\n", __FUNCTION__, __LINE__ );
	}

	rwnxl_set_nxmac_timer_value();
	power_save_dtim_ps_exit();

#if CFG_SUPPORT_BLE
	rf_wifi_used_clr();
#endif

	if ( power_save_wkup_event_get() & NEED_REBOOT_BIT ) {
		sddev_control ( WDT_DEV_NAME, WCMD_POWER_DOWN, NULL );
		os_printf ( "pswdt reboot\r\n" );
        bk_misc_update_set_type(RESET_SOURCE_REBOOT);
		sddev_control ( WDT_DEV_NAME, WCMD_SET_PERIOD, &wdt_val );
		sddev_control ( WDT_DEV_NAME, WCMD_POWER_UP, NULL );

		while ( 1 );
	}

	GLOBAL_INT_RESTORE();
	os_printf ( "exit dtim ps!\r\n" );
#if CFG_SUPPORT_BLE
	ps_recover_ble_switch_mac_status();
#endif
	return 0;
}

int power_save_dtim_disable ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	if ( bk_ps_mode == PS_DTIM_PS_MODE ) {
		GLOBAL_INT_RESTORE();
		power_save_me_ps_set_all_state ( true );
		os_printf ( "start exit!\r\n" );
		return 0;
	}
	else {
		GLOBAL_INT_RESTORE();
	}

	return 0;
}


int power_save_dtim_rf_ps_disable_send_msg ( void )
{
	if ( bk_ps_mode == PS_DTIM_PS_MODE ) {
		bmsg_ps_sender ( PS_BMSG_IOCTL_RF_DISANABLE );
	}

	return 0;
}

void power_save_rf_dtim_manual_do_wakeup ( void )
{
	UINT32 reg;
#if CFG_USE_AP_IDLE
	
	if ( bk_wlan_has_role ( VIF_AP ) && ap_ps_enable_get() ) {
		GLOBAL_INT_DECLARATION();
		GLOBAL_INT_DISABLE();
		power_save_rf_hold_bit_set(RF_HOLD_BY_AP_BIT);
		wifi_general_mac_state_set_active();
		GLOBAL_INT_RESTORE();
	}
	
#endif
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

#if CFG_SUPPORT_BLE
	if ( ble_switch_mac_sleeped ) {
		GLOBAL_INT_RESTORE();
		return;
	}
#endif
#if (1 == CFG_LOW_VOLTAGE_PS)
	return;
#endif
	rtos_lock_scheduling();
	PS_DEBUG_UP_TRIGER;

	if ( ( bk_ps_mode == PS_DTIM_PS_MODE )
	     && ( bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_NONE
	          || bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_UPING )
	     && ( bk_ps_info.ps_real_sleep == 1 ) ) {
		delay ( 1 );
		PS_DEBUG_UP_TRIGER;

		if ( bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_UPING ) {
			bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_RW;
		}
		else {
			bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_USER;
		}

		reg = REG_READ ( ICU_INTERRUPT_ENABLE );
		reg &= ~ ( CO_BIT ( FIQ_MAC_WAKEUP ) );
		REG_WRITE ( ICU_INTERRUPT_ENABLE, reg );
		power_save_ieee_dtim_wakeup();
		PS_PRT ( "m_r_u\r\n" );
	}

	rtos_unlock_scheduling();
	GLOBAL_INT_RESTORE();
}


#if PS_USE_KEEP_TIMER
void power_save_set_keep_timer_time ( UINT32 time )
{
	if ( time >= 0 && time < 100 ) {
		GLOBAL_INT_DECLARATION();
		GLOBAL_INT_DISABLE();
		ps_keep_timer_period = time ;
		power_save_keep_timer_init();
		GLOBAL_INT_RESTORE();
	}

	return;
}
#endif

void power_save_set_dtim_period ( UINT8 period )
{
	if ( bk_ps_info.ps_dtim_period != period ) {
		os_printf ( "new dtim period:%d\r\n", period );
	}

	bk_ps_info.ps_dtim_period = period;
}

void power_save_set_dtim_count ( UINT8 count )
{
	bk_ps_info.ps_dtim_count = count;
}

void power_save_cal_bcn_listen_int ( UINT16 bcn_int )
{
	if ( bcn_int != 0 ) {
		bk_ps_info.ps_beacon_int = bcn_int;
		//bk_ps_info.listen_int = PS_DTIM_COUNT;
		lv_ps_set_bcn_int(bcn_int << 10);
	}
}

void power_save_set_listen_int(UINT16 listen_int)
{
	if((listen_int > 100) || (listen_int == 0))
		bk_ps_info.listen_int = PS_DTIM_COUNT;
	else
		bk_ps_info.listen_int = listen_int;

	os_printf("set listen intval:%d\r\n", bk_ps_info.listen_int, listen_int);
}
UINT8 power_save_get_listen_int ( void )
{
	return bk_ps_info.listen_int;
}

void power_save_delay_sleep_check ( void )
{
	bmsg_ps_sender ( PS_BMSG_IOCTL_RF_TD_SET );
}

#if PS_USE_WAIT_TIMER
void power_save_wait_timer_stop ( void )
{
	OSStatus err;
	if ( rtos_is_oneshot_timer_running ( &ps_wait_timer ) )
	{
		err = rtos_stop_oneshot_timer ( &ps_wait_timer );
		ASSERT ( kNoErr == err );
	}
	ps_wait_timer_status = 0;
}

void power_save_wait_timer_real_handler ( void )
{
	power_save_wait_timer_stop();
#if (0 == CFG_LOW_VOLTAGE_PS)
	if ( PS_STA_DTIM_SWITCH ) {
		power_save_beacon_state_set ( STA_GET_TRUE );
	}
#else
	power_save_clr_all_vif_prevent_sleep((UINT32)(PS_VIF_WAITING_BCMC));
#endif
}

void power_save_wait_timer_handler ( void *data )
{
	bmsg_ps_sender ( PS_BMSG_IOCTL_WAIT_TM_HANDLER );
}

void power_save_wait_timer_init ( void )
{
	UINT32 err;
	if ( rtos_is_oneshot_timer_init ( &ps_wait_timer ) )
	{
		power_save_wait_timer_stop();
		err = rtos_deinit_oneshot_timer ( &ps_wait_timer );
		ASSERT ( kNoErr == err );
	}

	{
		err = rtos_init_oneshot_timer ( &ps_wait_timer,
		                        20,
		                        ( timer_2handler_t ) power_save_wait_timer_handler,
		                        NULL,
		                        NULL );
		ASSERT ( kNoErr == err );
	}
}

void power_save_wait_timer_set ( void )
{
	if ( PS_STA_DTIM_SWITCH ) {
		bmsg_ps_sender ( PS_BMSG_IOCTL_WAIT_TM_SET );
	}
}

void power_save_wait_timer_start ( void )
{
	OSStatus err;
	if ( rtos_is_oneshot_timer_init ( &ps_wait_timer ) && ps_wait_timer_status == 0 )
	{
		ps_wait_timer_status = 1;
#if (0 == CFG_LOW_VOLTAGE_PS)
		power_save_beacon_state_set ( STA_GET_FALSE );
#endif
		err = rtos_start_oneshot_timer ( &ps_wait_timer );
		ASSERT ( kNoErr == err );
	}
}
#else
void power_save_wait_set ( UINT32 set )
{
	ps_wait_timer_status = set;
}

UINT32 power_save_wait_get ( void )
{
	return ps_wait_timer_status;
}
#endif

#if PS_USE_KEEP_TIMER
void power_save_keep_timer_stop ( void )
{
	OSStatus err;
	GLOBAL_INT_DECLARATION();
	if ( rtos_is_oneshot_timer_running ( &ps_keep_timer ) )
	{
		err = rtos_stop_oneshot_timer ( &ps_keep_timer );
		ASSERT ( kNoErr == err );
	}

	GLOBAL_INT_DISABLE();
	ps_keep_timer_status = 0;
	GLOBAL_INT_RESTORE();
}

void power_save_keep_timer_real_handler(void)
{
	GLOBAL_INT_DECLARATION();
	power_save_keep_timer_stop();
	PS_DEBUG_PWM_TRIGER;
#if CFG_SUPPORT_BLE
	rf_wifi_used_clr();
#endif
	GLOBAL_INT_DISABLE();

	if ( ( PS_STA_DTIM_SWITCH )
		&& bk_ps_info.ps_arm_wakeup_way == PS_ARM_WAKEUP_RW
		&& 0 == bk_ps_info.ps_real_sleep ) {
		do{
			#if (1 == CFG_LOW_VOLTAGE_PS)
			bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_USER;
			power_save_clr_all_vif_prevent_sleep((UINT32)(PS_VIF_WAITING_BCN));
			ps_bcn_loss_max_count ++;

			lv_ps_beacon_missing_handler();
			break;
			#endif
	
			if(ps_keep_timer_flag && (power_save_beacon_state_get() != STA_GET_TRUE))
			{
				PS_DBG("@%d ",__LINE__);
				ps_fake_data_rx_check();
				ps_keep_timer_flag = 0;
				bmsg_ps_sender(PS_BMSG_IOCTL_RF_KP_SET);
				GLOBAL_INT_RESTORE();
				return;
			}

			if(0 == ps_reseted_moniter_flag
			&& ps_bcn_loss_max_count < PS_BCN_MAX_LOSS_LIMIT)
			{
				#if ( 1 == CFG_LOW_VOLTAGE_PS )
				bk_ps_info.ps_arm_wakeup_way = PS_ARM_WAKEUP_USER;
				#else
				power_save_beacon_state_set ( STA_GET_TRUE );
				#endif
				power_save_clr_all_vif_prevent_sleep((UINT32)(PS_VIF_WAITING_BCN));
				ps_bcn_loss_max_count ++;

				PS_DBG("@%d ",__LINE__);
				ps_run_td_timer(0);
			}
			else {
				//If more than 5 consecutive beacon loss happens, stay wakeup
				ps_reseted_moniter_flag = 0;
			}
		}while(0);
		GLOBAL_INT_RESTORE();
		delay ( 1 );
		PS_DEBUG_PWM_TRIGER;
#if CFG_USE_STA_PS
		extern void bmsg_null_sender ( void );
		bmsg_null_sender();
#endif
	}
	else {
		GLOBAL_INT_RESTORE();
	}
}

void power_save_keep_timer_handler ( void *data )
{
	bmsg_ps_sender ( PS_BMSG_IOCTL_RF_KP_HANDLER );
}

void power_save_keep_timer_set ( void )
{
	OSStatus err;
	if ( rtos_is_oneshot_timer_init ( &ps_keep_timer ) && ps_keep_timer_status == 0 )
	{
		ps_keep_timer_status = 1;
		err = rtos_start_oneshot_timer ( &ps_keep_timer );
		ASSERT ( kNoErr == err );
	}
}
#endif

void power_save_rf_ps_wkup_semlist_init ( void )
{
	co_list_init ( &bk_ps_info.wk_list );
}

void *power_save_rf_ps_wkup_semlist_create ( void )
{
	UINT32 ret;
	PS_DO_WKUP_SEM *sem_list = ( PS_DO_WKUP_SEM * ) os_malloc ( sizeof ( PS_DO_WKUP_SEM ) );

	if ( !sem_list ) {
		os_printf ( "semlist_wait NULL\r\n" );
		return 0;
	}

	ret = rtos_init_semaphore ( &sem_list->wkup_sema, 1 );
	ASSERT ( 0 == ret );
	return sem_list;
}


void power_save_rf_ps_wkup_semlist_wait ( void *sem_list_p )
{
	PS_DO_WKUP_SEM *sem_list = ( PS_DO_WKUP_SEM * ) sem_list_p;
	co_list_push_back ( &bk_ps_info.wk_list, &sem_list->list );

#if CFG_SUPPORT_BLE
	if ( !ble_switch_mac_sleeped )
#endif
		bmsg_ps_sender ( PS_BMSG_IOCTL_RF_USER_WKUP );
}

void power_save_rf_ps_wkup_semlist_destroy ( void *sem_list_p )
{
	UINT32 ret;
	PS_DO_WKUP_SEM *sem_list = ( PS_DO_WKUP_SEM * ) sem_list_p;
	ret = rtos_deinit_semaphore ( &sem_list->wkup_sema );
	ASSERT ( 0 == ret );
}

void power_save_rf_ps_wkup_semlist_get ( void *sem_list )
{
	UINT32 ret;

	if ( sem_list ) {
		ret = rtos_get_semaphore ( & ( ( PS_DO_WKUP_SEM * ) sem_list )->wkup_sema, BEKEN_NEVER_TIMEOUT );
		ASSERT ( 0 == ret );
		GLOBAL_INT_DECLARATION();
		GLOBAL_INT_DISABLE();
		co_list_extract ( &bk_ps_info.wk_list, & ( ( PS_DO_WKUP_SEM * ) sem_list )->list );
		GLOBAL_INT_RESTORE();
		ret = rtos_deinit_semaphore ( & ( ( PS_DO_WKUP_SEM * ) sem_list )->wkup_sema );
		ASSERT ( 0 == ret );
		os_free ( sem_list );
		sem_list = NULL;
	}
}

void power_save_rf_ps_wkup_semlist_set ( void )
{
	UINT32 ret;

	rtos_lock_scheduling();
	while ( !co_list_is_empty ( &bk_ps_info.wk_list ) ) {
		PS_DO_WKUP_SEM *sem_list;
		sem_list = list2sem ( co_list_pop_front ( &bk_ps_info.wk_list ) );
		ret = rtos_set_semaphore ( &sem_list->wkup_sema );
		ASSERT ( 0 == ret );
	}
	rtos_unlock_scheduling();
}

void power_save_beacon_state_set ( PS_STA_BEACON_STATE state )
{
	bk_ps_info.waited_beacon = state;
}

void power_save_beacon_state_update ( void )
{
	PS_DEBUG_RX_TRIGER;
#if CFG_SUPPORT_BLE
	rf_wifi_used_clr();
#endif

	if ( PS_STA_DTIM_SWITCH ) {
		if ( power_save_if_ps_can_sleep()
		     && power_save_beacon_state_get() == STA_GET_INIT ) {
			power_save_beacon_state_set ( STA_GET_FALSE );
		}
	}

	if ( PS_STA_DTIM_SWITCH
	     && ( power_save_beacon_state_get() != STA_GET_TRUE )
	   ) {
		power_save_beacon_state_set ( STA_GET_TRUE );
#if PS_USE_KEEP_TIMER
		ps_bcn_loss_max_count = 0;
#endif
		if (platform_is_in_interrupt_context() != RTOS_SUCCESS) {
			if(1 == ps_keep_timer_status)
			{
				//bmsg_ps_sender(PS_BMSG_IOCTL_RF_KP_STOP);
				power_save_keep_timer_stop();
			}
			if(0 == ps_keep_timer_flag)
			{
				PS_DBG("@%d ",__LINE__);
				ps_run_td_timer(0);
			}
		}
	}
}

void power_save_bcn_callback ( uint8_t *data, int len, wifi_link_info_t *info )
{
	struct bcn_frame *bcn = ( struct bcn_frame * ) data;
	VIF_INF_PTR vif_entry;
	vif_entry = ( VIF_INF_PTR ) rwm_mgmt_is_vif_first_used();

	while ( vif_entry ) {
		if ( vif_entry->type == VIF_STA && vif_entry->active ) {
			break;
		}

		vif_entry = ( VIF_INF_PTR ) rwm_mgmt_next ( vif_entry );
	}

	if ( !vif_entry )
		return;

	if ( bcn->bcnint != bk_ps_info.ps_beacon_int ) {
		os_printf ( "bcn interval changed %x %x\r\n", bcn->bcnint, bk_ps_info.ps_beacon_int );
		mm_send_connection_loss_ind ( vif_entry );
	}
}

UINT8 power_save_if_sleep_first ( void )
{
	return bk_ps_info.sleep_first;
}

PS_STA_BEACON_STATE power_save_beacon_state_get ( void )
{
	return bk_ps_info.waited_beacon;
}

PS_ARM_WAKEUP_WAY power_save_wkup_way_get ( void )
{
	return bk_ps_info.ps_arm_wakeup_way;
}

UINT8 power_save_if_ps_can_sleep ( void )
{
	return ( bk_ps_info.ps_can_sleep == 1 );
}

INT8 power_save_if_sleep_at_first ( void )
{
	return ( bk_ps_info.sleep_count < 6 );
}

UINT32 power_save_get_sleep_count ( void )
{
	return bk_ps_info.sleep_count;
}

void power_save_ps_mode_set ( PS_MODE_STATUS mode )
{
	bk_ps_mode = mode;
}

UINT16 power_save_radio_wkup_get ( void )
{
	return r_wakeup_time;
}

void power_save_radio_wkup_set ( UINT16 time )
{
	r_wakeup_time = time;
}

UINT32 power_save_wkup_event_get ( void )
{
	return ps_dis_flag;
}

void power_save_wkup_event_set ( UINT32 value )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	ps_dis_flag |= value;
	GLOBAL_INT_RESTORE();
}

void power_save_wkup_event_clear ( UINT32 value )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	ps_dis_flag &= ~value;
	GLOBAL_INT_RESTORE();
}

UINT16 power_save_beacon_len_get ( void )
{
	return beacon_len;
}

void power_save_beacon_len_set ( UINT16 len )
{
	beacon_len = len + 4/*fcs*/ /*+25 radiotap*/;
}

#if PS_USE_KEEP_TIMER
void power_save_set_reseted_flag ( void )
{
	ps_reseted_moniter_flag = 1;
}

UINT32 power_save_get_bcn_lost_count ( void )
{
	return ps_bcn_loss_max_count;
}
#endif

UINT8 power_save_set_dtim_multi ( UINT8 multi )
{
	bk_ps_info.ps_dtim_multi = multi;

	if ( bk_ps_info.ps_dtim_multi > 0 && bk_ps_info.ps_dtim_multi < 100 ) {
		os_printf ( "set listen dtim:%d\r\n", bk_ps_info.ps_dtim_multi );
	}
	else {
		os_printf ( "set listen dtim:%d err,use default 1\r\n", bk_ps_info.ps_dtim_multi );
		bk_ps_info.ps_dtim_multi = 1;
	}

	bk_ps_info.listen_mode = PS_LISTEN_MODE_DTIM;
	return 0;
}

UINT16 power_save_forbid_trace ( PS_FORBID_STATUS forbid )
{
	bk_forbid_count ++;

	if ( bk_forbid_code != forbid || ( bk_forbid_count % 100 == 0 ) ) {
		PS_DBG ( "front c:%d\r\n\r\n", bk_forbid_count );
		PS_DBG ( "ps_cd:%d %d\r\n", bk_forbid_code, forbid );
		bk_forbid_count = 0;
	}

	bk_forbid_code = forbid;
	return bk_forbid_count;
}
void power_save_dump ( void )
{
	UINT32 i;
	extern UINT32 txl_cntrl_pck_get ( void );
	os_printf ( "rf:%x\r\n", bk_ps_mode );
	os_printf ( "info dump\r\n" );

	for ( i = 0; i < sizeof ( bk_ps_info ); i++ )
		os_printf ( " %d 0x%x\r\n", i, * ( ( UINT8 * ) ( &bk_ps_info ) + i ) );

	os_printf ( "globel dump\r\n" );
	os_printf ( "%d %d %d %d %d %d\r\n",
	            bk_ps_mode,
	            mhdr_get_station_status(),
	            g_wlan_general_param->role,
	            bk_ps_info.waited_beacon,
	            bk_ps_info.ps_can_sleep,
	            ps_lock );
	os_printf ( "env dump\r\n" );
	os_printf ( "%d %d %d %d\r\n",
	            ps_env.ps_on,
	            me_env.ps_on,
	            beacon_len,
	            txl_cntrl_pck_get() );
#if CFG_USE_MCU_PS
	os_printf ( "mcu dump\r\n" );
	os_printf ( "%d %d\r\n",
	            peri_busy_count_get(),
	            mcu_prevent_get() );
#endif
	os_printf ( "%d %d %d %d %d\r\n",
	            bk_ps_info.ps_dtim_period, bk_ps_info.ps_dtim_count,
	            bk_ps_info.ps_dtim_multi, bk_forbid_code );
#if CFG_USE_STA_PS
	sctrl_ps_dump();
#endif
}

void power_save_wake_mac_rf_if_in_sleep(void)
{
    ps_set_rf_prevent();
    power_save_rf_dtim_manual_do_wakeup();

    power_save_rf_hold_bit_set(RF_HOLD_BY_MAC_USE_BIT);
}

void power_save_wake_mac_rf_end_clr_flag(void)
{
    if(ps_get_sleep_prevent() & PS_WAITING_RF_OPERATION)
    {
        ps_clear_rf_prevent();
    }

    power_save_rf_hold_bit_clear(RF_HOLD_BY_MAC_USE_BIT);
}

void power_save_check_clr_rf_prevent_flag(void)
{
}

void power_save_wake_rf_if_in_sleep(void)
{
}

void power_save_clr_temp_use_rf_flag(void)
{
    if(ps_get_sleep_prevent() & PS_WAITING_RF_OPERATION)
    {
        ps_clear_rf_prevent();
    }

    power_save_rf_hold_bit_clear(RF_HOLD_BY_TEMP_BIT);
}

void power_save_set_temp_use_rf_flag(void)
{
    ps_set_rf_prevent();
    power_save_rf_hold_bit_set(RF_HOLD_BY_TEMP_BIT);
}

void power_save_rf_hold_bit_set(UINT32 rf_hold_bit)
{
    UINT32 reg = rf_hold_bit;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
}

void power_save_rf_hold_bit_clear(UINT32 rf_hold_bit)
{
    UINT32 reg = rf_hold_bit;
    sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
}

UINT8 power_save_if_ps_rf_dtim_enabled ( void )
{
	return ( bk_ps_mode == PS_DTIM_PS_MODE );
}

PS_MODE_STATUS power_save_ps_mode_get ( void )
{
	return bk_ps_mode;
}
UINT8 power_save_if_rf_sleep ( void )
{
#if CFG_USE_STA_PS

	if ( bk_ps_info.ps_real_sleep == 1 ) {
		return 1;
	}

#endif
	return 0;
}

UINT32 power_save_time_to_sleep ( void )
{
	INT32 less;
#if CFG_USE_STA_PS
	UINT32 tm;

	if ( bk_ps_info.ps_dtim_count == 0 ) {
		tm = bk_ps_info.ps_dtim_period * bk_ps_info.ps_dtim_multi * bk_ps_info.ps_beacon_int;
	}
	else {
		tm = ( bk_ps_info.ps_dtim_period * ( bk_ps_info.ps_dtim_multi - 1 ) + bk_ps_info.ps_dtim_count ) * bk_ps_info.ps_beacon_int;
	}

	less = tm - ( ( ( fclk_get_tick() - last_wk_tick ) * FCLK_DURATION_MS ) % tm );
#else
	less = 0;
#endif
	return less;
}

#if CFG_LOW_LATENCY_PS
void power_save_set_low_latency ( UINT8 value )
{
	ps_data_low_latency = value;
}

UINT8 power_save_low_latency_get ( void )
{
	return ps_data_low_latency;
}
#endif
// eof

