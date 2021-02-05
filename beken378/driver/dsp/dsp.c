#include "include.h"
#include "arm_arch.h"
#include "arm_mcu_pub.h"
#include "lbus_pub.h"
#include "dsp_pub.h"
#include "dsp.h"
#include "rtos_pub.h"
#include "pmu.h"

#if (CFG_SOC_NAME == SOC_BK7271)
int g_dsp_inited = 0;

#if CFG_USE_DSP
void dsp_start(void)
{
	uint32 i;
	uint32 src_code_offset = 0;
	uint32 src_code_len = 0;
	uint32 src_data_offset = 0;
	uint32 src_data_len = 0;
	uint32 src_ext_data_offset = 0;
	uint32 src_ext_data_len = 0;
	uint32 dst_code_base = W_DSP_DMEM_64KB_BASE_ADDR;
	uint32 dst_code_offset = 0x0;
	uint32 dst_data_base = W_DSP_DMEM_64KB_BASE_ADDR;
	uint32 dst_data_offset = 0x0;
	uint32 dst_ext_data_base = W_DSP_DMEM_64KB_BASE_ADDR;
	uint32 dst_ext_data_offset = 0;

	src_code_offset = REG_READ(CFG_DSP_SRC_ADD);
	src_code_len  = REG_READ(CFG_DSP_SRC_ADD + 4);
	dst_code_offset = REG_READ(CFG_DSP_SRC_ADD + 8);

	src_data_offset = REG_READ(CFG_DSP_SRC_ADD + 16);
	src_data_len    = REG_READ(CFG_DSP_SRC_ADD + 20);
	dst_data_offset = REG_READ(CFG_DSP_SRC_ADD + 24);

	src_ext_data_offset = REG_READ(CFG_DSP_SRC_ADD + 32);
	src_ext_data_len    = REG_READ(CFG_DSP_SRC_ADD + 36);
	dst_ext_data_offset = REG_READ(CFG_DSP_SRC_ADD + 40);

	bk_printf("code:0x%08x 0x%08x\r\n" "dst_code_offset:0x%08x \r\n"
			  "data:0x%08x 0x%08x\r\n" "dst_data_offset:0x%08x \r\n"
			  "ext_data:0x%08x 0x%08x\r\n" "dst_ext_data_offset:0x%08x \r\n"
			  , src_code_offset, src_code_len, dst_code_offset
			  , src_data_offset, src_data_len, dst_data_offset
			  , src_ext_data_offset, src_ext_data_len, dst_ext_data_offset);

	/* LBUS should be ready when copy dsp and bluetooth data */
	*((volatile unsigned long *) PMU_CO_MCU_CONFIG) |= 0x15;

	bk_printf("start code DL !! \r\n");
	i = 0;
	while (i < src_code_len) {
		*((volatile unsigned long *)(dst_code_base + dst_code_offset  + i)) =
			*((volatile unsigned long *)(CFG_DSP_SRC_ADD + src_code_offset + i));
		i += 4;
	}

	bk_printf("start data DL !! \r\n");
	i = 0;
	while (i < src_data_len) {
		*((volatile unsigned long *)(dst_data_base + dst_data_offset + i)) =
			*((volatile unsigned long *)(CFG_DSP_SRC_ADD + src_data_offset + i));
		i += 4;
	}

	if (*((volatile unsigned long *)(dst_data_base + dst_data_offset + i - 4)) !=
		*((volatile unsigned long *)(CFG_DSP_SRC_ADD + src_data_offset + i - 4)))
		bk_printf("last dst data error \r\n");

	bk_printf("start ext data DL !! \r\n");
	i = 0;
	while (i < src_ext_data_len) {
		*((volatile unsigned long *)(dst_ext_data_base + dst_ext_data_offset + i)) =
			*((volatile unsigned long *)(CFG_DSP_SRC_ADD + src_ext_data_offset + i));
		i += 4;
	}

	if (*((volatile unsigned long *)(dst_ext_data_base + dst_ext_data_offset + i - 4)) !=
		*((volatile unsigned long *)(CFG_DSP_SRC_ADD + src_ext_data_offset + i - 4)))
		bk_printf("last dst ext data error \r\n");

	*((volatile unsigned long *) PMU_CO_MCU_CONFIG) &= ~(0x1 << 0);
}

void dsp_reset(void)
{
	*((volatile unsigned long *) LBUS_CONF0_REG) |= DSP_DOWNLOAD_ENABLE;

	rtos_delay_milliseconds(100);

	*((volatile unsigned long *) LBUS_CONF0_REG) &= ~DSP_DOWNLOAD_ENABLE;
}
#endif

void dsp_init(void)
{
	if (get_ate_mode_state()) {
		return;
	}
#if CFG_USE_DSP
	DSP_PRT("dsp_init\r\n");
	dsp_start();
	g_dsp_inited = 1;
#endif
}

uint32_t dsp_is_inited(void)
{
	return g_dsp_inited;
}


void dsp_exit(void)
{
}
#endif // (CFG_SOC_NAME == SOC_BK7271)
// eof



