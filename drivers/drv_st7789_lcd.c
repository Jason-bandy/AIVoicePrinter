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
#include "arm_arch.h"
#include "../applications/qrcodegen/qrcodegen.h"

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

/* 快速 GPIO 寄存器直写 — 消除 gpio_output() → sddev_control() 调用链开销
 * GPIO30/31 使用索引 30/31，GPIO32-39 使用索引 48-55（见 gpio.h）
 */
#define REG_GPIO_CLK   REG_GPIO_30_CONFIG   /* GPIO30 → index 30 */
#define REG_GPIO_CS    REG_GPIO_31_CONFIG   /* GPIO31 → index 31 */
#define REG_GPIO_MOSI  REG_GPIO_32_CONFIG   /* GPIO32 → index 48 */
#define REG_GPIO_DC    REG_GPIO_34_CONFIG   /* GPIO34 → index 50 */
#define REG_GPIO_RST   REG_GPIO_35_CONFIG   /* GPIO35 → index 51 */
#define REG_GPIO_BLK   REG_GPIO_37_CONFIG   /* GPIO37 → index 53 */

#define FAST_GPIO_SET(reg)   REG_SET_BIT(reg, GCFG_OUTPUT_BIT)
#define FAST_GPIO_CLR(reg)   REG_CLR_BIT(reg, GCFG_OUTPUT_BIT)

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
    FAST_GPIO_CLR(REG_GPIO_CS);
}

static void lcd_cs_deselect(void)
{
    FAST_GPIO_SET(REG_GPIO_CS);
}

static void lcd_dc_command(void)
{
    FAST_GPIO_CLR(REG_GPIO_DC);
}

static void lcd_dc_data(void)
{
    FAST_GPIO_SET(REG_GPIO_DC);
}

static void lcd_reset(void)
{
    /* 拉低复位 */
    FAST_GPIO_CLR(REG_GPIO_RST);
    rt_thread_mdelay(100);

    /* 释放复位 */
    FAST_GPIO_SET(REG_GPIO_RST);
    rt_thread_mdelay(100);
}

/* ========================= 软件模拟 SPI ========================= */

/* 模拟 SPI 发送一个字节 — 寄存器直写，无函数调用开销
 * ST7789 支持 SPI 模式 0 (CPOL=0, CPHA=0) — 时钟空闲低，上升沿采样
 * 160MHz 下每 bit 约 6 次寄存器操作，理论 SPI 时钟 ~10-13MHz
 */
static void soft_spi_write_byte(rt_uint8_t data)
{
    rt_int8_t i;

    for (i = 7; i >= 0; i--)
    {
        /* 时钟低时设置 MOSI 数据 */
        if (data & (1 << i))
            FAST_GPIO_SET(REG_GPIO_MOSI);
        else
            FAST_GPIO_CLR(REG_GPIO_MOSI);

        /* 拉高时钟，上升沿锁存数据 */
        FAST_GPIO_SET(REG_GPIO_CLK);
        /* 短延迟确保 LCD 采样到稳定信号 */
        asm volatile("nop");

        /* 拉低时钟，准备下一次 */
        FAST_GPIO_CLR(REG_GPIO_CLK);
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

/* 初始化测试：画多个彩色方块定位实际可见区域 */
static void lcd_init_test_colors(void)
{
    rt_uint32_t i;
    rt_uint8_t high, low;
    rt_uint16_t w = 60, h = 60;

    /* 先在屏幕四个角各画一个彩色方块 */
    struct { rt_uint16_t x, y; rt_uint16_t color; } blocks[6] = {
        { 0, 0, RED },       /* 左上 - 红色 */
        { LCD_WIDTH - w, 0, GREEN },  /* 右上 - 绿色 */
        { 0, LCD_HEIGHT - h, BLUE },  /* 左下 - 蓝色 */
        { LCD_WIDTH - w, LCD_HEIGHT - h, YELLOW }, /* 右下 - 黄色 */
        { 80, 80, CYAN },    /* 中间偏上 - 青色 */
        { LCD_WIDTH / 2 - w / 2, LCD_HEIGHT / 2 - h / 2, MAGENTA }, /* 正中 - 品红 */
    };

    for (int b = 0; b < 6; b++) {
        high = blocks[b].color >> 8;
        low = blocks[b].color & 0xFF;

        lcd_dc_command();
        lcd_cs_select();
        soft_spi_write_byte(0x2A);
        lcd_dc_data();
        soft_spi_write_byte((blocks[b].x + LCD_COL_OFFSET) >> 8);
        soft_spi_write_byte((blocks[b].x + LCD_COL_OFFSET) & 0xFF);
        soft_spi_write_byte(((blocks[b].x + w - 1) + LCD_COL_OFFSET) >> 8);
        soft_spi_write_byte(((blocks[b].x + w - 1) + LCD_COL_OFFSET) & 0xFF);
        lcd_cs_deselect();

        lcd_dc_command();
        lcd_cs_select();
        soft_spi_write_byte(0x2B);
        lcd_dc_data();
        soft_spi_write_byte((blocks[b].y + LCD_ROW_OFFSET) >> 8);
        soft_spi_write_byte((blocks[b].y + LCD_ROW_OFFSET) & 0xFF);
        soft_spi_write_byte(((blocks[b].y + h - 1) + LCD_ROW_OFFSET) >> 8);
        soft_spi_write_byte(((blocks[b].y + h - 1) + LCD_ROW_OFFSET) & 0xFF);
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

    rt_kprintf("[LCD Test] 6个彩色方块已画: 左上红/右上绿/左下蓝/右下黄/中上青/正中品红\n");
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

/* QR 码缓冲区动态分配，用完即释放，不占常驻内存（两个缓冲区约 8KB） */

/* 显示 QR 码（动态分配缓冲区，用完即释放） */
void lcd_draw_qrcode(const char *ssid, const char *password)
{
    char wifi_payload[128];
    uint8_t *qr_temp = RT_NULL;
    uint8_t *qr_code = RT_NULL;

    rt_kprintf("[LCD] 显示 WiFi QR 码\n");
    rt_kprintf("  SSID: %s\n", ssid);
    rt_kprintf("  Password: %s\n", password);

    qr_temp = rt_malloc(qrcodegen_BUFFER_LEN_MAX);
    qr_code = rt_malloc(qrcodegen_BUFFER_LEN_MAX);
    if (!qr_temp || !qr_code) {
        rt_kprintf("[LCD] QR OOM: cannot allocate %d bytes\n", qrcodegen_BUFFER_LEN_MAX * 2);
        lcd_clear(WHITE);
        lcd_draw_string(60, 140, "QR OOM", BLACK, WHITE);
        goto cleanup;
    }

    rt_snprintf(wifi_payload, sizeof(wifi_payload), "WIFI:T:WPA;S:%s;P:%s;;", ssid, password);

    if (!qrcodegen_encodeText(wifi_payload, qr_temp, qr_code,
                              qrcodegen_Ecc_MEDIUM, 1, 40,
                              qrcodegen_Mask_AUTO, true)) {
        rt_kprintf("[LCD] QR encode failed\n");
        lcd_clear(WHITE);
        lcd_draw_string(60, 140, "QR fail", BLACK, WHITE);
        goto cleanup;
    }

    int size = qrcodegen_getSize(qr_code);
    int scale = 4;
    int quiet = 4;
    int qr_px = (size + quiet * 2) * scale;
    int offset_x = (LCD_WIDTH - qr_px) / 2;
    int offset_y = 20;

    lcd_clear(WHITE);

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcodegen_getModule(qr_code, x, y)) {
                int sx = offset_x + (x + quiet) * scale;
                int sy = offset_y + (y + quiet) * scale;
                lcd_fill_rectangle(sx, sy, scale, scale, BLACK);
            }
        }
        if (y % 8 == 0) {
            rt_thread_delay(1);
        }
    }

    char ssid_text[64];
    rt_snprintf(ssid_text, sizeof(ssid_text), "WiFi: %s", ssid);
    int text_y = offset_y + qr_px + 8;
    if (text_y + 16 < LCD_HEIGHT) {
        lcd_draw_string(10, text_y, ssid_text, BLACK, WHITE);
    }

    rt_kprintf("[LCD] QR done: %dx%d, scale=%d\n", size, size, scale);

cleanup:
    if (qr_temp) rt_free(qr_temp);
    if (qr_code) rt_free(qr_code);
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
    lcd_write_cmd(0x21);  /* 0x21=反转（部分面板默认已反转，需反转来抵消） */

    rt_kprintf("[LCD] 5. MADCTL (方向+色序)...\n");
    lcd_write_cmd(0x36);
    lcd_write_data(0x08);  /* 竖屏模式，BGR 色序（R/B 交换） */
    
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

    /* 4. 彩色测试方块（避免全屏清屏阻塞太久导致重启） */
    rt_kprintf("[LCD] 4. 画彩色测试方块...\n");
    lcd_init_test_colors();
    rt_kprintf("[LCD] 测试方块完成\n");

    lcd_initialized = RT_TRUE;
    rt_kprintf("[LCD] === 初始化完成！===\n");

    return RT_EOK;
}

/* ========================= 背光控制 ========================= */

void lcd_backlight_on(void)
{
    FAST_GPIO_SET(REG_GPIO_BLK);
    rt_kprintf("[LCD] 背光开启\n");
}

void lcd_backlight_off(void)
{
    FAST_GPIO_CLR(REG_GPIO_BLK);
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
