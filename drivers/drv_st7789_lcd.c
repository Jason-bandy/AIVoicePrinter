/*
 * ST7789 320x240 LCD 驱动程序
 * 硬件接口：软件模拟 SPI
 * 适用：BK7252N + 320x240 IPS 显示屏 (ST7789 驱动)
 *
 * 引脚连接说明（使用 VIDEO IF 排针）：
 * ┌─────────────────────────────────────────────────────┐
 * │  LCD 引脚 (12Pin)  →    BK7252N VIDEO IF          │
 * ├─────────────────────────────────────────────────────┤
 * │  1 SDA (SPI 数据)  →    GPIO38 (D6/P38)            │
 * │  2 SCL (SPI 时钟)  →    GPIO37 (D5/P37)            │
 * │  3 RESET (复位)    →    GPIO34 (D2/P34)            │
 * │  4 RS/A0 (DC)     →    GPIO35 (D3/P35)            │
 * │  5 CS (片选)      →    GPIO36 (D4/P36)            │
 * │  6 GND            →    GND                         │
 * │  7 VDD (2.8V)     →    VBAT (3.3V, 建议串 10Ω)     │
 * │  8-10 NC          →    悬空                        │
 * │  11 K- (LED 阴极)  →    GND                         │
 * │  12 A+ (LED 阳极)  →    VBAT (串 22-47Ω 电阻)        │
 * └─────────────────────────────────────────────────────┘
 *
 * VIDEO IF 排针引脚定义（从上到下，从左到右）：
 * ┌─────────────────────────────────────────────────────────┐
 * │ HSYNC/P30   D1/P29    SYNC/P34   TX1/P22               │
 * │ D2/P34      D3/P35    D4/P36     RX1/P23               │
 * │ D5/P37      D6/P38    D7/P39     GND                   │
 * │ VBAT        24V       NC         NC                    │
 * └─────────────────────────────────────────────────────────┘
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdio.h>

#include "gpio_pub.h"
#include "drv_model_pub.h"
#include "gpio.h"

/* ========================= 硬件配置 ========================= */

/* GPIO 定义（根据 BK7252N 实际引脚调整）
 *
 * 根据你的接线：
 * ┌──────────────────────────────────────────────────────────┐
 * │  LCD 引脚  │  芯片引脚  │  BK7252N GPIO              │
 * ├──────────────────────────────────────────────────────────┤
 * │  SDA      │  P32       │  GPIO32 (SPI0_MOSI)        │
 * │  SCL      │  P30       │  GPIO30 (SPI0_SCK)         │
 * │  CS       │  P31       │  GPIO31 (SPI0_CSN)         │
 * │  RS/A0    │  P34       │  GPIO34                    │
 * │  RESET    │  P35       │  GPIO35                    │
 * │  A+ (BLK) │  P37       │  GPIO37                    │
 * └──────────────────────────────────────────────────────────┘
 */
#define LCD_CS_PIN           GPIO31
#define LCD_CLK_PIN          GPIO30
#define LCD_MOSI_PIN         GPIO32
#define LCD_DC_PIN           GPIO34
#define LCD_RST_PIN          GPIO35
#define LCD_BLK_PIN          GPIO37  /* 背光控制 */

/* 屏幕参数 - ZH024B12550C: 2.4 寸 TFT, 240x320 竖屏，ST7789P3
 * 如果横向使用，通过 MADCTL 旋转
 */
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

static rt_bool_t lcd_initialized = RT_FALSE;

/* ========================= GPIO 控制 ========================= */

/*
 * BK7252N 底层 GPIO API：
 * - gpio_ctrl(CMD_GPIO_CFG, &param) 配置 GPIO
 * - gpio_output(pin, 0/1) 设置输出电平（快速）
 * - param = GPIO_CFG_PARAM(pin, mode) 配置参数，mode: GMODE_OUTPUT, GMODE_INPUT 等
 * - param = GPIO_OUTPUT_PARAM(pin, val) 输出参数，val: 0(低) 或 1(高)
 */

static void lcd_gpio_init(void)
{
    UINT32 param;

    /* 初始化 CLK (SPI 时钟) - 输出低电平 */
    param = GPIO_CFG_PARAM(LCD_CLK_PIN, GMODE_OUTPUT);
    gpio_ctrl(CMD_GPIO_CFG, &param);
    gpio_output(LCD_CLK_PIN, 0);

    /* 初始化 MOSI (SPI 数据) - 输出低电平 */
    param = GPIO_CFG_PARAM(LCD_MOSI_PIN, GMODE_OUTPUT);
    gpio_ctrl(CMD_GPIO_CFG, &param);
    gpio_output(LCD_MOSI_PIN, 0);

    /* 初始化 CS (片选) - 输出高电平，默认不选中 */
    param = GPIO_CFG_PARAM(LCD_CS_PIN, GMODE_OUTPUT);
    gpio_ctrl(CMD_GPIO_CFG, &param);
    gpio_output(LCD_CS_PIN, 1);

    /* 初始化 DC (数据/命令选择) - 输出低电平 */
    param = GPIO_CFG_PARAM(LCD_DC_PIN, GMODE_OUTPUT);
    gpio_ctrl(CMD_GPIO_CFG, &param);
    gpio_output(LCD_DC_PIN, 0);

    /* 初始化 RST (复位) - 输出高电平 */
    param = GPIO_CFG_PARAM(LCD_RST_PIN, GMODE_OUTPUT);
    gpio_ctrl(CMD_GPIO_CFG, &param);
    gpio_output(LCD_RST_PIN, 1);

    /* 初始化 BLK (背光控制) - 输出高电平，点亮背光 */
    param = GPIO_CFG_PARAM(LCD_BLK_PIN, GMODE_OUTPUT);
    gpio_ctrl(CMD_GPIO_CFG, &param);
    gpio_output(LCD_BLK_PIN, 1);
}

static void lcd_cs_select(void)
{
    gpio_output(LCD_CS_PIN, 0);
}

static void lcd_cs_deselect(void)
{
    gpio_output(LCD_CS_PIN, 1);
}

static void lcd_dc_command(void)
{
    gpio_output(LCD_DC_PIN, 0);
}

static void lcd_dc_data(void)
{
    gpio_output(LCD_DC_PIN, 1);
}

static void lcd_reset(void)
{
    /* 拉低复位 */
    gpio_output(LCD_RST_PIN, 0);
    rt_thread_mdelay(100);

    /* 释放复位 */
    gpio_output(LCD_RST_PIN, 1);
    rt_thread_mdelay(100);
}

/* ========================= 软件模拟 SPI ========================= */

/* 软件延迟 - 用于控制 SPI 速度 */
static void spi_delay(void)
{
    /* 空延迟 - 让 GPIO 变化有时间稳定
     * 160MHz 下 50 次循环约 0.5-1μs
     * 配合 clock-high 的第二次 delay，SPI 时钟约 ~500kHz */
    volatile int i;
    for (i = 0; i < 50; i++);
}

/* 模拟 SPI 发送一个字节 - 使用快速 GPIO 操作
 * ST7789 支持 SPI 模式 0 (CPOL=0, CPHA=0) - 时钟空闲低，上升沿采样
 */
static void soft_spi_write_byte(rt_uint8_t data)
{
    rt_int8_t i;

    for (i = 7; i >= 0; i--)
    {
        /* 设置 MOSI 数据（时钟低时变化） */
        gpio_output(LCD_MOSI_PIN, (data >> i) & 1);
        spi_delay();

        /* 拉高时钟，锁存数据 */
        gpio_output(LCD_CLK_PIN, 1);
        spi_delay();

        /* 保持时钟高一小段时间，确保 LCD 采样到数据 */
        spi_delay();

        /* 拉低时钟，准备下一次 */
        gpio_output(LCD_CLK_PIN, 0);
    }
}

/* 发送命令 */
static void lcd_write_cmd(rt_uint8_t cmd)
{
    lcd_dc_command();
    lcd_cs_select();

    soft_spi_write_byte(cmd);

    lcd_cs_deselect();
}

/* 发送数据 */
static void lcd_write_data(rt_uint8_t data)
{
    lcd_dc_data();
    lcd_cs_select();

    soft_spi_write_byte(data);

    lcd_cs_deselect();
}

/* 发送多个数据 */
static void lcd_write_data_multi(const rt_uint8_t *data, rt_size_t len)
{
    lcd_dc_data();
    lcd_cs_select();

    while (len--)
    {
        soft_spi_write_byte(*data++);
    }

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

/* ST7789 列偏移量，根据实际屏幕调整
 * 240x320 屏幕通常为 0，如果遇到花屏/偏移可尝试 1-3
 */
#define LCD_COL_OFFSET 0
#define LCD_ROW_OFFSET 0

/* 设置光标位置 */
static void lcd_set_cursor(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
    /* 列地址设置 */
    lcd_write_cmd(0x2A);
    lcd_write_data16(x1 + LCD_COL_OFFSET);
    lcd_write_data16(x2 + LCD_COL_OFFSET);

    /* 行地址设置 */
    lcd_write_cmd(0x2B);
    lcd_write_data16(y1 + LCD_ROW_OFFSET);
    lcd_write_data16(y2 + LCD_ROW_OFFSET);

    /* 内存写 */
    lcd_write_cmd(0x2C);
}

/* 清屏 - 不打印调试信息 */
void lcd_clear(rt_uint16_t color)
{
    rt_uint32_t i;
    rt_uint8_t high = color >> 8;
    rt_uint8_t low = color & 0xFF;

    /* 设置显示窗口 */
    lcd_dc_command();
    lcd_cs_select();
    soft_spi_write_byte(0x2A);  /* 列地址设置 */
    lcd_dc_data();
    soft_spi_write_byte((0 + LCD_COL_OFFSET) >> 8);
    soft_spi_write_byte((0 + LCD_COL_OFFSET) & 0xFF);
    soft_spi_write_byte(((LCD_WIDTH - 1) + LCD_COL_OFFSET) >> 8);
    soft_spi_write_byte(((LCD_WIDTH - 1) + LCD_COL_OFFSET) & 0xFF);
    lcd_cs_deselect();

    lcd_dc_command();
    lcd_cs_select();
    soft_spi_write_byte(0x2B);  /* 行地址设置 */
    lcd_dc_data();
    soft_spi_write_byte((0 + LCD_ROW_OFFSET) >> 8);
    soft_spi_write_byte((0 + LCD_ROW_OFFSET) & 0xFF);
    soft_spi_write_byte(((LCD_HEIGHT - 1) + LCD_ROW_OFFSET) >> 8);
    soft_spi_write_byte(((LCD_HEIGHT - 1) + LCD_ROW_OFFSET) & 0xFF);
    lcd_cs_deselect();

    /* 内存写命令 */
    lcd_dc_command();
    lcd_cs_select();
    soft_spi_write_byte(0x2C);
    lcd_dc_data();

    /* 发送全屏像素数据 */
    for (i = 0; i < (rt_uint32_t)LCD_WIDTH * LCD_HEIGHT; i++) {
        soft_spi_write_byte(high);
        soft_spi_write_byte(low);
    }

    lcd_cs_deselect();
}

/* 快速填充小块区域（用于初始化测试，避免阻塞太久） */
static void lcd_fill_test_area(rt_uint16_t color)
{
    rt_uint32_t i;
    rt_uint8_t high = color >> 8;
    rt_uint8_t low = color & 0xFF;
    rt_uint16_t w = 100;  /* 只填充 100x100 区域 */
    rt_uint16_t h = 100;

    lcd_dc_command();
    lcd_cs_select();
    soft_spi_write_byte(0x2A);
    lcd_dc_data();
    soft_spi_write_byte((0 + LCD_COL_OFFSET) >> 8);
    soft_spi_write_byte((0 + LCD_COL_OFFSET) & 0xFF);
    soft_spi_write_byte(((w - 1) + LCD_COL_OFFSET) >> 8);
    soft_spi_write_byte(((w - 1) + LCD_COL_OFFSET) & 0xFF);
    lcd_cs_deselect();

    lcd_dc_command();
    lcd_cs_select();
    soft_spi_write_byte(0x2B);
    lcd_dc_data();
    soft_spi_write_byte((0 + LCD_ROW_OFFSET) >> 8);
    soft_spi_write_byte((0 + LCD_ROW_OFFSET) & 0xFF);
    soft_spi_write_byte(((h - 1) + LCD_ROW_OFFSET) >> 8);
    soft_spi_write_byte(((h - 1) + LCD_ROW_OFFSET) & 0xFF);
    lcd_cs_deselect();

    lcd_dc_command();
    lcd_cs_select();
    soft_spi_write_byte(0x2C);
    lcd_dc_data();

    for (i = 0; i < (rt_uint32_t)w * h; i++) {
        soft_spi_write_byte(high);
        soft_spi_write_byte(low);
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
        soft_spi_write_byte(high);
        soft_spi_write_byte(low);
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
    lcd_draw_string(60, 10, ssid, BLACK, WHITE);

    /* TODO: 集成 QR 码生成库后绘制 QR 码 */
    /* 临时：画一个方框表示 QR 码区域 */
    lcd_draw_rectangle(60, 40, 200, 200, BLACK);
    lcd_fill_rectangle(70, 50, 180, 180, BLACK);

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

    /* 显示 Logo - 居中显示 */
    lcd_draw_string(110, 50, "LuckyPod", BLACK, WHITE);

    /* 显示状态 - 居中显示 */
    lcd_draw_string(120, 120, status, BLACK, WHITE);
}

/* ========================= 初始化 ========================= */

static void st7789_init_sequence(void)
{
    rt_kprintf("[LCD] 1. 软件复位...\n");
    lcd_write_cmd(0x01);
    rt_thread_mdelay(120);

    rt_kprintf("[LCD] 2. 退出睡眠...\n");
    lcd_write_cmd(0x11);
    rt_thread_mdelay(120);

    rt_kprintf("[LCD] 3. 像素格式 RGB565...\n");
    lcd_write_cmd(0x3A);
    lcd_write_data(0x55);

    rt_kprintf("[LCD] 4. 正常显示模式 (非反转)...\n");
    lcd_write_cmd(0x20);  /* 0x20=正常，0x21=反转 */

    rt_kprintf("[LCD] 5. MADCTL (方向+色序)...\n");
    lcd_write_cmd(0x36);
    lcd_write_data(0x00);  /* 竖屏模式，RGB 色序（非 BGR） */
    
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
    rt_thread_mdelay(120);
}

int st7789_lcd_init(void)
{
    rt_kprintf("\n");
    rt_kprintf("╔════════════════════════════════════════╗\n");
    rt_kprintf("║   ST7789 LCD 驱动初始化 (240x320)      ║\n");
    rt_kprintf("╚════════════════════════════════════════╝\n");

    if (lcd_initialized) {
        rt_kprintf("[LCD] 已经初始化过了，跳过\n");
        return RT_EOK;
    }

    /* 1. 初始化 GPIO */
    rt_kprintf("[LCD] 1. 初始化 GPIO...\n");
    lcd_gpio_init();
    rt_kprintf("[LCD] GPIO 初始化完成\n");

    /* 2. 硬件复位 */
    rt_kprintf("[LCD] 2. 复位 LCD...\n");
    lcd_reset();
    rt_kprintf("[LCD] 复位完成\n");

    /* 3. 发送初始化序列 */
    rt_kprintf("[LCD] 3. 发送初始化序列...\n");
    st7789_init_sequence();
    rt_kprintf("[LCD] 初始化序列完成\n");

    /* 4. 小区域填充测试（避免全屏清屏阻塞太久导致重启） */
    rt_kprintf("[LCD] 4. 填充测试区域...\n");
    lcd_fill_test_area(BLACK);
    rt_kprintf("[LCD] 测试区域填充完成\n");

    lcd_initialized = RT_TRUE;
    rt_kprintf("[LCD] === 初始化完成！===\n");

    return RT_EOK;
}

/* ========================= 背光控制 ========================= */

void lcd_backlight_on(void)
{
    gpio_output(LCD_BLK_PIN, 1);
    rt_kprintf("[LCD] 背光开启\n");
}

void lcd_backlight_off(void)
{
    gpio_output(LCD_BLK_PIN, 0);
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

/* 全屏测试 - 填充整个屏幕为红色 */
static void lcd_test(void)
{
    rt_kprintf("[LCD Test] 开始测试...\n");
    rt_kprintf("[LCD Test] 填充整个屏幕为红色...\n");
    lcd_clear(RED);
    rt_kprintf("[LCD Test] 完成，整个屏幕应该是红色\n");
}

/* 测试：直接发送数据，不经过任何缓冲区 - 填充全屏绿色 */
static void lcd_test_raw(void)
{
    rt_kprintf("[LCD Raw Test] 开始...\n");

    /* 设置整个屏幕为显示窗口 */
    lcd_set_cursor(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    lcd_dc_data();
    lcd_cs_select();

    /* 发送纯绿色数据 (RGB565: 0x07E0) */
    rt_kprintf("[LCD Raw Test] 发送绿色数据...\n");
    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
        soft_spi_write_byte(0x07);  /* 绿色高字节 */
        soft_spi_write_byte(0xE0);  /* 绿色低字节 */
    }

    lcd_cs_deselect();
    rt_kprintf("[LCD Raw Test] 完成，整个屏幕应该是绿色\n");
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
MSH_CMD_EXPORT(lcd_test_raw, LCD 原始数据测试);
MSH_CMD_EXPORT(lcd_bl_on, LCD 背光开启);
MSH_CMD_EXPORT(lcd_bl_off, LCD 背光关闭);

#endif /* FINSH_USING_MSH */

/* 自动初始化 */
INIT_DEVICE_EXPORT(st7789_lcd_init);
