/*
 * ST7789 240x240 LCD 驱动程序
 * 硬件接口：SPI
 * 适用：BK7252N + ZH024B12550C 显示屏 (240x240 IPS)
 *
 * 引脚连接说明：
 * ┌─────────────────────────────────────────────────────┐
 * │  ZH024B12550C 显示屏     →    BK7252N 开发板       │
 * ├─────────────────────────────────────────────────────┤
 * │  Pin 1 VCC             →    3.3V (VBAT)            │
 * │  Pin 2 GND             →    GND                    │
 * │  Pin 3 CS              →    GPIO6  (SPI0 CS)       │
 * │  Pin 4 RESET           →    GPIO10                 │
 * │  Pin 5 DC              →    GPIO9                  │
 * │  Pin 6 SDO             →    悬空 (MISO 不用)       │
 * │  Pin 7 SCLK            →    GPIO7  (SPI0 CLK)      │
 * │  Pin 8 SDI             →    GPIO8  (SPI0 MOSI)     │
 * │  Pin 9 LED_A (背光 +)   →    3.3V (串联电阻)        │
 * │  Pin 10 LED_K (背光 -)  →    GND                    │
 * └─────────────────────────────────────────────────────┘
 *
 * 如需修改引脚，请更改下方的 LCD_CS_PIN、LCD_DC_PIN、LCD_RST_PIN 定义
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
#define ST7789_SPI_DEVICE    "spi0"

/* GPIO 定义（根据 BK7252N 实际引脚调整）
 *
 * 默认引脚配置（ZH024B12550C 显示屏）：
 * ┌──────────────────────────────────────────────────────────┐
 * │  LCD 信号    │  BK7252N GPIO  │      功能说明           │
 * ├──────────────────────────────────────────────────────────┤
 * │  CS         │  GPIO6         │  SPI 片选，低电平有效   │
 * │  CLK        │  GPIO7         │  SPI 时钟               │
 * │  MOSI       │  GPIO8         │  SPI 数据输出           │
 * │  DC         │  GPIO9         │  数据/命令选择          │
 * │  RST        │  GPIO10        │  复位，低电平有效       │
 * │  BLK/PWM    │  GPIO11        │  背光控制 (PWM 可选)     │
 * └──────────────────────────────────────────────────────────┘
 *
 * 注意：BK7252N 的 GPIO 编号直接使用数字，不需要 GPIO_PBx 格式
 */
#define LCD_CS_PIN           GPIO6
#define LCD_CLK_PIN          GPIO7
#define LCD_MOSI_PIN         GPIO8
#define LCD_DC_PIN           GPIO9
#define LCD_RST_PIN          GPIO10
#define LCD_BLK_PIN          GPIO11  /* 背光控制，可选 */

/* 屏幕参数 */
#define LCD_WIDTH            240
#define LCD_HEIGHT           240

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
    gpio_set_pin_value(LCD_BLK_PIN, GPIO_HIGH);  /* 高电平点亮背光 */
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
    rt_thread_mdelay(100);
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

/* 设置光标位置 */
static void lcd_set_cursor(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
    /* 列地址设置 */
    lcd_write_cmd(0x2A);
    lcd_write_data16(x1 + 0);  /* 偏移量，根据实际屏幕调整 */
    lcd_write_data16(x2 + 0);
    
    /* 行地址设置 */
    lcd_write_cmd(0x2B);
    lcd_write_data16(y1 + 0);
    lcd_write_data16(y2 + 0);
    
    /* 内存写 */
    lcd_write_cmd(0x2C);
}

/* 清屏 */
void lcd_clear(rt_uint16_t color)
{
    rt_uint32_t i;
    rt_uint8_t high = color >> 8;
    rt_uint8_t low = color & 0xFF;
    
    lcd_set_cursor(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    lcd_dc_data();
    lcd_cs_select();
    
    /* 优化：一次发送多个像素 */
    for (i = 0; i < (rt_uint32_t)LCD_WIDTH * LCD_HEIGHT / 16; i++) {
        for (rt_uint8_t j = 0; j < 16; j++) {
            rt_spi_write(lcd_spi_dev, &high, 1);
            rt_spi_write(lcd_spi_dev, &low, 1);
        }
    }
    
    lcd_cs_deselect();
}

/* 设置显示窗口 */
void lcd_set_window(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height)
{
    lcd_set_cursor(x, y, x + width - 1, y + height - 1);
}

/* 画点 */
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return;
    }
    
    lcd_set_cursor(x, y, x, y);
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
    /* 上边 */
    lcd_draw_line(x, y, x + width, y, color);
    /* 下边 */
    lcd_draw_line(x, y + height, x + width, y + height, color);
    /* 左边 */
    lcd_draw_line(x, y, x, y + height, color);
    /* 右边 */
    lcd_draw_line(x + width, y, x + width, y + height, color);
}

/* 填充矩形 */
void lcd_fill_rectangle(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, rt_uint16_t color)
{
    rt_uint32_t i;
    rt_uint16_t total = width * height;
    
    lcd_set_window(x, y, width, height);
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

/* ========================= 文字显示（简单点阵） ========================= */

/* 5x7 点阵字体（简化版，只包含数字和字母） */
static const rt_uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00},  /* 空格 */
    /* 0-9 */
    {0x3E, 0x51, 0x49, 0x45, 0x3E},  /* 0 */
    {0x00, 0x42, 0x7F, 0x40, 0x00},  /* 1 */
    {0x42, 0x61, 0x51, 0x49, 0x46},  /* 2 */
    {0x21, 0x41, 0x45, 0x4B, 0x31},  /* 3 */
    {0x18, 0x14, 0x12, 0x7F, 0x10},  /* 4 */
    {0x27, 0x45, 0x45, 0x45, 0x39},  /* 5 */
    {0x3C, 0x4A, 0x49, 0x49, 0x30},  /* 6 */
    {0x01, 0x71, 0x09, 0x05, 0x03},  /* 7 */
    {0x36, 0x49, 0x49, 0x49, 0x36},  /* 8 */
    {0x06, 0x49, 0x49, 0x29, 0x1E},  /* 9 */
};

/* 显示字符（5x7 点阵） */
void lcd_draw_char(rt_uint16_t x, rt_uint16_t y, char chr, rt_uint16_t color, rt_uint16_t bg_color)
{
    rt_uint8_t i, j;
    rt_uint8_t index;
    
    /* 计算字符索引 */
    if (chr >= '0' && chr <= '9') {
        index = chr - '0' + 1;
    } else if (chr >= 'A' && chr <= 'Z') {
        /* TODO: 添加大写字母点阵 */
        return;
    } else {
        return;  /* 不支持的字符 */
    }
    
    lcd_set_window(x, y, 5, 7);
    lcd_dc_data();
    lcd_cs_select();
    
    for (i = 0; i < 5; i++) {
        rt_uint8_t line = font5x7[index][i];
        for (j = 0; j < 8; j++) {
            if (line & (1 << (7 - j))) {
                lcd_write_data16(color);
            } else {
                lcd_write_data16(bg_color);
            }
        }
    }
    
    lcd_cs_deselect();
}

/* 显示字符串 */
void lcd_draw_string(rt_uint16_t x, rt_uint16_t y, const char *str, rt_uint16_t color, rt_uint16_t bg_color)
{
    rt_uint16_t offset = 0;
    
    while (*str) {
        lcd_draw_char(x + offset, y, *str, color, bg_color);
        offset += 6;  /* 字符宽度 + 间距 */
        str++;
    }
}

/* ========================= QR 码显示 ========================= */

/* 显示 QR 码模块 */
void lcd_draw_qrcode_module(rt_uint16_t x, rt_uint16_t y, rt_uint8_t size, rt_bool_t black)
{
    rt_uint8_t i, j;
    rt_uint16_t color = black ? BLACK : WHITE;
    
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            lcd_draw_point(x + i, y + j, color);
        }
    }
}

/* 显示 QR 码（简化版，需要集成 QR 码生成库） */
void lcd_draw_qrcode(const char *ssid, const char *password)
{
    rt_kprintf("[LCD] 显示 WiFi QR 码\n");
    rt_kprintf("  SSID: %s\n", ssid);
    rt_kprintf("  Password: %s\n", password);
    
    /* 清屏为白色 */
    lcd_clear(WHITE);
    
    /* 显示标题 */
    lcd_draw_string(10, 10, "WiFi:", BLACK, WHITE);
    lcd_draw_string(50, 10, ssid, BLACK, WHITE);
    
    /* TODO: 集成 QR 码生成库后绘制 QR 码 */
    /* 临时：画一个方框表示 QR 码区域 */
    lcd_draw_rectangle(20, 40, 200, 200, BLACK);
    lcd_fill_rectangle(30, 50, 180, 180, BLACK);
    
    /* 显示密码 */
    char pwd_text[32];
    rt_snprintf(pwd_text, sizeof(pwd_text), "PWD: %s", password);
    lcd_draw_string(10, 250, pwd_text, BLACK, WHITE);
}

/* ========================= 状态显示 ========================= */

void lcd_show_status(const char *status)
{
    rt_kprintf("[LCD] 状态：%s\n", status);
    
    /* 清屏 */
    lcd_clear(WHITE);
    
    /* 显示 Logo */
    lcd_draw_string(70, 50, "LuckyPod", BLACK, WHITE);
    
    /* 显示状态 */
    lcd_draw_string(50, 150, status, BLACK, WHITE);
}

/* ========================= 初始化 ========================= */

static void st7789_init_sequence(void)
{
    /* 软件复位 */
    lcd_write_cmd(0x01);
    rt_thread_mdelay(120);
    
    /* 退出睡眠模式 */
    lcd_write_cmd(0x11);
    rt_thread_mdelay(120);
    
    /* 像素格式：RGB565 */
    lcd_write_cmd(0x3A);
    lcd_write_data(0x55);
    
    /* 显示反转（根据实际屏幕调整） */
    lcd_write_cmd(0x21);
    
    /* 内存数据访问顺序 */
    lcd_write_cmd(0x36);
    lcd_write_data(0x00);  /* 根据实际屏幕方向调整 */
    
    /* 帧率控制 */
    lcd_write_cmd(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);
    
    /* 门控控制 */
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
    
    /* 伽马校正 */
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
}

int st7789_lcd_init(void)
{
    rt_kprintf("\n");
    rt_kprintf("╔════════════════════════════════════════╗\n");
    rt_kprintf("║   ST7789 LCD 驱动初始化 (240x240)      ║\n");
    rt_kprintf("╚════════════════════════════════════════╝\n");
    
    if (lcd_initialized) {
        rt_kprintf("[LCD] 已经初始化过了\n");
        return RT_EOK;
    }
    
    /* 1. 初始化 GPIO */
    rt_kprintf("[LCD] 初始化 GPIO...\n");
    lcd_gpio_init();
    
    /* 2. 查找 SPI 设备 */
    rt_kprintf("[LCD] 查找 SPI 设备：%s...\n", ST7789_SPI_DEVICE);
    lcd_spi_dev = rt_spi_bus_attach_device(ST7789_SPI_DEVICE);
    if (lcd_spi_dev == RT_NULL) {
        rt_kprintf("[LCD] ERROR: 找不到 SPI 设备 %s\n", ST7789_SPI_DEVICE);
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
    rt_kprintf("[LCD] 发送初始化序列...\n");
    st7789_init_sequence();
    
    /* 6. 清屏 */
    rt_kprintf("[LCD] 清屏...\n");
    lcd_clear(BLACK);
    
    lcd_initialized = RT_TRUE;
    rt_kprintf("[LCD] 初始化完成！\n");
    
    return RT_EOK;
}

/* ========================= 背光控制 ========================= */

void lcd_backlight_on(void)
{
    gpio_set_pin_value(LCD_BLK_PIN, GPIO_HIGH);
    rt_kprintf("[LCD] 背光开启\n");
}

void lcd_backlight_off(void)
{
    gpio_set_pin_value(LCD_BLK_PIN, GPIO_LOW);
    rt_kprintf("[LCD] 背光关闭\n");
}

void lcd_backlight_set(rt_uint8_t level)
{
    /* level: 0-100, PWM 控制亮度（需要 PWM 支持） */
    if (level == 0)
        lcd_backlight_off();
    else
        lcd_backlight_on();
}

/* ========================= MSH 命令 ========================= */

#ifdef FINSH_USING_MSH
#include <finsh.h>

static void lcd_test(void)
{
    rt_kprintf("[LCD Test] 开始测试...\n");

    /* 清屏 */
    lcd_clear(WHITE);
    rt_kprintf("[LCD Test] 清屏 (白色)\n");
    rt_thread_mdelay(1000);

    /* 显示文字 */
    lcd_draw_string(10, 10, "Hello", BLACK, WHITE);
    lcd_draw_string(10, 30, "World", RED, WHITE);
    rt_kprintf("[LCD Test] 显示文字\n");
    rt_thread_mdelay(2000);

    /* 画图形 */
    lcd_draw_line(0, 50, 240, 50, BLUE);
    lcd_draw_rectangle(20, 70, 100, 100, GREEN);
    lcd_draw_circle(180, 120, 40, RED);
    rt_kprintf("[LCD Test] 画图形\n");
    rt_thread_mdelay(2000);

    /* 显示状态 */
    lcd_show_status("测试完成");
    rt_kprintf("[LCD Test] 完成\n");
}

static void lcd_bl_on(void)
{
    lcd_backlight_on();
}

static void lcd_bl_off(void)
{
    lcd_backlight_off();
}

MSH_CMD_EXPORT(st7789_lcd_init, 初始化 ST7789 LCD);
MSH_CMD_EXPORT(lcd_test, LCD 测试);
MSH_CMD_EXPORT(lcd_bl_on, LCD 背光开启);
MSH_CMD_EXPORT(lcd_bl_off, LCD 背光关闭);

#endif /* FINSH_USING_MSH */

/* 自动初始化 */
INIT_DEVICE_EXPORT(st7789_lcd_init);
