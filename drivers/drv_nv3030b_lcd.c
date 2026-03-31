/*
 * NV3030B-03 240x320 LCD 驱动程序
 * 硬件接口：SPI (4 线)
 * 适用：BK7252N + TF20QN003-10 屏幕
 * 
 * 杜邦线连接 (10 线)：
 * - VCC: 3.3V
 * - GND: GND
 * - CLK: GPIO7 (SPI CLK)
 * - MOSI: GPIO8 (SPI MOSI)
 * - CS: GPIO6
 * - DC: GPIO9
 * - RST: GPIO10
 * - BLK: GPIO11 (背光控制)
 * - MISO: (可选，读操作使用)
 * - NC: 备用
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdio.h>

#include "spi_pub.h"
#include "gpio_pub.h"
#include "sys_config.h"

/* ========================= 硬件配置 ========================= */

/* SPI 设备名称 */
#define NV3030B_SPI_DEVICE    "spi0"

/* GPIO 定义（根据实际杜邦线连接调整）
 * BK7252N 引脚定义：
 * - SPI CLK: GPIO7
 * - SPI MOSI: GPIO8
 * - CS: GPIO6
 * - DC: GPIO9
 * - RST: GPIO10
 * - BLK: GPIO11 (背光)
 */
#define LCD_CS_PIN           GPIO6
#define LCD_CLK_PIN          GPIO7
#define LCD_MOSI_PIN         GPIO8
#define LCD_DC_PIN           GPIO9
#define LCD_RST_PIN          GPIO10
#define LCD_BLK_PIN          GPIO11

/* 屏幕参数 - 根据 TF20QN003-10 规格书 */
#define LCD_WIDTH            240
#define LCD_HEIGHT           320

/* 颜色定义（RGB565 格式） */
#define WHITE                0xFFFF
#define BLACK                0x0000
#define BLUE                 0x001F
#define RED                  0xF800
#define GREEN                0x07E0
#define CYAN                 0x07FF
#define MAGENTA              0xF81F
#define YELLOW               0xFFE0
#define GRAY                 0x8410
#define LIGHT_GRAY           0xC618

/* ========================= 全局变量 ========================= */

static struct rt_spi_device *lcd_spi_dev = RT_NULL;
static rt_bool_t lcd_initialized = RT_FALSE;

/* ========================= GPIO 控制 ========================= */

static void lcd_gpio_init(void)
{
    /* 初始化 CS (片选) */
    gpio_set_pin_function(LCD_CS_PIN, GPIO_FUNC_GPIO);
    gpio_set_pin_direction(LCD_CS_PIN, GPIO_OUTPUT);
    gpio_set_pin_value(LCD_CS_PIN, GPIO_HIGH);  /* 默认不选中 */

    /* 初始化 DC (数据/命令选择) */
    gpio_set_pin_function(LCD_DC_PIN, GPIO_FUNC_GPIO);
    gpio_set_pin_direction(LCD_DC_PIN, GPIO_OUTPUT);
    gpio_set_pin_value(LCD_DC_PIN, GPIO_LOW);

    /* 初始化 RST (复位) */
    gpio_set_pin_function(LCD_RST_PIN, GPIO_FUNC_GPIO);
    gpio_set_pin_direction(LCD_RST_PIN, GPIO_OUTPUT);
    gpio_set_pin_value(LCD_RST_PIN, GPIO_HIGH);

    /* 初始化 BLK (背光控制) */
    gpio_set_pin_function(LCD_BLK_PIN, GPIO_FUNC_GPIO);
    gpio_set_pin_direction(LCD_BLK_PIN, GPIO_OUTPUT);
    gpio_set_pin_value(LCD_BLK_PIN, GPIO_LOW);  /* 默认关闭背光 */
}

static void lcd_cs_select(void)
{
    gpio_set_pin_value(LCD_CS_PIN, GPIO_LOW);
}

static void lcd_cs_deselect(void)
{
    gpio_set_pin_value(LCD_CS_PIN, GPIO_HIGH);
}

static void lcd_dc_command(void)
{
    gpio_set_pin_value(LCD_DC_PIN, GPIO_LOW);
}

static void lcd_dc_data(void)
{
    gpio_set_pin_value(LCD_DC_PIN, GPIO_HIGH);
}

static void lcd_reset(void)
{
    /* 拉低复位 */
    gpio_set_pin_value(LCD_RST_PIN, GPIO_LOW);
    rt_thread_mdelay(100);
    
    /* 释放复位 */
    gpio_set_pin_value(LCD_RST_PIN, GPIO_HIGH);
    rt_thread_mdelay(120);
}

static void lcd_backlight_on(void)
{
    gpio_set_pin_value(LCD_BLK_PIN, GPIO_HIGH);
}

static void lcd_backlight_off(void)
{
    gpio_set_pin_value(LCD_BLK_PIN, GPIO_LOW);
}

/* ========================= SPI 通信 ========================= */

/* 发送命令 */
static void lcd_write_cmd(rt_uint8_t cmd)
{
    lcd_dc_command();
    lcd_cs_select();
    
    rt_spi_write(lcd_spi_dev, &cmd, 1);
    
    lcd_cs_deselect();
}

/* 发送数据 */
static void lcd_write_data(rt_uint8_t data)
{
    lcd_dc_data();
    lcd_cs_select();
    
    rt_spi_write(lcd_spi_dev, &data, 1);
    
    lcd_cs_deselect();
}

/* 发送多个数据 */
static void lcd_write_data_multi(const rt_uint8_t *data, rt_size_t len)
{
    lcd_dc_data();
    lcd_cs_select();
    
    rt_spi_write(lcd_spi_dev, data, len);
    
    lcd_cs_deselect();
}

/* 发送 16 位数据 */
static void lcd_write_data16(rt_uint16_t data)
{
    rt_uint8_t buf[2];
    buf[0] = (data >> 8) & 0xFF;
    buf[1] = data & 0xFF;
    
    lcd_write_data_multi(buf, 2);
}

/* ========================= LCD 基础操作 ========================= */

/* 设置光标位置（窗口） */
static void lcd_set_window(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
    /* 列地址设置 (0x2A) */
    lcd_write_cmd(0x2A);
    lcd_write_data16(x1);
    lcd_write_data16(x2);
    
    /* 行地址设置 (0x2B) */
    lcd_write_cmd(0x2B);
    lcd_write_data16(y1);
    lcd_write_data16(y2);
    
    /* 内存写 (0x2C) */
    lcd_write_cmd(0x2C);
}

/* 清屏 */
void lcd_clear(rt_uint16_t color)
{
    rt_uint32_t i;
    rt_uint8_t high = color >> 8;
    rt_uint8_t low = color & 0xFF;
    
    lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    lcd_dc_data();
    lcd_cs_select();
    
    /* 优化：批量发送像素数据 */
    rt_uint8_t pixel_buf[32];  /* 16 个像素的缓冲区 */
    for (i = 0; i < 16; i++) {
        pixel_buf[i*2] = high;
        pixel_buf[i*2+1] = low;
    }
    
    rt_uint32_t total = LCD_WIDTH * LCD_HEIGHT;
    for (i = 0; i < total / 16; i++) {
        rt_spi_write(lcd_spi_dev, pixel_buf, 32);
    }
    
    /* 处理剩余像素 */
    rt_uint8_t remain = total % 16;
    if (remain > 0) {
        rt_spi_write(lcd_spi_dev, pixel_buf, remain * 2);
    }
    
    lcd_cs_deselect();
}

/* 画点 */
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return;
    }
    
    lcd_set_window(x, y, x, y);
    lcd_write_data16(color);
}

/* 画线 */
void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color)
{
    rt_int16_t dx, dy;
    rt_int16_t sx, sy;
    rt_int16_t err;
    rt_int16_t e2;
    
    dx = x2 - x1;
    dy = y2 - y1;
    sx = (dx > 0) ? 1 : -1;
    sy = (dy > 0) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;
    
    while (1) {
        lcd_draw_point(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) {
            break;
        }
        
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
}

/* 画矩形 */
void lcd_draw_rectangle(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, rt_uint16_t color)
{
    lcd_draw_line(x, y, x + width, y, color);
    lcd_draw_line(x, y + height, x + width, y + height, color);
    lcd_draw_line(x, y, x, y + height, color);
    lcd_draw_line(x + width, y, x + width, y + height, color);
}

/* 填充矩形 */
void lcd_fill_rectangle(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, rt_uint16_t color)
{
    rt_uint32_t i;
    rt_uint16_t total = width * height;
    
    lcd_set_window(x, y, x + width - 1, y + height - 1);
    lcd_dc_data();
    lcd_cs_select();
    
    rt_uint8_t high = color >> 8;
    rt_uint8_t low = color & 0xFF;
    
    for (i = 0; i < total; i++) {
        rt_spi_write(lcd_spi_dev, &high, 1);
        rt_spi_write(lcd_spi_dev, &low, 1);
    }
    
    lcd_cs_deselect();
}

/* 画圆 */
void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint16_t r, rt_uint16_t color)
{
    rt_int16_t x = 0, y = r;
    rt_int16_t d = 3 - 2 * r;
    
    while (x <= y) {
        lcd_draw_point(x0 + x, y0 + y, color);
        lcd_draw_point(x0 - x, y0 + y, color);
        lcd_draw_point(x0 + x, y0 - y, color);
        lcd_draw_point(x0 - x, y0 - y, color);
        lcd_draw_point(x0 + y, y0 + x, color);
        lcd_draw_point(x0 - y, y0 + x, color);
        lcd_draw_point(x0 + y, y0 - x, color);
        lcd_draw_point(x0 - y, y0 - x, color);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/* ========================= NV3030B-03 初始化序列 ========================= */
/* 
 * NV3030B-03 与 ST7789 命令兼容，但部分参数不同
 * 以下为 240x320 分辨率的典型配置
 */
static void nv3030b_init_sequence(void)
{
    /* 软件复位 */
    lcd_write_cmd(0x01);
    rt_thread_mdelay(120);
    
    /* 退出睡眠模式 */
    lcd_write_cmd(0x11);
    rt_thread_mdelay(120);
    
    /* 像素格式：RGB565 (0x55 = 16bit) */
    lcd_write_cmd(0x3A);
    lcd_write_data(0x55);
    
    /* 显示反转控制 (根据实际屏幕调整)
     * 0x00: 正常
     * 0x20: 反转颜色
     */
    lcd_write_cmd(0x21);  /* 开启颜色反转 */
    
    /* 内存数据访问顺序 (0x36)
     * 0x00: 正常
     * 0x80: 垂直翻转
     * 0x40: 水平翻转
     * 0x20: RGB->BGR
     * 0x08: 行交换
     * 0x04: 列交换
     * 根据实际屏幕方向调整
     */
    lcd_write_cmd(0x36);
    lcd_write_data(0x00);  /* 正常方向，如需要旋转请调整此值 */
    
    /* 帧率控制 (0xB2) */
    lcd_write_cmd(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);
    
    /* 门控控制 (0xB7) */
    lcd_write_cmd(0xB7);
    lcd_write_data(0x35);
    
    /* 电源控制 */
    lcd_write_cmd(0xBB);
    lcd_write_data(0x19);
    
    lcd_write_cmd(0xC0);
    lcd_write_data(0x2C);
    
    lcd_write_cmd(0xC2);
    lcd_write_data(0x01);
    
    lcd_write_cmd(0xC3);
    lcd_write_data(0x12);
    
    lcd_write_cmd(0xC4);
    lcd_write_data(0x20);
    
    lcd_write_cmd(0xC6);
    lcd_write_data(0x0F);
    
    lcd_write_cmd(0xD0);
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);
    
    /* 伽马校正 (正极性 0xE0) */
    lcd_write_cmd(0xE0);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0D);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2B);
    lcd_write_data(0x3F);
    lcd_write_data(0x54);
    lcd_write_data(0x4C);
    lcd_write_data(0x18);
    lcd_write_data(0x0D);
    lcd_write_data(0x0B);
    lcd_write_data(0x1F);
    lcd_write_data(0x23);
    
    /* 伽马校正 (负极性 0xE1) */
    lcd_write_cmd(0xE1);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0C);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2C);
    lcd_write_data(0x3F);
    lcd_write_data(0x44);
    lcd_write_data(0x51);
    lcd_write_data(0x2F);
    lcd_write_data(0x1F);
    lcd_write_data(0x1F);
    lcd_write_data(0x20);
    lcd_write_data(0x23);
    
    /* 启用显示 */
    lcd_write_cmd(0x29);
    
    rt_thread_mdelay(50);
}

/* ========================= 初始化 ========================= */

int nv3030b_lcd_init(void)
{
    rt_kprintf("\n");
    rt_kprintf("╔════════════════════════════════════════╗\n");
    rt_kprintf("║   NV3030B-03 LCD 初始化 (240x320)      ║\n");
    rt_kprintf("║   TF20QN003-10 屏幕驱动                ║\n");
    rt_kprintf("╚════════════════════════════════════════╝\n");
    
    if (lcd_initialized) {
        rt_kprintf("[LCD] 已经初始化过了\n");
        return RT_EOK;
    }
    
    /* 1. 初始化 GPIO */
    rt_kprintf("[LCD] 初始化 GPIO...\n");
    lcd_gpio_init();
    
    /* 2. 查找 SPI 设备 */
    rt_kprintf("[LCD] 查找 SPI 设备：%s...\n", NV3030B_SPI_DEVICE);
    lcd_spi_dev = rt_spi_bus_attach_device(NV3030B_SPI_DEVICE);
    if (lcd_spi_dev == RT_NULL) {
        rt_kprintf("[LCD] ERROR: 找不到 SPI 设备 %s\n", NV3030B_SPI_DEVICE);
        return -RT_ERROR;
    }
    
    /* 3. 配置 SPI 参数 */
    struct rt_spi_configuration cfg = {0};
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
    cfg.max_hz = 20 * 1000 * 1000;  /* 20MHz */
    
    rt_spi_configure(lcd_spi_dev, &cfg);
    rt_kprintf("[LCD] SPI 配置完成 (20MHz)\n");
    
    /* 4. 硬件复位 */
    rt_kprintf("[LCD] 复位 LCD...\n");
    lcd_reset();
    
    /* 5. 发送初始化序列 */
    rt_kprintf("[LCD] 发送 NV3030B-03 初始化序列...\n");
    nv3030b_init_sequence();
    
    /* 6. 清屏 */
    rt_kprintf("[LCD] 清屏...\n");
    lcd_clear(BLACK);
    
    /* 7. 开启背光 */
    rt_kprintf("[LCD] 开启背光...\n");
    lcd_backlight_on();
    
    lcd_initialized = RT_TRUE;
    rt_kprintf("[LCD] 初始化完成！\n");
    
    return RT_EOK;
}

/* ========================= 显示功能 ========================= */

/* 显示状态信息 */
void lcd_show_status(const char *status)
{
    rt_kprintf("[LCD] 状态：%s\n", status);
    
    lcd_clear(WHITE);
    lcd_draw_string(50, 50, "LuckyPod", BLACK, WHITE);
    lcd_draw_string(50, 150, status, BLACK, WHITE);
}

/* 显示 WiFi QR 码区域 */
void lcd_draw_qrcode(const char *ssid, const char *password)
{
    rt_kprintf("[LCD] 显示 WiFi QR 码\n");
    rt_kprintf("  SSID: %s\n", ssid);
    rt_kprintf("  Password: %s\n", password);
    
    lcd_clear(WHITE);
    lcd_draw_string(10, 10, "WiFi:", BLACK, WHITE);
    lcd_draw_string(50, 10, ssid, BLACK, WHITE);
    
    /* QR 码区域占位 */
    lcd_draw_rectangle(20, 40, 200, 200, BLACK);
    lcd_fill_rectangle(30, 50, 180, 180, BLACK);
    
    char pwd_text[32];
    rt_snprintf(pwd_text, sizeof(pwd_text), "PWD: %s", password);
    lcd_draw_string(10, 250, pwd_text, BLACK, WHITE);
}

/* ========================= MSH 命令 ========================= */

#ifdef FINSH_USING_MSH
#include <finsh.h>

static void lcd_test(void)
{
    rt_kprintf("[LCD Test] 开始测试...\n");
    
    /* 清屏测试 */
    lcd_clear(WHITE);
    rt_kprintf("[LCD Test] 清屏 (白色)\n");
    rt_thread_mdelay(1000);
    
    /* 文字显示测试 */
    lcd_draw_string(10, 10, "Hello", BLACK, WHITE);
    lcd_draw_string(10, 30, "World", RED, WHITE);
    rt_kprintf("[LCD Test] 显示文字\n");
    rt_thread_mdelay(2000);
    
    /* 图形测试 */
    lcd_draw_line(0, 50, 240, 50, BLUE);
    lcd_draw_rectangle(20, 70, 100, 100, GREEN);
    lcd_draw_circle(180, 120, 40, RED);
    rt_kprintf("[LCD Test] 画图形\n");
    rt_thread_mdelay(2000);
    
    /* 填充测试 */
    lcd_fill_rectangle(50, 150, 140, 100, YELLOW);
    rt_kprintf("[LCD Test] 填充矩形\n");
    rt_thread_mdelay(2000);
    
    /* 状态显示 */
    lcd_show_status("测试完成");
    rt_kprintf("[LCD Test] 完成\n");
}

MSH_CMD_EXPORT(nv3030b_lcd_init, 初始化 NV3030B LCD);
MSH_CMD_EXPORT(lcd_test, LCD 测试);

#endif /* FINSH_USING_MSH */

/* 自动初始化 */
INIT_DEVICE_EXPORT(nv3030b_lcd_init);
