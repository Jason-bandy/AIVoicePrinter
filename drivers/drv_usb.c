#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "include.h"
#include "interrupt.h"
#include "typedef.h"

#if CFG_USB
#include "usb_pub.h"

static struct rt_device usb_device;
static struct rt_mutex usb_mutex;

static rt_err_t rt_usb_init(rt_device_t dev)
{
	rt_mutex_init(&usb_mutex, "usb", RT_IPC_FLAG_PRIO);
	return RT_EOK;
}

static rt_err_t rt_usb_open(rt_device_t dev, rt_uint16_t oflag)
{
	rt_mutex_take(&usb_mutex, RT_WAITING_FOREVER);
	rt_mutex_release(&usb_mutex);
	return RT_EOK;
}

static rt_err_t rt_usb_close(rt_device_t dev)
{
	rt_err_t ret;

	rt_mutex_take(&usb_mutex, RT_WAITING_FOREVER);
	ret = usb_close();
	rt_mutex_release(&usb_mutex);

	return ret;
}

static rt_size_t rt_usb_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
	uint32_t start_blk_addr;
	uint8_t read_blk_num;
	uint8_t *read_data_buf;

	rt_mutex_take(&usb_mutex, RT_WAITING_FOREVER);
	start_blk_addr = pos;
	read_blk_num = size;
	read_data_buf = (uint8_t *)buffer;

	if (RT_EOK != MUSB_HfiRead_sync(start_blk_addr, read_blk_num, read_data_buf)) {
		size = 0;
	}
	rt_mutex_release(&usb_mutex);

	return size;
}

static rt_size_t rt_usb_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
	uint32_t start_blk_addr;
	uint8_t write_blk_num;
	uint8_t *write_data_buf;

	rt_mutex_take(&usb_mutex, RT_WAITING_FOREVER);
	start_blk_addr = pos;
	write_blk_num = size;
	write_data_buf = (uint8_t *)buffer;

	if (RT_EOK != MUSB_HfiWrite_sync(start_blk_addr, write_blk_num, write_data_buf)) {
		size = 0;
	}
	rt_mutex_release(&usb_mutex);

	return size;
}

static rt_err_t rt_usb_control(rt_device_t dev, int cmd, void *args)
{
	struct rt_device_blk_geometry *geometry;

	RT_ASSERT(dev != RT_NULL);

	switch (cmd) {
		case RT_DEVICE_CTRL_BLK_GETGEOME:
			geometry = (struct rt_device_blk_geometry *)args;
			if (geometry == RT_NULL) {
				return -RT_ERROR;
			}
			rt_kprintf("---blksize:%x,totalcnt:%x---\n", get_HfiMedium_blksize(), get_HfiMedium_size());
			geometry->block_size = get_HfiMedium_blksize();
			geometry->sector_count = get_HfiMedium_size();
			geometry->bytes_per_sector = get_HfiMedium_blksize();
			break;
		default:
			rt_kprintf("unknow cmd %x.\n", cmd);
			break;
	}

	return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops usb_ops = {
	rt_usb_init,
	rt_usb_open,
	rt_usb_close,
	rt_usb_read,
	rt_usb_write,
	rt_usb_control
};
#endif

int rt_hw_usb_init(void)
{
	usb_device.type = RT_Device_Class_Block;
#ifdef RT_USING_DEVICE_OPS
	usb_device.ops = &usb_ops;
#else
	usb_device.init = rt_usb_init;
	usb_device.open = rt_usb_open;
	usb_device.close = rt_usb_close;
	usb_device.read = rt_usb_read;
	usb_device.write = rt_usb_write;
	usb_device.control = rt_usb_control;
#endif
	usb_device.user_data = RT_NULL;

	rt_device_register(&usb_device, "usb0",
						RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE | RT_DEVICE_FLAG_STANDALONE);

	return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_usb_init);
#endif
