#include "manual_ps.h"
#include "manual_ps_pub.h"
#include "gpio_pub.h"
#include "power_save_pub.h"
#include "sys_ctrl_pub.h"
#include "target_util_pub.h"

#include "mcu_ps_pub.h"
#include "ps_debug_pub.h"
#include "icu_pub.h"
#include "sys_ctrl.h"
#include "gpio.h"
#include "fake_clock_pub.h"
#include "icu.h"
#include "power_save.h"

UINT32 use_unconditional_sleep = 0;
UINT32 bk_unconditional_sleep_mode_get ( void )
{
	return use_unconditional_sleep;
}
#if CFG_USE_FAKERTC_PS
UINT32 unconditional_sleep_inited = 0;
void unconditional_ps_init ( void )
{
	UINT32 reg;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	
	if ( 0 == unconditional_sleep_inited ) {
		PS_LOGI("%s\r\n", __FUNCTION__ );
		sctrl_mcu_init();
		unconditional_sleep_inited = 1;
	}
	
	GLOBAL_INT_RESTORE();
}


void unconditional_ps_exit ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	
	if ( 1 == unconditional_sleep_inited ) {
		PS_LOGI("%s\r\n", __FUNCTION__ );
		unconditional_sleep_inited = 0;
		sctrl_mcu_exit();
	}
	
	GLOBAL_INT_RESTORE();
}

void bk_enable_unconditional_sleep ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	use_unconditional_sleep = 1;
	PS_LOGI("%s\r\n", __FUNCTION__ );
	unconditional_ps_init();
	GLOBAL_INT_RESTORE();
}

void bk_disable_unconditional_sleep ( void )
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	use_unconditional_sleep = 0;
	PS_LOGI("%s\r\n", __FUNCTION__ );
	unconditional_ps_exit();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_RF_UP, 0 );
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_MAC_UP, 0 );
	GLOBAL_INT_RESTORE();
}


int bk_unconditional_normal_sleep ( UINT32 sleep_ms, char flag )
{
	//flag:1:only gpio wake,0:and other wakeup
	UINT32  sleep_pwm_t, param, uart_miss_us = 0, miss_ticks = 0;
	UINT32 wkup_type, wastage = 0;
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	{
		do {
			if ( sleep_ms != 0xFFFFFFFF ) {
				if ( ps_timer2_get() <= 2U ) {
					break;
				}
				
				if ( sleep_ms <= 5U ) {
					break;
				}
				
				sleep_ms = sleep_ms - FCLK_DURATION_MS;
				sleep_pwm_t = ( sleep_ms * 32U );
				
				if ( ( int32 ) sleep_pwm_t <= 64U ) {
					break;
				}
				
#if (CFG_SOC_NAME == SOC_BK7231)
				
				if ( sleep_pwm_t > 65535U )
					sleep_pwm_t = 65535;
				else
#endif
					if ( sleep_pwm_t < 64U )
						sleep_pwm_t = 64;
			}
			
			sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_RF_UP, 0 );
			sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_UNCONDITIONAL_MAC_UP, 0 );
			mcu_ps_machw_init();
			mcu_ps_machw_cal();
			//PS_LOGI("s:%d %d %x\r\n",fclk_freertos_get_tick(),sleep_pwm_t,REG_READ(ICU_ARM_WAKEUP_EN));
			{
				if ( 0 == REG_READ ( ICU_GLOBAL_INT_EN ) ) {
					PS_LOGI("ICU_GLOBAL_INT_EN err\r\n" );
				}
				
				if ( ( 0x3 ) != ( REG_READ ( ICU_INTERRUPT_ENABLE ) & ( 0x3 ) ) ) {
					PS_LOGI("ICU_INTERRUPT_ENABLE err %x\r\n", REG_READ ( ICU_INTERRUPT_ENABLE ) );
				}
				
#if 1
				
				if ( fclk_get_tick_id() == BK_PWM_TIMER_ID0 ) {
					ps_pwm_disable();
				}
				
				if ( sleep_ms != 0xFFFFFFFF ) {
					ps_timer3_enable ( sleep_pwm_t );
				}
				else {
					PS_LOGI("set tim 0xFFFFFFFF\r\n" );
					ps_timer3_disable();
					
					if ( flag == 1 ) {
						ps_timer02_disable();
					}
				}
				
#else
				ps_pwm0_suspend_tick ( sleep_pwm_t );
#endif
			}
#if 1
			
			if ( sleep_ms != 0xFFFFFFFF ) {
				if ( flag == 1 ) {
					param = ( 0xfffff );
				}
				else {
					param = ( 0xfffff  & ( ~PWD_TIMER_26M_CLK_BIT )
					          & ( ~PWD_TIMER_32K_CLK_BIT ) & ( ~PWD_UART2_CLK_BIT )
					          & ( ~PWD_UART1_CLK_BIT )
					        );
				}
			}
			else {
				if ( flag == 1 ) {
					param = ( 0xfffff );
				}
				else {
					param = ( 0xfffff & ( ~PWD_UART2_CLK_BIT )
					          & ( ~PWD_UART1_CLK_BIT )
					        );
				}
			}
			
#else
#endif
			
			if ( sctrl_unconditional_normal_sleep ( param ) != 0 ) {
				//PS_LOGI("c\r\n");
				ps_timer3_disable();
				
				if ( fclk_get_tick_id() == BK_PWM_TIMER_ID0 ) {
					ps_pwm_enable();
				}
				
				if ( flag == 1 ) {
					ps_timer02_restore();
				}
				
				GLOBAL_INT_RESTORE();
				return 1;
			}
			
#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
			ps_timer3_measure_prepare();
#endif
			wkup_type = sctrl_unconditional_normal_wakeup();
#if 1
			ps_timer3_disable();
			
			if ( fclk_get_tick_id() == BK_PWM_TIMER_ID0 ) {
				ps_pwm_enable();
			}
			
			if ( flag == 1 ) {
				ps_timer02_restore();
			}
			
			mcu_ps_machw_cal();
#endif
			//PS_LOGI("t:%d is:%x\r\n",fclk_freertos_get_tick(),REG_READ(ICU_INT_STATUS));
		}
		while ( 0 );
	}
	GLOBAL_INT_RESTORE();
	return 0;
}
#endif

#if CFG_USE_DEEP_PS
void bk_enter_deep_sleep_mode ( PS_DEEP_CTRL_PARAM *deep_param )
{
	ASSERT ( deep_param != NULL );

	if ( ( deep_param->wake_up_way & PS_DEEP_WAKEUP_GPIO ) ) {
		if ( deep_param->gpio_index_map ) {
			PS_LOGI("---enter deep sleep :wake up with gpio 0~31 ps: 0x%x 0x%x \r\n",
			            deep_param->gpio_index_map, deep_param->gpio_edge_map );
		}
		
		if ( deep_param->gpio_last_index_map ) {
			PS_LOGI("---enter deep sleep :wake up with gpio32~39 ps: 0x%x 0x%x \r\n",
			            deep_param->gpio_last_index_map, deep_param->gpio_last_edge_map );
		}
	}
	
	if ( ( deep_param->wake_up_way & PS_DEEP_WAKEUP_RTC ) ) {
		PS_LOGI("---enter deep sleep :wake up with " );
		
		if ( deep_param->lpo_32k_src == LPO_SELECT_32K_XTAL ) {
			PS_LOGI(" xtal 32k " );
		}
		else {
			PS_LOGI("  rosc " );
		}
		
		PS_LOGI("ps :%d s\r\n", deep_param->sleep_time );
		
		if ( deep_param->sleep_time > 0x1ffff ) {
			deep_param->sleep_time = 0x1ffff;
		}
		
		deep_param->sleep_time = 32768 * deep_param->sleep_time;
	}
	
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	sddev_control ( SCTRL_DEV_NAME, CMD_SCTRL_RTOS_DEEP_SLEEP, deep_param );
	delay ( 5 );
	GLOBAL_INT_RESTORE();
}
#endif
