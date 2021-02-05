#include "include.h"
#include "arm_arch.h"

#if(CFG_SOC_NAME == SOC_BK7271)
#include "spi_bk7271.h"
#include "spi_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "mcu_ps_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"

#define SPI3_PERI_CLK_26M		(26 * 1000 * 1000)
#define SPI3_PERI_CLK_DCO		(120 * 1000 * 1000)

static SDD_OPERATIONS spi3_op = {
	spi3_ctrl
};

static void spi3_active(BOOLEAN val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~SPIEN;
	else if (val == 1)
		value |= SPIEN;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_set_msten(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~MSTEN;
	else if (val == 1)
		value |= MSTEN;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_set_ckpha(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~CKPHA;
	else if (val == 1)
		value |= CKPHA;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_set_skpol(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~CKPOL;
	else if (val == 1)
		value |= CKPOL;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_set_bit_wdth(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~BIT_WDTH;
	else if (val == 1)
		value |= BIT_WDTH;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_set_nssmd(UINT8 val)
{
	UINT32 value;
	value = REG_READ(SPI3_CTRL);
	value &= ~CTRL_NSSMD_3;
	value |= (val << 17);
	REG_WRITE(SPI3_CTRL, value);
}

/*
    spi3_clk : 90M/3=30M    - DC0180 - DIV2
    spi3_clk : 90M/4=22.5M  - DC0180 - DIV3
    spi3_clk : 90M/5=18M    - DC0180 - DIV4
    spi3_clk : 90M/6=15M    - DC0180 - DIV5
    spi3_clk : 90M/7=12.85M - DC0180 - DIV6
    spi3_clk : 90M/8=11.25M - DC0180 - DIV7
    spi3_clk : 90M/9=10M    - DC0180 - DIV8
    spi3_clk : 90M/10=9M    - DC0180 - DIV9
    spi3_clk : 90M/11=8.18M - DC0180 - DIV10
*/
static void spi3_set_clock(UINT32 max_hz)
{
	int source_clk = 0;
	int spi_clk = 0;
	int div = 0;
	UINT32 param;

	if (max_hz > 4333000) {
		BK_SPI_PRT("config spi3 clk source DCO\n");

		if (max_hz > 30000000) { // 180M/2 / (2 + 1) = 30M
			spi_clk = 30000000;
			BK_SPI_PRT("input clk > 30MHz, set input clk = 30MHz\n");
		} else
			spi_clk = max_hz;

		source_clk = SPI3_PERI_CLK_DCO;
		param = PCLK_POSI_SPI3;
		sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_DCO, &param);
	} else {
		BK_SPI_PRT("config spi clk source 26MHz\n");

		spi_clk = max_hz;
#if CFG_XTAL_FREQUENCE
		source_clk = CFG_XTAL_FREQUENCE;
#else
		source_clk = SPI3_PERI_CLK_26M;
#endif

		param = PCLK_POSI_SPI3;
		sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
	}

	// spi_clk = in_clk / (2 * (div + 1))
	div = ((source_clk >> 1) / spi_clk) - 1;

	if (div < 2)
		div = 2;
	else if (div >= 255)
		div = 255;

	param = REG_READ(SPI3_CTRL);
	param &= ~(SPI_CKR_MASK << SPI_CKR_POSI);
	param |= (div << SPI_CKR_POSI);
	REG_WRITE(SPI3_CTRL, param);

	BK_SPI_PRT("div = %d \n", div);
	BK_SPI_PRT("spi_clk = %d \n", spi_clk);
	BK_SPI_PRT("source_clk = %d \n", source_clk);
	BK_SPI_PRT("target frequency = %d, actual frequency = %d \n", max_hz, source_clk / 2 / (div + 1));
}


static void spi3_rxint_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~RXINT_EN;
	else if (val == 1)
		value |= RXINT_EN;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_txint_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~TXINT_EN;
	else if (val == 1)
		value |= TXINT_EN;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_rxovr_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~RXOVR_EN;
	else if (val == 1)
		value |= RXOVR_EN;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_txovr_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~TXOVR_EN;
	else if (val == 1)
		value |= TXOVR_EN;
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_rxint_mode(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);

	value &= ~(RXINT_MODE_MASK << RXINT_MODE_POSI);
	value |= ((val & RXINT_MODE_MASK) << RXINT_MODE_POSI);

	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_txint_mode(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);

	value &= ~(TXINT_MODE_MASK << TXINT_MODE_POSI);
	value |= ((val & TXINT_MODE_MASK) << TXINT_MODE_POSI);

	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_slave_release_int_enable(UINT32 enable)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (enable)
		value |= SPI_S_CS_UP_INT_EN;
	else
		value &= ~(SPI_S_CS_UP_INT_EN);
	REG_WRITE(SPI3_CTRL, value);
}

static void spi3_gpio_configuration(void)
{
	uint32_t val;
#ifdef SPI3_GPIO_MODE2
	val = GFUNC_MODE_SPI3_2;
#else
	val = GFUNC_MODE_SPI3_1;
#endif
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);
}

static void spi3_icu_configuration(UINT32 enable)
{
	UINT32 param;

	if (enable) {
		param = PWD_SPI3_CLK_BIT;
		sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);

		param = (IRQ_SPI3_BIT);
		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
	} else {
		param = (IRQ_SPI3_BIT);
		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);

		param = PWD_SPI3_CLK_BIT;
		sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
	}
}

static void spi3_lsb_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CTRL);
	if (val == 0)
		value &= ~LSB_FIRST;
	else if (val == 1)
		value |= LSB_FIRST;
	REG_WRITE(SPI3_CTRL, value);
}


static void spi3_tx_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CONFIG);
	if (val == 0)
		value &= ~SPI_TX_EN;
	else if (val == 1)
		value |= SPI_TX_EN;
	REG_WRITE(SPI3_CONFIG, value);
}

static void spi3_rx_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CONFIG);
	if (val == 0)
		value &= ~SPI_RX_EN;
	else if (val == 1)
		value |= SPI_RX_EN;
	REG_WRITE(SPI3_CONFIG, value);
}

static void spi3_txfinish_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CONFIG);
	if (val == 0)
		value &= ~SPI_TX_FINISH_EN;
	else if (val == 1)
		value |= SPI_TX_FINISH_EN;
	REG_WRITE(SPI3_CONFIG, value);

}

static void spi3_rxfinish_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CONFIG);
	if (val == 0)
		value &= ~SPI_RX_FINISH_EN;
	else if (val == 1)
		value |= SPI_RX_FINISH_EN;
	REG_WRITE(SPI3_CONFIG, value);

}

static void set_txtrans_len(UINT32 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CONFIG);

	value &= ~(0xFFF << SPI_TX_TRAHS_LEN_POSI);
	value |= ((val & 0xFFF) << SPI_TX_TRAHS_LEN_POSI);

	REG_WRITE(SPI3_CONFIG, value);
}

static void set_rxtrans_len(UINT32 val)
{
	UINT32 value;

	value = REG_READ(SPI3_CONFIG);

	value &= ~(0xFFF << SPI_RX_TRAHS_LEN_POSI);
	value |= ((val & 0xFFF) << SPI_RX_TRAHS_LEN_POSI);

	REG_WRITE(SPI3_CONFIG, value);
}

static void spi3_init_msten(UINT8 param)
{
	UINT32 value = 0;
	UINT8 msten = (param & 0x0F);

	value = REG_READ(SPI3_CTRL);
	value &= ~((TXINT_MODE_MASK << TXINT_MODE_POSI) | (RXINT_MODE_MASK << RXINT_MODE_POSI));

	value |= RXOVR_EN
			 | TXOVR_EN
			 | (0x3UL << RXINT_MODE_POSI)   // fifo_level :32
			 | (0x3UL << TXINT_MODE_POSI);	//  fifo_level :32

	REG_WRITE(SPI3_CTRL, value);
	if (msten == 0)
		spi3_slave_release_int_enable(1);
	else
		spi3_slave_release_int_enable(0);

	spi3_icu_configuration(1);
	spi3_gpio_configuration();
}

static void spi3_deinit_msten(void)
{
	UINT32 status, slv_status;

	spi3_icu_configuration(0);

	REG_WRITE(SPI3_CTRL, 0);

	status = REG_READ(SPI3_STAT);
	REG_WRITE(SPI3_STAT, status);
}

static void spi3_rxfifo_clr(void)
{
	UINT32 value;

	value = REG_READ(SPI3_STAT);

	while (value & RXFIFO_RD_READ) {
		REG_READ(SPI3_DAT);
		value = REG_READ(SPI3_STAT);
	}
}

UINT32 spi3_read_rxfifo(UINT8 *data)
{
	UINT32 value;

	value = REG_READ(SPI3_STAT);

	if (value & RXFIFO_RD_READ)

	{
		REG_WRITE((0x00802800 + (0x1c * 4)), 0x02);
		REG_WRITE((0x00802800 + (0x1c * 4)), 0x00);

		value = REG_READ(SPI3_DAT);
		if (data)
			*data = value;
		return 1;
	}

	return 0;
}

UINT32 spi3_write_txfifo(UINT8 data)
{
	UINT32 value;

	value = REG_READ(SPI3_STAT);

	if (value & TXFIFO_WR_READ) {
		REG_WRITE(SPI3_DAT, data);
		return 1;
	}

	return 0;
}

static struct spi_callback_des spi3_receive_callback = {NULL, NULL};
static struct spi_callback_des spi3_txfifo_needwr_callback = {NULL, NULL};
static struct spi_callback_des spi3_tx_end_callback = {NULL, NULL};

static void spi3_rx_callback_set(spi_callback callback, void *param)
{
	spi3_receive_callback.callback = callback;
	spi3_receive_callback.param = param;
}

static void spi3_tx_fifo_needwr_callback_set(spi_callback callback, void *param)
{
	spi3_txfifo_needwr_callback.callback = callback;
	spi3_txfifo_needwr_callback.param = param;
}

static void spi3_tx_end_callback_set(spi_callback callback, void *param)
{
	spi3_tx_end_callback.callback = callback;
	spi3_tx_end_callback.param = param;
}


UINT32 spi3_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret = SPI_SUCCESS;

	peri_busy_count_add();

	switch (cmd) {
	case CMD_SPI3_UNIT_ENABLE:
		spi3_active(*(UINT8 *)param);
		break;
	case CMD_SPI3_SET_MSTEN:
		spi3_set_msten(*(UINT8 *)param);
		break;
	case CMD_SPI3_SET_CKPHA:
		spi3_set_ckpha(*(UINT8 *)param);
		break;
	case CMD_SPI3_SET_CKPOL:
		spi3_set_skpol(*(UINT8 *)param);
		break;
	case CMD_SPI3_SET_BITWIDTH:
		spi3_set_bit_wdth(*(UINT8 *)param);
		break;
	case CMD_SPI3_SET_NSSMD:
		spi3_set_nssmd(*(UINT8 *)param);
		break;
	case CMD_SPI3_SET_CKR:
		spi3_set_clock(*(UINT32 *)param);
		break;
	case CMD_SPI3_RXINT_EN:
		spi3_rxint_enable(*(UINT8 *)param);
		break;
	case CMD_SPI3_TXINT_EN:
		spi3_txint_enable(*(UINT8 *)param);
		break;
	case CMD_SPI3_RXOVR_EN:
		spi3_rxovr_enable(*(UINT8 *)param);
		break;
	case CMD_SPI3_TXOVR_EN:
		spi3_txovr_enable(*(UINT8 *)param);
		break;
	case CMD_SPI3_RXFIFO_CLR:
		spi3_rxfifo_clr();
		break;
	case CMD_SPI3_RXINT_MODE:
		spi3_rxint_mode(*(UINT8 *)param);
		break;
	case CMD_SPI3_TXINT_MODE:
		spi3_txint_mode(*(UINT8 *)param);
		break;
	case CMD_SPI3_INIT_MSTEN:
		spi3_init_msten(*(UINT8 *)param);
		break;
	case CMD_SPI3_GET_BUSY:
		break;
	case CMD_SPI3_SET_RX_CALLBACK: {
		struct spi_callback_des *callback = (struct spi_callback_des *)param;
		spi3_rx_callback_set(callback->callback, callback->param);
	}
	break;
	case CMD_SPI3_SET_TX_NEED_WRITE_CALLBACK: {
		struct spi_callback_des *callback = (struct spi_callback_des *)param;
		spi3_tx_fifo_needwr_callback_set(callback->callback, callback->param);
	}
	break;
	case CMD_SPI3_SET_TX_FINISH_CALLBACK: {
		struct spi_callback_des *callback = (struct spi_callback_des *)param;
		spi3_tx_end_callback_set(callback->callback, callback->param);
	}
	break;
	case CMD_SPI3_DEINIT_MSTEN:
		spi3_deinit_msten();
		break;
	case CMD_SPI3_LSB_EN:
		spi3_lsb_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI3_TX_EN:
		spi3_tx_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI3_RX_EN:
		spi3_rx_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI3_TXFINISH_EN:
		spi3_txfinish_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI3_RXFINISH_EN:
		spi3_rxfinish_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI3_TXTRANS_EN:
		set_txtrans_len(*(UINT32 *)param);
		break;
	case CMD_SPI3_RXTRANS_EN:
		set_txtrans_len(*(UINT32 *)param);
		break;
	case CMD_SPI3_CS_EN:
		spi3_slave_release_int_enable(*(UINT32 *)param);
		break;
	default:
		ret = SPI_FAILURE;
		break;
	}

	peri_busy_count_dec();

	return ret;
}

void spi3_isr(void)
{
	UINT32 status, slv_status;
	volatile UINT8 fifo_empty_num, data_num, rxfifo_empty;

	//REG_WRITE((0x00802800+(19*4)), 0x02);
	//REG_WRITE((0x00802800+(0x1a*4)), 0x02);
	//REG_WRITE((0x00802800+(0x1a*4)), 0x00);

	data_num = 0; /*fix warning by clang analyzer*/
	fifo_empty_num = 0; /*fix warning by clang analyzer*/

	status = REG_READ(SPI3_STAT);
	REG_WRITE(SPI3_STAT, status);

	os_printf("0x%08x, 0x%08x\r\n", status, slv_status);
	//REG_WRITE((0x00802800+(19*4)), 0x00);

	if ((status & RXINT) || (status & SPI_S_CS_UP_INT_STATUS)) {
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x02);

		if (spi3_receive_callback.callback != 0) {
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x02);
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x00);

			void *param = spi3_receive_callback.param;
			int is_rx_end = (status & SPI_S_CS_UP_INT_STATUS) ? 1 : 0;
			spi3_receive_callback.callback(is_rx_end, param);
		} else {
			/*drop data*/
			spi3_rxfifo_clr();
		}
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x00);
	}

	if (status & TXINT) {
		//REG_WRITE((0x00802800+(0x1c*4)), 0x02);
		//REG_WRITE((0x00802800+(0x1c*4)), 0x00);
		os_printf("spi3 txint\r\n");

		if (spi3_txfifo_needwr_callback.callback != 0) {
			void *param = spi3_txfifo_needwr_callback.param;

			spi3_txfifo_needwr_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi3_txfifo_fill();
		}
	}

	if (status & TXOVR)
		os_printf("txovr\r\n");

	if (status & RXOVR)
		os_printf("rxovr\r\n");

	if (status & RX_FINISH_INT)
		os_printf("rx finish int \r\n");

	if (status & TXFIFO_WR_READ)

	{
		if (spi3_tx_end_callback.callback != 0) {
			void *param = spi3_tx_end_callback.param;

			spi3_tx_end_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi3_txfifo_fill();
		}
	}
}

void spi3_init(void)
{
	intc_service_register(IRQ_SPI3, PRI_IRQ_SPI, spi3_isr);

	sddev_register_dev(SPI3_DEV_NAME, &spi3_op);
}

void spi3_exit(void)
{
	sddev_unregister_dev(SPI3_DEV_NAME);
}

#endif
// eof

