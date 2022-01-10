#include "include.h"
#include "arm_arch.h"
#include "flash_bypass.h"
#include "gpio_pub.h"
#include "gpio.h"
#include "icu_pub.h"
#include "flash_pub.h"
#include "spi_pub.h"
#include "sys_ctrl.h"
#include "spi.h"
#include "icu.h"

#if ((CFG_SOC_NAME == SOC_BK7221U))
extern void spi_gpio_configuration(void);
extern void bk_spi_configure(UINT32 rate, UINT32 mode);

void flash_bypass_wr_sr_cb(uint32_t larg, uint32_t rarg)
{
	volatile int32_t delay_count, j;
	uint8_t sr_width = (uint8_t)larg;
	uint16_t sr_val = (uint16_t)rarg;
	uint32_t intc_enable_reg, reg;
	char *text_ptr, temp_buf = 0;
	uint32_t i, reg_ctrl, reg_dat;
	uint32_t reg_stat, reg_sctrl;
	uint32_t exceptional_flag = 0;
	UINT32 ret, mode, is_spi_mode;

	if (0 == flash_is_support_0x50h_cmd()
		|| (sr_width > 2))
		return;

	/*step 0, disable interrupt*/
	intc_enable_reg = REG_READ(ICU_GLOBAL_INT_EN);
	reg = intc_enable_reg & (~(GINTR_FIQ_EN | GINTR_IRQ_EN));
	REG_WRITE(ICU_GLOBAL_INT_EN, reg);

	/*step 1, save spi register configuration*/
	reg_ctrl = REG_READ(SPI_CTRL);
	reg_stat = REG_READ(SPI_STAT);
	reg_dat  = REG_READ(SPI_DAT);
	reg_sctrl = REG_READ(SPI_SLAVE_CTRL);

	/*step 2, resident cache*/
	REG_WRITE(SPI_CTRL, 0);
	do {
		text_ptr = (char *)flash_bypass_wr_sr_cb;
		for (i = 0; i < CURRENT_ROUTINE_TEXT_SIZE; i ++)
			temp_buf += text_ptr[i];

		REG_WRITE(SPI_STAT, temp_buf);
	} while (0);

	/*step 3, config spi master*/
	/*     3.1 clear spi fifo content*/
	reg = REG_READ(SPI_STAT);
	while ((reg & RXFIFO_EMPTY) == 0) {
		REG_READ(SPI_DAT);
		reg = REG_READ(SPI_STAT);
	}

	/*     3.2 disable spi block*/
	reg = REG_READ(ICU_PERI_CLK_PWD);
	reg |= (PWD_SPI_CLK_BIT);
	REG_WRITE(ICU_PERI_CLK_PWD, reg);

	/*     3.3 clear spi status*/
	REG_WRITE(SPI_CTRL, 0);
	reg = REG_READ(SPI_STAT);
	REG_WRITE(SPI_STAT, reg);

	reg = REG_READ(SPI_SLAVE_CTRL);
	REG_WRITE(SPI_SLAVE_CTRL, reg);

	/*     3.4 save the previous setting status*/
	ret = gpio_get_config(GPIO14, &mode);
	is_spi_mode = 0;
	if ((GPIO_SUCCESS == ret) && (PERIAL_MODE_2 == mode))
		is_spi_mode = 1;

	/*     3.5 set the spi master mode*/
	bk_spi_configure(SPI_DEF_CLK_HZ, SPI_DEF_MODE);

	/*step 4, gpio(14/15/16/17) are set as high-impedance state or input state ,
	          for spi mux with them*/
	gpio_config(GPIO14, GMODE_SET_HIGH_IMPENDANCE);
	gpio_config(GPIO15, GMODE_SET_HIGH_IMPENDANCE);
	gpio_config(GPIO16, GMODE_SET_HIGH_IMPENDANCE);
	gpio_config(GPIO17, GMODE_SET_HIGH_IMPENDANCE);

	/*step 5, switch flash interface to spi:sctrl_flash_select_spi_controller()
	 *        Pay attention to prefetch instruction destination, the text can not
	 *        fetch from flash space after this timepoint.
	 */
	reg = REG_READ(SCTRL_CONTROL);
	reg |= FLASH_SPI_MUX_BIT;
	REG_WRITE(SCTRL_CONTROL, reg);
	while (1) { /* double check the bit:FLASH_SPI_MUX_BIT. The bit cannot be set successfully, if cpu is fetching instruction*/
		reg = REG_READ(SCTRL_CONTROL);
		if (reg & FLASH_SPI_MUX_BIT)
			break;
	}

	/*step 6, write enable for volatile status register: 50H*/
	/*      6.1:take cs*/
	reg = REG_READ(SPI_CTRL);
	reg &= ~(NSSMD_MASK << NSSMD_POSI);
	reg |= (SPI_VAL_TAKE_CS << NSSMD_POSI);
	REG_WRITE(SPI_CTRL, reg);

	/*      6.2:write tx fifo*/
	reg = REG_READ(SPI_STAT);
	if ((reg & TXFIFO_FULL) == 0)
		REG_WRITE(SPI_DAT, FLASH_CMD_WR_EN_VSR);
	else {
		exceptional_flag = 1;
		goto wr_exceptional;
	}

	/*      6.3:waiting for TXFIFO_EMPTY interrupt*/
	while (1) {
		reg = REG_READ(SPI_STAT);
		if (reg & TXFIFO_EMPTY)
			break;
	}

	while (1) {
		reg = REG_READ(SPI_STAT);
		if (reg & SPIBUSY)
			break;
	}

	/*delay 1us--5us. the bits of 2 clock do not send over, if the spi status is TXFIFO_EMPTY*/
	for (delay_count = 0; delay_count < 1; delay_count ++) {
		for (j = 0; j < 4; j ++)
			;
	}

	/*      6.4:release cs*/
	reg = REG_READ(SPI_CTRL);
	reg &= ~(NSSMD_MASK << NSSMD_POSI);
	reg |= (SPI_VAL_RELEASE_CS << NSSMD_POSI);
	REG_WRITE(SPI_CTRL, reg);

	/*step 7, write flash command:01H and sr*/
	/*      7.1:take cs*/
	reg = REG_READ(SPI_CTRL);
	reg &= ~(NSSMD_MASK << NSSMD_POSI);
	reg |= (SPI_VAL_TAKE_CS << NSSMD_POSI);
	REG_WRITE(SPI_CTRL, reg);

	/*      7.2:write tx fifo*/
	reg = REG_READ(SPI_STAT);
	if ((reg & TXFIFO_FULL) == 0) {
		REG_WRITE(SPI_DAT, FLASH_CMD_WR_SR);
		for (int i = 0; i < sr_width; i ++)
			REG_WRITE(SPI_DAT, (sr_val >> (i * 8)) & 0xff);
	} else {
		exceptional_flag = 1;
		goto wr_exceptional;
	}

	/*      7.3:waiting for TXFIFO_EMPTY interrupt*/
	while (1) {
		reg = REG_READ(SPI_STAT);
		if (reg & TXFIFO_EMPTY)
			break;
	}

	while (1) {
		reg = REG_READ(SPI_STAT);
		if (reg & SPIBUSY)
			break;
	}

	/*delay 1us--5us. the bits of 2 clock do not send over, if the spi status is TXFIFO_EMPTY*/
	for (delay_count = 0; delay_count < 1; delay_count ++) {
		for (j = 0; j < 4; j ++)
			;
	}

	/*      7.4:release cs*/
	reg = REG_READ(SPI_CTRL);
	reg &= ~(NSSMD_MASK << NSSMD_POSI);
	reg |= (SPI_VAL_RELEASE_CS << NSSMD_POSI);
	REG_WRITE(SPI_CTRL, reg);

wr_exceptional:
	/*step 8, flash interface select: flash controller-->sctrl_flash_select_flash_controller()*/
	reg = REG_READ(SCTRL_CONTROL);
	reg &= ~(FLASH_SPI_MUX_BIT);
	REG_WRITE(SCTRL_CONTROL, reg);

	/*step 9, gpio(14/15/16/17) second function*/
	if (is_spi_mode)
		spi_gpio_configuration();

	/*step 10, restore spi register configuration*/
	REG_WRITE(SPI_CTRL, reg_ctrl);
	REG_WRITE(SPI_STAT, reg_stat);
	REG_WRITE(SPI_DAT, reg_dat);
	REG_WRITE(SPI_SLAVE_CTRL, reg_sctrl);

	/*step 11, enable interrupt*/
	REG_WRITE(ICU_GLOBAL_INT_EN, intc_enable_reg);

	if (exceptional_flag)
		os_printf("fb_wr_sr failed\r\n");
}

uint32_t flash_bypass_operate_sr_init(void)
{
	flash_register_bypass_cb(flash_bypass_wr_sr_cb);

	return 0;
}
#endif
// eof

