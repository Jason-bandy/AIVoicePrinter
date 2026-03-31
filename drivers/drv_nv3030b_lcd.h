/*
 * NV3030B-03 LCD 驱动头文件
 * 适用：TF20QN003-10 屏幕 (240x320, SPI 接口)
 */

#ifndef __DRV_NV3030B_LCD_H__
#define __DRV_NV3030B_LCD_H__

#include <rtthread.h>

/* 颜色定义 (RGB565 格式) */
#define LCD_WHITE       0xFFFF
#define LCD_BLACK       0x0000
#define LCD_BLUE        0x001F
#define LCD_RED         0xF800
#define LCD_GREEN       0x07E0
#define LCD_CYAN        0x07FF
#define LCD_MAGENTA     0xF81F
#define LCD_YELLOW      0xFFE0
#define LCD_GRAY        0x8410
#define LCD_LIGHT_GRAY  0xC618

/* 屏幕尺寸 - TF20QN003-10 */
#define LCD_WIDTH       240
#define LCD_HEIGHT      320

/* 函数声明 */
int nv3030b_lcd_init(void);
void lcd_clear(rt_uint16_t color);
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);
void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color);
void lcd_draw_rectangle(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, rt_uint16_t color);
void lcd_fill_rectangle(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, rt_uint16_t color);
void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint16_t r, rt_uint16_t color);
void lcd_draw_char(rt_uint16_t x, rt_uint16_t y, char chr, rt_uint16_t color, rt_uint16_t bg_color);
void lcd_draw_string(rt_uint16_t x, rt_uint16_t y, const char *str, rt_uint16_t color, rt_uint16_t bg_color);
void lcd_draw_qrcode(const char *ssid, const char *password);
void lcd_draw_qrcode_module(rt_uint16_t x, rt_uint16_t y, rt_uint8_t size, rt_bool_t black);
void lcd_show_status(const char *status);
void lcd_set_window(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height);
void lcd_backlight_on(void);
void lcd_backlight_off(void);

#endif /* __DRV_NV3030B_LCD_H__ */
