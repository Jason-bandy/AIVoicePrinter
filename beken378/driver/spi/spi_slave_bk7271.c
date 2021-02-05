#include "include.h"
#include "arm_arch.h"

#include "typedef.h"
#include "arm_arch.h"
#include "icu_pub.h"
#include "spi_pub.h"
#include "gpio_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "mem_pub.h"
#include "sys_config.h"
#include "error.h"
#include "rtos_pub.h"
#include "general_dma_pub.h"
#include "general_dma.h"

#if(CFG_SOC_NAME == SOC_BK7271)
#include "spi_bk7271.h"
#if CFG_USE_SPI_SLAVE
#define TRUE		1
#define FALSE		0

#define SPI_SLAVE_RX_FIFO_LEN      (512)

struct spi_rx_fifo {
	UINT8 *buffer;

	UINT16 put_index, get_index;

	UINT32 is_full;
};

struct bk_spi_slave_dev {
	UINT32 flag;

	beken_semaphore_t tx_sem;
	UINT8 *tx_ptr;
	UINT32 tx_len;

	beken_semaphore_t rx_sem;
	struct spi_rx_fifo *rx_fifo;

	beken_mutex_t mutex;
};

static struct bk_spi_slave_dev *spi_slave_dev;

static UINT32 bk_spi_slave_get_rx_fifo(void)
{
	UINT32 rx_length;
	struct spi_rx_fifo *rx_fifo = spi_slave_dev->rx_fifo;
	GLOBAL_INT_DECLARATION();

	/* get rx length */
	GLOBAL_INT_DISABLE();

	rx_length = (rx_fifo->put_index >= rx_fifo->get_index) ?
				(rx_fifo->put_index - rx_fifo->get_index) :
				(SPI_SLAVE_RX_FIFO_LEN - (rx_fifo->get_index - rx_fifo->put_index));

	GLOBAL_INT_RESTORE();

	return rx_length;
}

static void bk_spi_slave_spi_rx_callback(int is_rx_end, void *param)
{
	UINT8 ch;
	struct spi_rx_fifo *rx_fifo;
	//GLOBAL_INT_DECLARATION();

	rx_fifo = (struct spi_rx_fifo *)spi_slave_dev->rx_fifo;
	ASSERT(rx_fifo != NULL);

	os_printf("rx callback:rx_end:%d\r\n ", is_rx_end);
	//REG_WRITE((0x00802800+(18*4)), 0x02);
	while (1) {
		if (spi_read_rxfifo(&ch) == 0)
			break;
		//REG_WRITE((0x00802800+(0x1a*4)), 0x02);
		//REG_WRITE((0x00802800+(0x1a*4)), 0x00);
		//GLOBAL_INT_DISABLE();

		rx_fifo->buffer[rx_fifo->put_index] = ch;
		rx_fifo->put_index += 1;
		if (rx_fifo->put_index >= SPI_SLAVE_RX_FIFO_LEN)
			rx_fifo->put_index = 0;

		if (rx_fifo->put_index == rx_fifo->get_index) {
			rx_fifo->get_index += 1;
			rx_fifo->is_full = TRUE;
			if (rx_fifo->get_index >= SPI_SLAVE_RX_FIFO_LEN)
				rx_fifo->get_index = 0;
		}
		//GLOBAL_INT_RESTORE();

		if (spi_slave_dev->tx_ptr == NULL)
			spi_write_txfifo(0xFF);
	}

	if (is_rx_end) {
		// only rx end happened, wake up rx_semp
		os_printf("----> rx end\r\n");
		rtos_set_semaphore(&spi_slave_dev->rx_sem);
	}
	//REG_WRITE((0x00802800+(18*4)), 0x00);
}

static int bk_spi_slave_get_rx_data(UINT8 *rx_buf, int len)
{
	struct spi_rx_fifo *rx_fifo;
	rx_fifo = (struct spi_rx_fifo *)spi_slave_dev->rx_fifo;
	int size = len;
	os_printf("len:%d\r\n", len);

	//os_printf("rx_fifo:%d\r\n", rx_fifo);

	ASSERT(rx_fifo != NULL);

	//os_printf("rx_buf:%d\r\n", rx_buf);

	if (rx_buf == NULL)
		return 0;

	//os_printf("%d %d %d\r\n", bk_spi_slave_get_rx_fifo(),
	//rx_fifo->get_index, rx_fifo->put_index);

	while (size) {
		uint8_t ch;
		GLOBAL_INT_DECLARATION();

		GLOBAL_INT_DISABLE();

		if ((rx_fifo->get_index == rx_fifo->put_index)
			&& (rx_fifo->is_full == FALSE)) {
			GLOBAL_INT_RESTORE();
			os_printf("break:get rx data \r\n");
			break;
		}

		ch = rx_fifo->buffer[rx_fifo->get_index];
		rx_fifo->get_index += 1;
		if (rx_fifo->get_index >= SPI_SLAVE_RX_FIFO_LEN)
			rx_fifo->get_index = 0;

		if (rx_fifo->is_full == TRUE)
			rx_fifo->is_full = FALSE;

		GLOBAL_INT_RESTORE();

		*rx_buf = ch & 0xff;
		rx_buf ++;
		size --;

	}

	return (len - size);
}

static void bk_spi_slave_tx_needwrite_callback(int port, void *param)
{
	UINT8 *tx_ptr = spi_slave_dev->tx_ptr;
	UINT32 tx_len = spi_slave_dev->tx_len;
	GLOBAL_INT_DECLARATION();

	if (tx_ptr && tx_len) {
		UINT8 data = *tx_ptr;

		while (spi_write_txfifo(data) == 1) {
			spi_read_rxfifo(&data);
			tx_len --;
			tx_ptr ++;
			if (tx_len == 0) {
				UINT32 enable = 0;
				sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&enable);
				break;
			}
			data = *tx_ptr;
		}
	} else {
		//rt_kprintf("nw:%p,%d\r\n", tx_ptr, tx_len);
		while (spi_write_txfifo(0xff)) {
			if (tx_len)
				tx_len--;

			if (tx_len == 0) {
				os_printf("close tx\r\n");
				UINT32 enable = 0;
				sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&enable);
				break;
			}
		}
	}

	GLOBAL_INT_DISABLE();
	spi_slave_dev->tx_ptr = tx_ptr;
	spi_slave_dev->tx_len = tx_len;
	GLOBAL_INT_RESTORE();

}

static void bk_spi_slave_tx_finish_callback(int port, void *param)
{
	if ((spi_slave_dev->tx_len == 0) && (spi_slave_dev->tx_ptr)) {
		if ((spi_slave_dev->flag & TX_FINISH_FLAG) == 0) {
			spi_slave_dev->flag |= TX_FINISH_FLAG;
			rtos_set_semaphore(&spi_slave_dev->tx_sem);
		}
	}
}

static void bk_spi_slave_configure(UINT32 rate, UINT32 mode)
{
	UINT32 param;
	struct spi_callback_des spi_dev_cb;

	/* data bit width */
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_BITWIDTH, (void *)&param);

	/* baudrate */
	BK_SPI_PRT("max_hz = %d \n", rate);
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKR, (void *)&rate);

	/* mode */
	if (mode & BK_SPI_CPOL)
		param = 1;
	else
		param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKPOL, (void *)&param);

	/* CPHA */
	if (mode & BK_SPI_CPHA)
		param = 1;
	else
		param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKPHA, (void *)&param);

	/* slave */
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_MSTEN, (void *)&param);

#if (CFG_SOC_NAME != SOC_BK7231N)
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_NSSMD, (void *)&param);
#else
	// 4line :7231N nssms is 0
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_NSSMD, (void *)&param);
#endif

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_INIT_MSTEN, (void *)&param);


	/* set call back func */
	spi_dev_cb.callback = bk_spi_slave_spi_rx_callback;
	spi_dev_cb.param = NULL;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_RX_CALLBACK, (void *)&spi_dev_cb);

	spi_dev_cb.callback = bk_spi_slave_tx_needwrite_callback;
	spi_dev_cb.param = NULL;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_TX_NEED_WRITE_CALLBACK, (void *)&spi_dev_cb);

	spi_dev_cb.callback = bk_spi_slave_tx_finish_callback;
	spi_dev_cb.param = NULL;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_TX_FINISH_CALLBACK, (void *)&spi_dev_cb);

#if (CFG_SOC_NAME != SOC_BK7231N)
	/* enable rx int */
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXINT_EN, (void *)&param);
#else
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_LSB_EN, (void *)&param);

	//enable rx/tx finish enable bit
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXFINISH_EN, (void *)&param);

	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXFINISH_EN, (void *)&param);

#ifdef SPI_DMA_TRANS
	//disable rx int bit
	param = 0;
#else
	//disable rx int bit
	param = 1;
#endif
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXINT_EN, (void *)&param);
#endif

	/* enable spi */
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_UNIT_ENABLE, (void *)&param);

	//BK_SPI_PRT("spi_slave [CTRL]:0x%08x \n", REG_READ(SPI_CTRL));

#if (CFG_SOC_NAME == SOC_BK7231N)
	//BK_SPI_PRT("spi_slave [CONFIG]:0x%08x \n", REG_READ(SPI_CONFIG));
#endif

}

static void bk_spi_slave_unconfigure(void)
{
	sddev_control(SPI_DEV_NAME, CMD_SPI_DEINIT_MSTEN, NULL);
}

int bk_spi_slave_xfer(struct spi_message *msg)
{
	UINT8 *recv_ptr = NULL;
	const UINT8 *send_ptr = NULL;
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();

	ASSERT(spi_slave_dev != NULL);
	ASSERT(msg != NULL);

	rtos_lock_mutex(&spi_slave_dev->mutex);

	recv_ptr = msg->recv_buf;
	recv_len = msg->recv_len;
	send_ptr = msg->send_buf;
	send_len = msg->send_len;

#if (CFG_SOC_NAME == SOC_BK7231N)
	//new spi hardware bug
	param = send_len - 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXTRANS_EN, (void *)&param);

	param = recv_len;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXTRANS_EN, (void *)&param);
#endif

	//enbale rx/tx enable bit
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TX_EN, (void *)&param);

	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RX_EN, (void *)&param);

	BK_SPI_PRT("spi_slave [CTRL]:0x%08x \n", REG_READ(SPI_CTRL));

#if (CFG_SOC_NAME == SOC_BK7231N)
	BK_SPI_PRT("spi_slave [CONFIG]:0x%08x \n", REG_READ(SPI_CONFIG));
#endif

	if ((send_ptr) && send_len) {
		GLOBAL_INT_DISABLE();
		spi_slave_dev->tx_ptr = (UINT8 *)send_ptr;
		spi_slave_dev->tx_len = send_len;
		spi_slave_dev->flag &= ~(TX_FINISH_FLAG);
		GLOBAL_INT_RESTORE();

		param = 1;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		//BK_SPI_PRT("0 %p-%d\r\n", send_ptr, send_len);
		rtos_get_semaphore(&spi_slave_dev->tx_sem, BEKEN_NEVER_TIMEOUT);

		param = 0;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		GLOBAL_INT_DISABLE();
		spi_slave_dev->tx_ptr = NULL;
		spi_slave_dev->tx_len = 0;
		spi_slave_dev->flag |= TX_FINISH_FLAG;
		GLOBAL_INT_RESTORE();

		//BK_SPI_PRT("1 %p-%d\r\n", send_ptr, send_len);
		param = send_len;
	} else if ((recv_ptr) && recv_len) {
		OSStatus err;
		int len;

		GLOBAL_INT_DISABLE();
		spi_slave_dev->tx_ptr = NULL;
		spi_slave_dev->tx_len = recv_len;
		GLOBAL_INT_RESTORE();

		param = 1;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		do {
			len = bk_spi_slave_get_rx_data(recv_ptr, recv_len);
			if (len == 0) {
				err = rtos_get_semaphore(&spi_slave_dev->rx_sem, BEKEN_WAIT_FOREVER);
				if (err != kNoErr)
					break;
			}
		} while (len == 0);

		param = 0;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		// clear all rx semp for this time
		do {
			err = rtos_get_semaphore(&spi_slave_dev->rx_sem, 0);
		} while (err == kNoErr);

		param = len;
	}

	rtos_unlock_mutex(&spi_slave_dev->mutex);

	return param;
}

int bk_spi_slave_init(UINT32 rate,  UINT32 mode)
{
	OSStatus result = 0;

	if (spi_slave_dev)
		bk_spi_slave_deinit();

	spi_slave_dev = os_malloc(sizeof(struct bk_spi_slave_dev));
	if (!spi_slave_dev) {
		BK_SPI_PRT("[spi]:malloc memory for spi_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi_slave_dev, 0, sizeof(struct bk_spi_slave_dev));

	result = rtos_init_semaphore(&spi_slave_dev->tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi tx semp init failed\n");
		goto _exit;
	}

	result = rtos_init_semaphore(&spi_slave_dev->rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi rx semp init failed\n");
		goto _exit;
	}

	result = rtos_init_mutex(&spi_slave_dev->mutex);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi mutex init failed\n");
		goto _exit;
	}

	struct spi_rx_fifo *rx_fifo;

	rx_fifo = (struct spi_rx_fifo *)os_malloc(sizeof(struct spi_rx_fifo) +
			  SPI_SLAVE_RX_FIFO_LEN);
	if (!rx_fifo) {
		BK_SPI_PRT("[spi]: spi rx fifo malloc failed\n");
		goto _exit;
	}

	rx_fifo->buffer = (uint8_t *)(rx_fifo + 1);
	os_memset(rx_fifo->buffer, 0, SPI_SLAVE_RX_FIFO_LEN);
	rx_fifo->put_index = 0;
	rx_fifo->get_index = 0;
	rx_fifo->is_full = 0;

	spi_slave_dev->rx_fifo = rx_fifo;

	spi_slave_dev->tx_ptr = NULL;
	spi_slave_dev->tx_len = 0;
	spi_slave_dev->flag |= TX_FINISH_FLAG;

	bk_spi_slave_configure(rate, mode);

	return 0;

_exit:
	if (spi_slave_dev->mutex)
		rtos_deinit_mutex(&spi_slave_dev->mutex);

	if (spi_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->tx_sem);

	if (spi_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->rx_sem);

	if (spi_slave_dev->rx_fifo)
		os_free(spi_slave_dev->rx_fifo);

	if (spi_slave_dev) {
		os_free(spi_slave_dev);
		spi_slave_dev = NULL;
	}

	return 1;
}


int bk_spi_slave_recv(UINT32 rate,  UINT32 mode, UINT32 recv_len, struct spi_message *msg)
{
	UINT8 *buf;
	int rx_len;

	struct spi_configuration *cfg;

	cfg->data_width = 8;			//data_with 8 bit :defult
	cfg->max_hz   = rate;			//set spi clock
	cfg->mode = mode;

	bk_spi_slave_init(cfg->max_hz, cfg->mode);

	os_printf("cfg:%d, 0x%02x, %d\r\n", cfg->data_width, cfg->mode, cfg->max_hz);

	rx_len = recv_len;

	buf = os_malloc(rx_len * sizeof(UINT8));

	if (buf) {
		os_memset(buf, 0, rx_len);

		msg->send_buf = NULL;
		msg->send_len = 0;
		msg->recv_buf = buf;
		msg->recv_len = rx_len;

		//os_printf("buf:%d\r\n", buf);
		rx_len = bk_spi_slave_xfer(&msg);
		os_printf("rx_len:%d\r\n", rx_len);

		for (int i = 0; i < rx_len; i++) {
			os_printf("%02x,", buf[i]);
			if ((i + 1) % 32 == 0)
				os_printf("\r\n");
		}
		os_printf("\r\n");

		os_free(buf);
	} else
		os_printf("buff is null\r\n");
}

int bk_spi_slave_deinit(void)
{
	if (spi_slave_dev == NULL)
		return 0;

	bk_spi_slave_unconfigure();

	if (spi_slave_dev->mutex)
		rtos_lock_mutex(&spi_slave_dev->mutex);

	if (spi_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->tx_sem);

	if (spi_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->rx_sem);

	if (spi_slave_dev->rx_fifo)
		os_free(spi_slave_dev->rx_fifo);

	if (spi_slave_dev->mutex) {
		rtos_unlock_mutex(&spi_slave_dev->mutex);
		rtos_deinit_mutex(&spi_slave_dev->mutex);
	}

	os_free(spi_slave_dev);
	spi_slave_dev = NULL;

	return 0;
}

#endif  // CFG_USE_SPI_SLAVE
#endif  // iifdef SOC_BK7271

