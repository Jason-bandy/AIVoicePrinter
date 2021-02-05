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
#if CFG_USE_SPI2_DMA

struct bk_spi2_dev {
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

struct bk_spi2_slave_dev {
	UINT32 flag;

	beken_semaphore_t tx_sem;
	UINT8 *tx_ptr;
	UINT32 tx_len;

	beken_semaphore_t rx_sem;
	struct spi_rx_fifo *rx_fifo;

	beken_mutex_t mutex;
};

static struct bk_spi2_dev *spi2_dev;
static struct bk_spi2_slave_dev *spi2_slave_dev;

int spi2_dma_trans_flag = 0 ;

#define SPI2_TEST_POART1        0
#define SPI2_TEST_POART2        1
#define SPI2_TX_BUFFER_SIZE     1024
#define SPI2_RX_BUFFER_SIZE     1024*2
#define SPI2_RX_DMA_CHANNEL     GDMA_CHANNEL_1
#define SPI2_TX_DMA_CHANNEL     GDMA_CHANNEL_3

static void spi2_debug_prt(void)
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

static void bk_spi2_master_configure(UINT32 rate, UINT32 mode)
{
	UINT32 param;

	/* data bit width */
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_BITWIDTH, (void *)&param);

	/* baudrate */
	BK_SPI_PRT("max_hz = %d \n", rate);
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_CKR, (void *)&rate);

	/* mode */
	if (mode & BK_SPI_CPOL)
		param = 1;
	else
		param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_CKPOL, (void *)&param);

	/* CPHA */
	if (mode & BK_SPI_CPHA)
		param = 1;
	else
		param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_CKPHA, (void *)&param);

	/* Master */
	param = 1;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_MSTEN, (void *)&param);
	param = 3;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_NSSMD, (void *)&param);
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_INIT_MSTEN, (void *)&param);

	/* enable spi2 */
	param = 1;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_UNIT_ENABLE, (void *)&param);

	BK_SPI_PRT("spi2_master:[CTRL]:0x%08x \n", REG_READ(SPI2_CTRL));
}

static void bk_spi2_slave_configure(UINT32 rate, UINT32 mode)
{
	UINT32 param;

	/* data bit width */
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_BITWIDTH, (void *)&param);

	/* baudrate */
	BK_SPI_PRT("max_hz = %d \n", rate);
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_CKR, (void *)&rate);

	/* mode */
	if (mode & BK_SPI_CPOL)
		param = 1;
	else
		param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_CKPOL, (void *)&param);

	/* CPHA */
	if (mode & BK_SPI_CPHA)
		param = 1;
	else
		param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_CKPHA, (void *)&param);

	/* slave */
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_MSTEN, (void *)&param);

	// 4line :7271 NSSMD is 0
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_NSSMD, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_INIT_MSTEN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_LSB_EN, (void *)&param);

	//enable rx/tx finish enable bit
	param = 1;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TXFINISH_EN, (void *)&param);

	param = 1;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXFINISH_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXINT_EN, (void *)&param);

	/* enable spi2 */
	param = 1;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_UNIT_ENABLE, (void *)&param);

	BK_SPI_PRT("spi2_slave [CONFIG]:0x%08x \n", REG_READ(SPI2_CONFIG));

}

static void bk_spi2_unconfigure(void)
{
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_DEINIT_MSTEN, NULL);
}

int bk_spi2_master_deinit(void)
{
	if (spi2_dev == NULL)
		return 0;

	if (spi2_dev->mutex)
		rtos_lock_mutex(&spi2_dev->mutex);

	if (spi2_dev->tx_sem)
		rtos_deinit_semaphore(&spi2_dev->tx_sem);

	if (spi2_dev->mutex) {
		rtos_unlock_mutex(&spi2_dev->mutex);
		rtos_deinit_mutex(&spi2_dev->mutex);
	}

	if (spi2_dev) {
		os_free(spi2_dev);
		spi2_dev = NULL;
	}

	bk_spi2_unconfigure();

	return 0;
}

int bk_spi2_slave_deinit(void)
{
	if (spi2_slave_dev == NULL)
		return 0;

	bk_spi2_unconfigure();

	if (spi2_slave_dev->mutex)
		rtos_lock_mutex(&spi2_slave_dev->mutex);

	if (spi2_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi2_slave_dev->tx_sem);

	if (spi2_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi2_slave_dev->rx_sem);

	if (spi2_slave_dev->rx_fifo)
		os_free(spi2_slave_dev->rx_fifo);

	if (spi2_slave_dev->mutex) {
		rtos_unlock_mutex(&spi2_slave_dev->mutex);
		rtos_deinit_mutex(&spi2_slave_dev->mutex);
	}

	os_free(spi2_slave_dev);
	spi2_slave_dev = NULL;

	return 0;
}

static void spi2_dma_tx_enable(UINT8 enable)
{
	int param;
	GDMA_CFG_ST en_cfg;

	//os_printf("dma enable\r\n");

	en_cfg.channel = SPI2_TX_DMA_CHANNEL;

	if (enable)
		en_cfg.param = 1;
	else
		en_cfg.param = 0;
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RX_EN, (void *)&param);

	//enable tx
	param = enable;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TX_EN, (void *)&param);
}

static void spi2_dma_rx_enable(UINT8 enable)
{
	int param ;
	GDMA_CFG_ST en_cfg;

	en_cfg.channel = SPI2_RX_DMA_CHANNEL;

	if (enable)
		en_cfg.param = 1;
	else
		en_cfg.param = 0;
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TX_EN, (void *)&param);

	//enable rx
	param = enable;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RX_EN, (void *)&param);
}

static void spi2_dma_tx_half_handler(UINT32 param)
{
	spi2_dma_trans_flag |= 1;

	os_printf("spi2_dma half handler\r\n");
}

static void spi2_dma_rx_half_handler(UINT32 param)
{
	spi2_dma_trans_flag |= 1;
	//os_printf("spi2_dma rx half hander\r\n");
}


static void  bk_spi2_dma_rx_finish_callback(UINT32 param)
{
	spi2_dma_trans_flag |= 2;
	int i = 0;
	rtos_set_semaphore(&spi2_dev->rx_sem);
	//spi2_dma_rx_enable(0);
	//os_printf("dma rx end:trans= %d\r\n",dma_trans_flag);
}

static void bk_spi2_dma_tx_finish_callback(void)
{
	spi2_dev->flag |= TX_FINISH_FLAG;
	rtos_set_semaphore(&spi2_dev->tx_sem);
	os_printf("dma tx end\r\n");
}

static int spi2_dma_master_tx_init(struct spi_message *spi2_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi2 dma tx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 8;
	init_cfg.srcdat_width = 32;
	init_cfg.dstptr_incr =  0;
	init_cfg.srcptr_incr =  1;

	init_cfg.src_start_addr = spi2_msg->send_buf;
	init_cfg.dst_start_addr = (void *)SPI2_DAT;

	init_cfg.channel = SPI2_TX_DMA_CHANNEL ;
	init_cfg.prio = 0;
	init_cfg.u.type4.src_loop_start_addr = spi2_msg->send_buf;
	init_cfg.u.type4.src_loop_end_addr = spi2_msg->send_buf + spi2_msg->send_len;

	init_cfg.half_fin_handler = spi2_dma_tx_half_handler;
	init_cfg.fin_handler = bk_spi2_dma_tx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_DTCM_RD_REQ;
	init_cfg.dst_module = GDMA_X_DST_GSPI2_TX_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE4, (void *)&init_cfg);

	en_cfg.channel = SPI2_TX_DMA_CHANNEL;
	en_cfg.param = spi2_msg->send_len;      // dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);

	en_cfg.channel = SPI2_TX_DMA_CHANNEL;
	en_cfg.param = 0;                           // 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}

static int spi2_dma_master_rx_init(struct spi_message *spi2_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi2 dma rx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 32;
	init_cfg.srcdat_width = 8;
	init_cfg.dstptr_incr =  1;
	init_cfg.srcptr_incr =  0;

	init_cfg.src_start_addr = (void *)SPI2_DAT;
	init_cfg.dst_start_addr = spi2_msg->recv_buf;

	init_cfg.channel = SPI2_RX_DMA_CHANNEL;
	init_cfg.prio = 0;
	init_cfg.u.type5.dst_loop_start_addr = spi2_msg->recv_buf;
	init_cfg.u.type5.dst_loop_end_addr = spi2_msg->recv_buf + spi2_msg->recv_len;

	init_cfg.half_fin_handler = spi2_dma_rx_half_handler;
	init_cfg.fin_handler = bk_spi2_dma_rx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_GSPI2_RX_REQ;
	init_cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, (void *)&init_cfg);

	en_cfg.channel = SPI2_RX_DMA_CHANNEL;
	en_cfg.param   = spi2_msg->recv_len;        // dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);


	en_cfg.channel = SPI2_RX_DMA_CHANNEL;
	en_cfg.param = 0;                           // 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}


static void bk_spi2_master_dma_config(UINT32 mode, UINT32 rate)
{
	UINT32 param;
	os_printf("spi2 master dma init: mode:%d, rate:%d\r\n", mode, rate);
	bk_spi2_master_configure(rate, mode);

	//disable tx/rx int disable
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TXINT_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXINT_EN, (void *)&param);

	//disable rx/tx finish enable bit
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TXFINISH_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXFINISH_EN, (void *)&param);

	//disable rx/tx over
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_NSSMD, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_BITWIDTH, (void *)&param);

	//disable CSN intterrupt
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_CS_EN, (void *)&param);

	//clk test
	//param = 5000000;
	//sddev_control(SPI2_DEV_NAME, CMD_SPI2_SET_CKR, (void *)&param);

	os_printf("spi2_master [CTRL]:0x%08x \n", REG_READ(SPI2_CTRL));
	os_printf("spi2_master [CONFIG]:0x%08x \n", REG_READ(SPI2_CONFIG));

}


int bk_spi2_master_dma_tx_init(UINT32 mode, UINT32 rate, struct spi_message *spi2_msg)
{
	OSStatus result = 0;

	if (spi2_dev)
		bk_spi2_master_deinit();

	spi2_dev = os_malloc(sizeof(struct bk_spi2_dev));
	if (!spi2_dev) {
		BK_SPI_PRT("[spi2]:malloc memory for spi2_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi2_dev, 0, sizeof(struct bk_spi2_dev));


	result = rtos_init_semaphore(&spi2_dev->tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi2]: spi2 tx semp init failed\n");
		goto _exit;
	}

	bk_spi2_master_dma_config(mode, rate);

	spi2_dma_master_tx_init(spi2_msg);

	return 0;

_exit:

	if (spi2_dev->tx_sem)
		rtos_deinit_semaphore(&spi2_dev->tx_sem);


	if (spi2_dev) {
		os_free(spi2_dev);
		spi2_dev = NULL;
	}

	return 1;
}

int bk_spi2_master_dma_rx_init(UINT32 mode, UINT32 rate, struct spi_message *spi2_msg)
{
	OSStatus result = 0;

	if (spi2_dev)
		bk_spi2_master_deinit();

	spi2_dev = os_malloc(sizeof(struct bk_spi2_dev));
	if (!spi2_dev) {
		BK_SPI_PRT("[spi2]:malloc memory for spi2_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi2_dev, 0, sizeof(struct bk_spi2_dev));


	result = rtos_init_semaphore(&spi2_dev->rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi2]: spi2 tx semp init failed\n");
		goto _exit;
	}

	bk_spi2_master_dma_config(mode, rate);

	spi2_dma_master_rx_init(spi2_msg);

	return 0;

_exit:

	if (spi2_dev->rx_sem)
		rtos_deinit_semaphore(&spi2_dev->rx_sem);


	if (spi2_dev) {
		os_free(spi2_dev);
		spi2_dev = NULL;
	}

	return 1;
}

int bk_spi2_master_dma_send(struct spi_message *spi2_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi2_msg != NULL);

	GLOBAL_INT_DISABLE();
	spi2_dev->flag &= ~(TX_FINISH_FLAG);
	GLOBAL_INT_RESTORE();

	spi2_dma_tx_enable(1);

	/* wait tx finish */
	rtos_get_semaphore(&spi2_dev->tx_sem, BEKEN_NEVER_TIMEOUT);

	spi2_dma_trans_flag = 0;

	if (spi2_msg->send_buf != NULL)
		return spi2_dma_trans_flag;
	else {
		os_printf("spi2_dma tx error send_buff\r\n", spi2_msg->send_buf);
		return 1;
	}
}


int bk_spi2_master_dma_recv(struct spi_message *spi2_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi2_msg != NULL);

	spi2_dma_rx_enable(1);

	rtos_get_semaphore(&spi2_dev->rx_sem, BEKEN_NEVER_TIMEOUT);

	spi2_dma_trans_flag = 0;

	os_printf("get rx semaphore\r\n");


	if (spi2_msg->recv_buf != NULL)
		return spi2_dma_trans_flag;
	else {
		os_printf("spi2_dma rx error recv_buff\r\n", spi2_msg->recv_buf);
		return 1;
	}
}
void bk_spi2_master_dma_disable(void)
{
	spi2_dma_rx_enable(0);
	spi2_dma_tx_enable(0);
}


static int spi2_dma_slave_tx_init(struct spi_message *spi2_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi2 dma tx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 8;
	init_cfg.srcdat_width = 32;
	init_cfg.dstptr_incr =  0;
	init_cfg.srcptr_incr =  1;

	init_cfg.src_start_addr = spi2_msg->send_buf;
	init_cfg.dst_start_addr = (void *)SPI2_DAT;

	init_cfg.channel = SPI2_TX_DMA_CHANNEL ;
	init_cfg.prio = 0;
	init_cfg.u.type4.src_loop_start_addr = spi2_msg->send_buf;
	init_cfg.u.type4.src_loop_end_addr = spi2_msg->send_buf + spi2_msg->send_len;

	init_cfg.half_fin_handler = spi2_dma_tx_half_handler;
	init_cfg.fin_handler = bk_spi2_dma_tx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_DTCM_RD_REQ;
	init_cfg.dst_module = GDMA_X_DST_GSPI2_TX_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE4, (void *)&init_cfg);

	en_cfg.channel = SPI2_TX_DMA_CHANNEL;
	en_cfg.param = spi2_msg->send_len;      // dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);

	en_cfg.channel = SPI2_TX_DMA_CHANNEL;
	en_cfg.param = 0;                           // 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}

static int spi2_dma_slave_rx_init(struct spi_message *spi2_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;
	int status;
	int reg_addr;

	os_printf("spi2 dma rx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 32;
	init_cfg.srcdat_width = 8;
	init_cfg.dstptr_incr =  1;
	init_cfg.srcptr_incr =  0;

	init_cfg.src_start_addr = (void *)SPI2_DAT;
	init_cfg.dst_start_addr = spi2_msg->recv_buf;

	init_cfg.channel = SPI2_RX_DMA_CHANNEL;
	init_cfg.prio = 0;
	init_cfg.u.type5.dst_loop_start_addr = spi2_msg->recv_buf;
	init_cfg.u.type5.dst_loop_end_addr = spi2_msg->recv_buf + spi2_msg->recv_len;

	init_cfg.half_fin_handler = spi2_dma_rx_half_handler;
	init_cfg.fin_handler = bk_spi2_dma_rx_finish_callback;

	init_cfg.src_module = GDMA_X_SRC_GSPI2_RX_REQ;
	init_cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, (void *)&init_cfg);

	en_cfg.channel = SPI2_RX_DMA_CHANNEL;
	en_cfg.param   = spi2_msg->recv_len;        // dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);


	en_cfg.channel = SPI2_RX_DMA_CHANNEL;
	en_cfg.param = 1;                           // 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	return 0;
}

static void bk_spi2_slave_dma_config(UINT32 mode, UINT32 rate)
{
	UINT32 param;
	os_printf("spi2 slave dma init: mode:%d, rate;%d\r\n", mode, rate);
	bk_spi2_slave_configure(rate, mode);

	//disable tx/rx int disable
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TXINT_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXINT_EN, (void *)&param);

	//disable rx/tx finish enable bit
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TXFINISH_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXFINISH_EN, (void *)&param);

	//disable rx/tx over
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_RXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_TXOVR_EN, (void *)&param);

	//disable CSN intterrupt
	param = 0;
	sddev_control(SPI2_DEV_NAME, CMD_SPI2_CS_EN, (void *)&param);

	os_printf("spi2_slave [CTRL]:0x%08x \n", REG_READ(SPI2_CTRL));
	os_printf("spi2_slave [CONFIG]:0x%08x \n", REG_READ(SPI2_CONFIG));
}

int bk_spi2_slave_dma_rx_init(UINT32 mode, UINT32 rate, struct spi_message *spi2_msg)
{
	OSStatus result = 0;

	if (spi2_slave_dev)
		bk_spi2_slave_deinit();

	spi2_slave_dev = os_malloc(sizeof(struct bk_spi2_slave_dev));
	if (!spi2_slave_dev) {
		BK_SPI_PRT("[spi2]:malloc memory for spi2_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi2_slave_dev, 0, sizeof(struct bk_spi2_slave_dev));

	result = rtos_init_semaphore(&spi2_slave_dev->rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi2]: spi2 rx semp init failed\n");
		goto _exit;
	}

	bk_spi2_slave_dma_config(mode, rate);
	spi2_dma_slave_rx_init(spi2_msg);

	return 0;

_exit:

	if (spi2_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi2_slave_dev->rx_sem);

	if (spi2_slave_dev) {
		os_free(spi2_slave_dev);
		spi2_slave_dev = NULL;
	}

	return 1;
}

int bk_spi2_slave_dma_tx_init(UINT32 mode, UINT32 rate, struct spi_message *spi2_msg)
{
	OSStatus result = 0;

	if (spi2_slave_dev)
		bk_spi_slave_deinit();

	spi2_slave_dev = os_malloc(sizeof(struct bk_spi2_slave_dev));
	if (!spi2_slave_dev) {
		BK_SPI_PRT("[spi2]:malloc memory for spi2_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi2_slave_dev, 0, sizeof(struct bk_spi2_slave_dev));

	result = rtos_init_semaphore(&spi2_slave_dev->tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi2]: spi2 tx semp init failed\n");
		goto _exit;
	}

	bk_spi2_slave_dma_config(mode, rate);
	spi2_dma_slave_tx_init(spi2_msg);

	return 0;

_exit:

	if (spi2_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi2_slave_dev->tx_sem);

	if (spi2_slave_dev) {
		os_free(spi2_slave_dev);
		spi2_slave_dev = NULL;
	}

	return 1;
}


int bk_spi2_slave_dma_send(struct spi_message *spi2_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi2_msg != NULL);

	GLOBAL_INT_DISABLE();
	spi2_slave_dev->flag &= ~(TX_FINISH_FLAG);
	GLOBAL_INT_RESTORE();

	spi2_dma_tx_enable(1);

	rtos_get_semaphore(&spi2_slave_dev->tx_sem, BEKEN_NEVER_TIMEOUT);

	spi2_dma_trans_flag = 0;

	if (spi2_msg->send_buf != NULL)
		return spi2_dma_trans_flag;
	else {
		os_printf("spi2_dma tx error send_buff\r\n", spi2_msg->send_buf);
		return 1;
	}
}


int bk_spi2_slave_dma_recv(struct spi_message *spi2_msg)
{
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi2_msg != NULL);

	spi2_dma_rx_enable(1);

	rtos_get_semaphore(&spi2_slave_dev->rx_sem, BEKEN_WAIT_FOREVER);

	spi2_dma_trans_flag = 0;

	os_printf("get rx semaphorer\n");

	if (spi2_msg->recv_buf != NULL)
		return spi2_dma_trans_flag;
	else {
		os_printf("spi2_dma rx error recv_buff\r\n", spi2_msg->recv_buf);
		return 1;
	}
}
void bk_spi2_slave_dma_disable(void)
{
	spi2_dma_rx_enable(0);
	spi2_dma_tx_enable(0);
}

#endif  // CFG_USE_SPI2_SLAVE
#endif  // iifdef SOC_BK7271

