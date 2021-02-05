#include "include.h"
#include "arm_arch.h"
#include "typedef.h"
#include "arm_arch.h"
#include "icu_pub.h"
#include "spi_pub.h"
#include "spi_bk7271.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "mem_pub.h"
#include "sys_config.h"
#include "error.h"
#include "rtos_pub.h"
#include "general_dma_pub.h"
#include "general_dma.h"

#if(CFG_SOC_NAME == SOC_BK7271)
#if CFG_USE_SPI3_DMA

struct bk_spi3_dev {
	UINT8 *tx_ptr;
	UINT32 tx_len;
	beken_semaphore_t tx_sem;
	beken_semaphore_t rx_sem;

	UINT8 *rx_ptr;
	UINT32 rx_len;
	UINT32 rx_offset;
	UINT32 rx_drop;

	UINT32 total_len;
	UINT32 flag;

	beken_mutex_t mutex;
};

struct bk_spi3_slave_dev {
	UINT32 flag;

	beken_semaphore_t tx_sem;
	UINT8 *tx_ptr;
	UINT32 tx_len;

	beken_semaphore_t rx_sem;
	struct spi_rx_fifo *rx_fifo;

	beken_mutex_t mutex;
};

static struct bk_spi3_dev *spi3_dev;
static struct bk_spi3_slave_dev *spi3_slave_dev;

int spi3_dma_trans_flag = 0 ;

#define SPI3_TEST_POART1		0
#define SPI3_TEST_POART2		1
#define SPI3_TX_BUFFER_SIZE		1024
#define SPI3_RX_BUFFER_SIZE		1024*2
#define SPI3_RX_DMA_CHANNEL     GDMA_CHANNEL_1
#define SPI3_TX_DMA_CHANNEL     GDMA_CHANNEL_3

static void spi3_debug_prt(void)
{
	int reg_addr = 0;

	// wf debug
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x08) * 4));
	TUART_PRT("reg08:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x09) * 4));
	TUART_PRT("reg09:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x0a) * 4));
	TUART_PRT("reg0a:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x1b) * 4));
	TUART_PRT("reg0b:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x0c) * 4));
	TUART_PRT("reg0c:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x0d) * 4));
	TUART_PRT("reg0d:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x0e) * 4));
	TUART_PRT("reg0e:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x0f) * 4));
	TUART_PRT("reg0f:0x%lx\r\n", reg_addr);
}

static void bk_spi3_master_configure(UINT32 rate, UINT32 mode)
{
	UINT32 param;

	/* data bit width */
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_BITWIDTH, (void *)&param);

	/* baudrate */
	BK_SPI_PRT("max_hz = %d \n", rate);
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_CKR, (void *)&rate);

	/* mode */
	if (mode & BK_SPI_CPOL)
		param = 1;
	else
		param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_CKPOL, (void *)&param);

	/* CPHA */
	if (mode & BK_SPI_CPHA)
		param = 1;
	else
		param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_CKPHA, (void *)&param);

	/* Master */
	param = 1;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_MSTEN, (void *)&param);
	param = 3;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_NSSMD, (void *)&param);
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_INIT_MSTEN, (void *)&param);

	/* enable spi3 */
	param = 1;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_UNIT_ENABLE, (void *)&param);

	BK_SPI_PRT("spi3_master:[CTRL]:0x%08x \n", REG_READ(SPI3_CTRL));
}

static void bk_spi3_slave_configure(UINT32 rate, UINT32 mode)
{
	UINT32 param;

	/* data bit width */
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_BITWIDTH, (void *)&param);

	/* baudrate */
	BK_SPI_PRT("max_hz = %d \n", rate);
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_CKR, (void *)&rate);

	/* mode */
	if (mode & BK_SPI_CPOL)
		param = 1;
	else
		param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_CKPOL, (void *)&param);

	/* CPHA */
	if (mode & BK_SPI_CPHA)
		param = 1;
	else
		param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_CKPHA, (void *)&param);

	/* slave */
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_MSTEN, (void *)&param);

	// 4line :7271 NSSMD is 0
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_NSSMD, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_INIT_MSTEN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_LSB_EN, (void *)&param);

	//enable rx/tx finish enable bit
	param = 1;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TXFINISH_EN, (void *)&param);

	param = 1;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXFINISH_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXINT_EN, (void *)&param);

	/* enable spi3 */
	param = 1;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_UNIT_ENABLE, (void *)&param);

	BK_SPI_PRT("spi3_slave [CONFIG]:0x%08x \n", REG_READ(SPI3_CONFIG));

}

static void bk_spi3_unconfigure(void)
{
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_DEINIT_MSTEN, NULL);
}

int bk_spi3_master_deinit(void)
{
	if (spi3_dev == NULL)
		return 0;

	if (spi3_dev->mutex)
		rtos_lock_mutex(&spi3_dev->mutex);

	if (spi3_dev->tx_sem)
		rtos_deinit_semaphore(&spi3_dev->tx_sem);

	if (spi3_dev->mutex) {
		rtos_unlock_mutex(&spi3_dev->mutex);
		rtos_deinit_mutex(&spi3_dev->mutex);
	}

	if (spi3_dev) {
		os_free(spi3_dev);
		spi3_dev = NULL;
	}

	bk_spi3_unconfigure();

	return 0;
}
int bk_spi3_slave_deinit(void)
{
	if (spi3_slave_dev == NULL)
		return 0;

	bk_spi3_unconfigure();

	if (spi3_slave_dev->mutex)
		rtos_lock_mutex(&spi3_slave_dev->mutex);

	if (spi3_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi3_slave_dev->tx_sem);

	if (spi3_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi3_slave_dev->rx_sem);

	if (spi3_slave_dev->rx_fifo)
		os_free(spi3_slave_dev->rx_fifo);

	if (spi3_slave_dev->mutex) {
		rtos_unlock_mutex(&spi3_slave_dev->mutex);
		rtos_deinit_mutex(&spi3_slave_dev->mutex);
	}

	os_free(spi3_slave_dev);
	spi3_slave_dev = NULL;

	return 0;
}

static void spi3_dma_tx_enable(UINT8 enable)
{
	int param;
	GDMA_CFG_ST en_cfg;

	//os_printf("dma enable\r\n");

	en_cfg.channel = SPI3_TX_DMA_CHANNEL;

	if (enable)
		en_cfg.param = 1;
	else
		en_cfg.param = 0;
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RX_EN, (void *)&param);

	//enable tx
	param = enable;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TX_EN, (void *)&param);
}

static void spi3_dma_rx_enable(UINT8 enable)
{
	int param ;
	GDMA_CFG_ST en_cfg;

	en_cfg.channel = SPI3_RX_DMA_CHANNEL;

	if (enable)
		en_cfg.param = 1;
	else
		en_cfg.param = 0;
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TX_EN, (void *)&param);

	//enable rx
	param = enable;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RX_EN, (void *)&param);
}

static void spi3_dma_tx_half_handler(UINT32 param)
{
	spi3_dma_trans_flag |= 1;

	os_printf("spi3_dma half handler\r\n");
}

static void spi3_dma_rx_half_handler(UINT32 param)
{
	spi3_dma_trans_flag |= 1;
	//os_printf("spi3_dma rx half hander\r\n");
}


static void  bk_spi3_dma_rx_finish_callback(UINT32 param)
{
	spi3_dma_trans_flag |= 2;
	int i = 0;
	rtos_set_semaphore(&spi3_dev->rx_sem);
	//spi3_dma_rx_enable(0);
	//os_printf("dma rx end:trans= %d\r\n",dma_trans_flag);
}

static void bk_spi3_dma_tx_finish_callback(void)
{
	spi3_dev->flag |= TX_FINISH_FLAG;
	rtos_set_semaphore(&spi3_dev->tx_sem);
	os_printf("dma tx end\r\n");
}

static int spi3_dma_master_tx_init(struct spi_message *spi3_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi3 dma tx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 8;
	init_cfg.srcdat_width = 32;
	init_cfg.dstptr_incr =  0;
	init_cfg.srcptr_incr =  1;

	init_cfg.src_start_addr = spi3_msg->send_buf;
	init_cfg.dst_start_addr = (void *)SPI3_DAT;

	init_cfg.channel = SPI3_TX_DMA_CHANNEL ;
	init_cfg.prio = 0;
	init_cfg.u.type4.src_loop_start_addr = spi3_msg->send_buf;
	init_cfg.u.type4.src_loop_end_addr = spi3_msg->send_buf + spi3_msg->send_len;

	init_cfg.half_fin_handler = spi3_dma_tx_half_handler;
	init_cfg.fin_handler = bk_spi3_dma_tx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_DTCM_RD_REQ;
	init_cfg.dst_module = GDMA_X_DST_GSPI3_TX_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE4, (void *)&init_cfg);

	en_cfg.channel = SPI3_TX_DMA_CHANNEL;
	en_cfg.param = spi3_msg->send_len;
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);

	en_cfg.channel = SPI3_TX_DMA_CHANNEL;
	en_cfg.param = 0;							// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}



static int spi3_dma_master_rx_init(struct spi_message *spi3_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi3 dma rx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 32;
	init_cfg.srcdat_width = 8;
	init_cfg.dstptr_incr =  1;
	init_cfg.srcptr_incr =  0;

	init_cfg.src_start_addr = (void *)SPI3_DAT;
	init_cfg.dst_start_addr = spi3_msg->recv_buf;

	init_cfg.channel = SPI3_RX_DMA_CHANNEL;
	init_cfg.prio = 0;
	init_cfg.u.type5.dst_loop_start_addr = spi3_msg->recv_buf;
	init_cfg.u.type5.dst_loop_end_addr = spi3_msg->recv_buf + spi3_msg->recv_len;

	init_cfg.half_fin_handler = spi3_dma_rx_half_handler;
	init_cfg.fin_handler = bk_spi3_dma_rx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_GSPI3_RX_REQ;
	init_cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, (void *)&init_cfg);

	en_cfg.channel = SPI3_RX_DMA_CHANNEL;
	en_cfg.param   = spi3_msg->recv_len;			// dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);


	en_cfg.channel = SPI3_RX_DMA_CHANNEL;
	en_cfg.param = 0;								// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}




static void bk_spi3_master_dma_config(UINT32 mode, UINT32 rate)
{
	UINT32 param;
	os_printf("spi3 master dma init: mode:%d, rate:%d\r\n", mode, rate);
	bk_spi3_master_configure(rate, mode);

	//disable tx/rx int disable
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TXINT_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXINT_EN, (void *)&param);

	//disable rx/tx finish enable bit
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TXFINISH_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXFINISH_EN, (void *)&param);

	//disable rx/tx over
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_NSSMD, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_BITWIDTH, (void *)&param);

	//disable CSN intterrupt
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_CS_EN, (void *)&param);

	//clk test
	//param = 5000000;
	//sddev_control(SPI3_DEV_NAME, CMD_SPI3_SET_CKR, (void *)&param);

	os_printf("spi3_master [CTRL]:0x%08x \n", REG_READ(SPI3_CTRL));
	os_printf("spi3_master [CONFIG]:0x%08x \n", REG_READ(SPI3_CONFIG));

}


int bk_spi3_master_dma_tx_init(UINT32 mode, UINT32 rate, struct spi_message *spi3_msg)
{
	OSStatus result = 0;

	if (spi3_dev)
		bk_spi3_master_deinit();

	spi3_dev = os_malloc(sizeof(struct bk_spi3_dev));
	if (!spi3_dev) {
		BK_SPI_PRT("[spi3]:malloc memory for spi3_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi3_dev, 0, sizeof(struct bk_spi3_dev));


	result = rtos_init_semaphore(&spi3_dev->tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi3]: spi3 tx semp init failed\n");
		goto _exit;
	}

	bk_spi3_master_dma_config(mode, rate);

	spi3_dma_master_tx_init(spi3_msg);

	return 0;

_exit:

	if (spi3_dev->tx_sem)
		rtos_deinit_semaphore(&spi3_dev->tx_sem);


	if (spi3_dev) {
		os_free(spi3_dev);
		spi3_dev = NULL;
	}

	return 1;
}

int bk_spi3_master_dma_rx_init(UINT32 mode, UINT32 rate, struct spi_message *spi3_msg)
{
	OSStatus result = 0;

	if (spi3_dev)
		bk_spi3_master_deinit();

	spi3_dev = os_malloc(sizeof(struct bk_spi3_dev));
	if (!spi3_dev) {
		BK_SPI_PRT("[spi3]:malloc memory for spi3_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi3_dev, 0, sizeof(struct bk_spi3_dev));


	result = rtos_init_semaphore(&spi3_dev->rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi3]: spi3 tx semp init failed\n");
		goto _exit;
	}

	bk_spi3_master_dma_config(mode, rate);

	spi3_dma_master_rx_init(spi3_msg);

	return 0;

_exit:

	if (spi3_dev->rx_sem)
		rtos_deinit_semaphore(&spi3_dev->rx_sem);


	if (spi3_dev) {
		os_free(spi3_dev);
		spi3_dev = NULL;
	}

	return 1;
}

int bk_spi3_master_dma_send(struct spi_message *spi3_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi3_msg != NULL);

	GLOBAL_INT_DISABLE();
	spi3_dev->flag &= ~(TX_FINISH_FLAG);
	GLOBAL_INT_RESTORE();

	spi3_dma_tx_enable(1);

	/* wait tx finish */
	rtos_get_semaphore(&spi3_dev->tx_sem, BEKEN_NEVER_TIMEOUT);

	spi3_dma_trans_flag = 0;

	if (spi3_msg->send_buf != NULL)
		return spi3_dma_trans_flag;
	else {
		os_printf("spi3_dma tx error send_buff\r\n", spi3_msg->send_buf);
		return 1;
	}
}


int bk_spi3_master_dma_recv(struct spi_message *spi3_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi3_msg != NULL);

	spi3_dma_rx_enable(1);

	rtos_get_semaphore(&spi3_dev->rx_sem, BEKEN_NEVER_TIMEOUT);

	spi3_dma_trans_flag = 0;

	os_printf("get rx semaphore\r\n");


	if (spi3_msg->recv_buf != NULL)
		return spi3_dma_trans_flag;
	else {
		os_printf("spi3_dma rx error recv_buff\r\n", spi3_msg->recv_buf);
		return 1;
	}
}
void bk_spi3_master_dma_disable(void)
{
	spi3_dma_rx_enable(0);
	spi3_dma_tx_enable(0);
}


static int spi3_dma_slave_tx_init(struct spi_message *spi3_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi3 dma tx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 8;
	init_cfg.srcdat_width = 32;
	init_cfg.dstptr_incr =  0;
	init_cfg.srcptr_incr =  1;

	init_cfg.src_start_addr = spi3_msg->send_buf;
	init_cfg.dst_start_addr = (void *)SPI3_DAT;

	init_cfg.channel = SPI3_TX_DMA_CHANNEL ;
	init_cfg.prio = 0;
	init_cfg.u.type4.src_loop_start_addr = spi3_msg->send_buf;
	init_cfg.u.type4.src_loop_end_addr = spi3_msg->send_buf + spi3_msg->send_len;

	init_cfg.half_fin_handler = spi3_dma_tx_half_handler;
	init_cfg.fin_handler = bk_spi3_dma_tx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_DTCM_RD_REQ;
	init_cfg.dst_module = GDMA_X_DST_GSPI3_TX_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE4, (void *)&init_cfg);

	en_cfg.channel = SPI3_TX_DMA_CHANNEL;
	en_cfg.param = spi3_msg->send_len;				// dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);

	en_cfg.channel = SPI3_TX_DMA_CHANNEL;
	en_cfg.param = 0;								// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}

static int spi3_dma_slave_rx_init(struct spi_message *spi3_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi3 dma rx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 32;
	init_cfg.srcdat_width = 8;
	init_cfg.dstptr_incr =  1;
	init_cfg.srcptr_incr =  0;

	init_cfg.src_start_addr = (void *)SPI3_DAT;
	init_cfg.dst_start_addr = spi3_msg->recv_buf;

	init_cfg.channel = SPI3_RX_DMA_CHANNEL;
	init_cfg.prio = 0;
	init_cfg.u.type5.dst_loop_start_addr = spi3_msg->recv_buf;
	init_cfg.u.type5.dst_loop_end_addr = spi3_msg->recv_buf + spi3_msg->recv_len;

	init_cfg.half_fin_handler = spi3_dma_rx_half_handler;
	init_cfg.fin_handler = bk_spi3_dma_rx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_GSPI3_RX_REQ;
	init_cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, (void *)&init_cfg);

	en_cfg.channel = SPI3_RX_DMA_CHANNEL;
	en_cfg.param   = spi3_msg->recv_len;			// dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);


	en_cfg.channel = SPI3_RX_DMA_CHANNEL;
	en_cfg.param = 1;								// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}

static void bk_spi3_slave_dma_config(UINT32 mode, UINT32 rate)
{
	UINT32 param;
	os_printf("spi3 slave dma init: mode:%d, rate;%d\r\n", mode, rate);
	bk_spi3_slave_configure(rate, mode);

	//disable tx/rx int disable
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TXINT_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXINT_EN, (void *)&param);

	//disable rx/tx finish enable bit
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TXFINISH_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXFINISH_EN, (void *)&param);

	//disable rx/tx over
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_RXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_TXOVR_EN, (void *)&param);

	//disable CSN intterrupt
	param = 0;
	sddev_control(SPI3_DEV_NAME, CMD_SPI3_CS_EN, (void *)&param);

	os_printf("spi3_slave [CTRL]:0x%08x \n", REG_READ(SPI3_CTRL));
	os_printf("spi3_slave [CONFIG]:0x%08x \n", REG_READ(SPI3_CONFIG));
}

int bk_spi3_slave_dma_rx_init(UINT32 mode, UINT32 rate, struct spi_message *spi3_msg)
{
	OSStatus result = 0;

	if (spi3_slave_dev)
		bk_spi3_slave_deinit();

	spi3_slave_dev = os_malloc(sizeof(struct bk_spi3_slave_dev));
	if (!spi3_slave_dev)
	{
		BK_SPI_PRT("[spi3]:malloc memory for spi3_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi3_slave_dev, 0, sizeof(struct bk_spi3_slave_dev));

	result = rtos_init_semaphore(&spi3_slave_dev->rx_sem, 1);
	if (result != kNoErr)
	{
		BK_SPI_PRT("[spi3]: spi3 rx semp init failed\n");
		goto _exit;
	}

	bk_spi3_slave_dma_config(mode, rate);
	spi3_dma_slave_rx_init(spi3_msg);

	return 0;

_exit:

	if (spi3_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi3_slave_dev->rx_sem);

	if (spi3_slave_dev)
	{
		os_free(spi3_slave_dev);
		spi3_slave_dev = NULL;
	}

	return 1;
}



int bk_spi3_slave_dma_tx_init(UINT32 mode, UINT32 rate, struct spi_message *spi3_msg)
{
	OSStatus result = 0;

	if (spi3_slave_dev)
		bk_spi3_slave_deinit();

	spi3_slave_dev = os_malloc(sizeof(struct bk_spi3_slave_dev));
	if (!spi3_slave_dev)
	{
		BK_SPI_PRT("[spi3]:malloc memory for spi3_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi3_slave_dev, 0, sizeof(struct bk_spi3_slave_dev));

	result = rtos_init_semaphore(&spi3_slave_dev->tx_sem, 1);
	if (result != kNoErr)
	{
		BK_SPI_PRT("[spi3]: spi3 tx semp init failed\n");
		goto _exit;
	}

	bk_spi3_slave_dma_config(mode, rate);
	spi3_dma_slave_tx_init(spi3_msg);

	return 0;

_exit:

	if (spi3_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi3_slave_dev->tx_sem);

	if (spi3_slave_dev)
	{
		os_free(spi3_slave_dev);
		spi3_slave_dev = NULL;
	}

	return 1;
}






int bk_spi3_slave_dma_send(struct spi_message *spi3_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi3_msg != NULL);

	GLOBAL_INT_DISABLE();
	spi3_slave_dev->flag &= ~(TX_FINISH_FLAG);
	GLOBAL_INT_RESTORE();

	spi3_dma_tx_enable(1);

	rtos_get_semaphore(&spi3_slave_dev->tx_sem, BEKEN_NEVER_TIMEOUT);

	spi3_dma_trans_flag = 0;

	if (spi3_msg->send_buf != NULL)
		return spi3_dma_trans_flag;
	else {
		os_printf("spi3_dma tx error send_buff\r\n", spi3_msg->send_buf);
		return 1;
	}
}


int bk_spi3_slave_dma_recv(struct spi_message *spi3_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi3_msg != NULL);

	spi3_dma_rx_enable(1);

	rtos_get_semaphore(&spi3_slave_dev->rx_sem, BEKEN_WAIT_FOREVER);

	spi3_dma_trans_flag = 0;

	os_printf("get rx semaphorer\n");

	if (spi3_msg->recv_buf != NULL)
		return spi3_dma_trans_flag;
	else {
		os_printf("spi3_dma rx error recv_buff\r\n", spi3_msg->recv_buf);
		return 1;
	}
}
void bk_spi3_slave_dma_disable(void)
{
	spi3_dma_rx_enable(0);
	spi3_dma_tx_enable(0);
}

#endif  // CFG_USE_SPI3_SLAVE
#endif  // iifdef SOC_BK7271


