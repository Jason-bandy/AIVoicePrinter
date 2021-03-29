#include "include.h"
#include "arm_arch.h"
#include "co_list.h"
#include "audio.h"
#include "audio_pub.h"
#include "mailbox_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "mem_pub.h"

#if CFG_USE_AUDIO
void audio_power_up(void)
{
#if (CFG_SOC_NAME == SOC_BK7271)
	rt_kprintf("%s:%d UNIMPLEMENTED\r\n", __FUNCTION__, __LINE__);
#else
	UINT32 param;
	param = PWD_AUDIO_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
#endif
}

void audio_power_down(void)
{
#if (CFG_SOC_NAME == SOC_BK7271)
	rt_kprintf("%s:%d UNIMPLEMENTED\r\n", __FUNCTION__, __LINE__);
#else
	UINT32 param;
	param = PWD_AUDIO_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
#endif
}

void audio_enable_interrupt(void)
{
#if (CFG_SOC_NAME == SOC_BK7271)
	rt_kprintf("%s:%d UNIMPLEMENTED\r\n", __FUNCTION__, __LINE__);
#else
	UINT32 param;
	param = (IRQ_AUDIO_BIT);
	sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
#endif
}

void audio_disable_interrupt(void)
{
#if (CFG_SOC_NAME == SOC_BK7271)
	rt_kprintf("%s:%d UNIMPLEMENTED\r\n", __FUNCTION__, __LINE__);
#else
	UINT32 param;
	param = (IRQ_AUDIO_BIT);
	sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
#endif
}

#if (CFG_SOC_NAME != SOC_BK7271)
extern void audio_adc_irq_handler(UINT32 arg);
static void audio_isr(void)
{
	UINT32 status = REG_READ(AUD_AD_FIFO_STATUS);

	if (status & (DAC_R_INT_FLAG | DAC_L_INT_FLAG)) {

		REG_WRITE(AUD_AD_FIFO_STATUS,
				  DAC_R_NEAR_FULL | DAC_L_NEAR_FULL
				  | DAC_R_NEAR_EMPTY | DAC_L_NEAR_EMPTY
				  | DAC_R_FIFO_FULL | DAC_L_FIFO_FULL
				  | DAC_R_FIFO_EMPTY | DAC_L_FIFO_EMPTY
				  | DAC_R_INT_FLAG | DAC_L_INT_FLAG);

	}
	if (status & ADC_INT_FLAG) {
#if CFG_USE_AUD_ADC
		audio_adc_irq_handler(status);
#endif

		REG_WRITE(AUD_AD_FIFO_STATUS,
				  ADC_NEAR_FULL | ADC_NEAR_EMPTY | ADC_FIFO_FULL
				  | ADC_FIFO_EMPTY | ADC_INT_FLAG);
	}
	if (status & DTMF_INT_FLAG) {
		//audio_dtmf_isr(status);
		REG_WRITE(AUD_AD_FIFO_STATUS,
				  DTMF_NEAR_FULL | DTMF_NEAR_EMPTY | DTMF_FIFO_FULL
				  | DTMF_FIFO_EMPTY | DTMF_INT_FLAG);
	}
}

void audio_hardware_init(void)
{
	UINT32 val;

	/* register interrupt */
	intc_service_register(IRQ_AUDIO, PRI_IRQ_AUDIO, audio_isr);

	REG_WRITE(AUDIO_CONFIG, 0);

	REG_WRITE(AUD_DTMF_CONFIG_0, 0);
	REG_WRITE(AUD_DTMF_CONFIG_1, 0);
	REG_WRITE(AUD_DTMF_CONFIG_2, 0);

	REG_WRITE(AUD_ADC_CONFIG_0, 0x00e93A22);
	REG_WRITE(AUD_ADC_CONFIG_1, 0x8BBF3A22);
	REG_WRITE(AUD_ADC_CONFIG_2, 0xC9E6751C);
	REG_WRITE(AUD_AGC_CONFIG_0, 0x4A019465);
	REG_WRITE(AUD_AGC_CONFIG_1, 0x02016C01);
	REG_WRITE(AUD_AGC_CONFIG_2, 0x0F020940);

	REG_WRITE(AUD_DAC_CONFIG_0, 0);
	REG_WRITE(AUD_DAC_CONFIG_1, 0);
	REG_WRITE(AUD_DAC_CONFIG_2, 0);

	// it's very import to config dac interrupt thred(not all zero)
	REG_WRITE(AUD_FIFO_CONFIG, 0x210);

	/* reset int status */
	val = REG_READ(AUD_AD_FIFO_STATUS);
	REG_WRITE(AUD_AD_FIFO_STATUS, val);
}
#endif

void audio_init(void)
{
#if (CFG_SOC_NAME == SOC_BK7271)
	uint32_t ret;
	uint32_t param;

	param = AUDIO_DAC_VOL_DIFF_MODE;
	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_VOLUME_PORT, &param);
	if (ret) {
		rt_kprintf("set volume fail.\r\n");
		return;
	}

	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_OPEN_DAC_ANALOG, NULL);
	if (ret) {
		rt_kprintf("open dac analog fail.\r\n");
		return;
	}

	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_OPEN_ADC_MIC_ANALOG, NULL);
	if (ret) {
		rt_kprintf("open adc mic analog fail.\r\n");
		return;
	}

	param = 16000;
	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_AUDIO_PLL, &param);
	if (ret) {
		rt_kprintf("set audio PLL fail.\r\n");
		return;
	}
#else
	audio_hardware_init();
#endif
}

void audio_exit(void)
{
#if (CFG_SOC_NAME == SOC_BK7271)
	uint32_t ret;

	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_CLOSE_DAC_ANALOG, NULL);
	if (ret) {
		rt_kprintf("close dac analog fail.\r\n");
		return;
	}

	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_CLOSE_ADC_MIC_ANALOG, NULL);
	if (ret) {
		rt_kprintf("open adc mic analog fail.\r\n");
		return;
	}
#else
	UINT32 val;

	REG_WRITE(AUDIO_CONFIG, 0);

	REG_WRITE(AUD_DTMF_CONFIG_0, 0);
	REG_WRITE(AUD_DTMF_CONFIG_1, 0);
	REG_WRITE(AUD_DTMF_CONFIG_2, 0);

	REG_WRITE(AUD_ADC_CONFIG_0, 0);
	REG_WRITE(AUD_ADC_CONFIG_1, 0);
	REG_WRITE(AUD_ADC_CONFIG_2, 0);

	REG_WRITE(AUD_DAC_CONFIG_0, 0);
	REG_WRITE(AUD_DAC_CONFIG_1, 0);
	REG_WRITE(AUD_DAC_CONFIG_2, 0);

	REG_WRITE(AUD_FIFO_CONFIG, 0);

	/* reset int status */
	val = REG_READ(AUD_AD_FIFO_STATUS);
	REG_WRITE(AUD_AD_FIFO_STATUS, val);

#if CFG_USE_AUD_DAC
	ddev_unregister_dev(AUD_DAC_DEV_NAME);
#endif
#endif
}
#endif // CFG_USE_AUDIO


