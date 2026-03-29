# ST7789 LCD 驱动程序

## 📋 概述

ST7789 240x240 彩色 LCD 显示屏驱动，基于 SPI 接口，适用于 BK7252N 平台。

**特点：**
- ✅ 240x240 分辨率
- ✅ SPI 接口（4 线制）
- ✅ 支持 RGB565 颜色格式
- ✅ 无需触摸屏
- ✅ 支持文字、图形、QR 码显示

---

## 🔧 硬件连接

### GPIO 配置（根据实际硬件修改）

| 引脚 | BK7252N GPIO | 功能 | 说明 |
|------|-------------|------|------|
| CS   | GPIO_PB0    | 片选 | 低电平有效 |
| CLK  | GPIO_PB1    | 时钟 | SPI CLK |
| MOSI | GPIO_PB2    | 数据输出 | SPI MOSI |
| DC   | GPIO_PB3    | 数据/命令选择 | 高=数据，低=命令 |
| RST  | GPIO_PB4    | 复位 | 低电平复位 |
| VCC  | 3.3V        | 电源 | 3.3V |
| GND  | GND         | 地 | GND |
| BLK  | GPIO_XX     | 背光 | 可选，PWM 调光 |

### 原理图符号

```
        BK7252N              ST7789 LCD
        ┌──────┐            ┌─────────┐
   PB0  │      │────────────│ CS      │
   PB1  │      │────────────│ CLK     │
   PB2  │      │────────────│ MOSI    │
   PB3  │      │────────────│ DC      │
   PB4  │      │────────────│ RST     │
        │      │            │         │
  3.3V ┤      ├────────────│ VCC     │
  GND  ┤      ├────────────│ GND     │
        │      │            │         │
        │      │            │  240x240│
        │      │            │  Color  │
        │      │            │         │
        └──────┘            └─────────┘
```

---

## 🚀 使用方法

### 1. 启用驱动

在 `menuconfig` 中启用：

```bash
# 进入 SDK 根目录
cd /Users/zhengzhican/Jobs/智伽/coding/beken_rtt_sdk_release_v3.0.78

# 打开配置界面
scons --menuconfig

# 导航到：
Drivers Config  --->
    [*] Using SPI Master  --->
    [*] Using ST7789 LCD Driver
```

或手动修改 `drivers/Kconfig`：
```
config BEKEN_USING_ST7789_LCD
    bool "Using ST7789 LCD Driver"
    default y
```

### 2. 编译固件

```bash
scons
```

### 3. 初始化 LCD

驱动会自动初始化（`INIT_DEVICE_EXPORT`），也可以手动初始化：

```c
#include "drv_st7789_lcd.h"

/* 手动初始化 */
st7789_lcd_init();
```

### 4. 使用示例

```c
#include "drv_st7789_lcd.h"

/* 清屏 */
lcd_clear(LCD_WHITE);

/* 显示文字 */
lcd_draw_string(10, 10, "Hello LuckyPod!", LCD_BLACK, LCD_WHITE);

/* 画图形 */
lcd_draw_rectangle(20, 40, 100, 100, LCD_RED);
lcd_draw_circle(150, 100, 50, LCD_BLUE);

/* 显示 WiFi QR 码 */
lcd_draw_qrcode("LuckyPod-6C", "luckypod2026");

/* 显示状态 */
lcd_show_status("配网中...");
```

---

## 📚 API 文档

### 基础函数

#### `int st7789_lcd_init(void)`
初始化 LCD 显示屏
- **返回值**: `RT_EOK` 成功，`-RT_ERROR` 失败

#### `void lcd_clear(rt_uint16_t color)`
清屏为指定颜色
- **参数**: `color` - RGB565 颜色值

#### `void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)`
画点
- **参数**: 
  - `x`, `y` - 坐标
  - `color` - 颜色

#### `void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color)`
画线（Bresenham 算法）
- **参数**: 起点、终点坐标和颜色

#### `void lcd_draw_rectangle(...)` 
画矩形（空心）

#### `void lcd_fill_rectangle(...)`
填充矩形（实心）

#### `void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint16_t r, rt_uint16_t color)`
画圆

### 文字显示

#### `void lcd_draw_char(rt_uint16_t x, rt_uint16_t y, char chr, rt_uint16_t color, rt_uint16_t bg_color)`
显示单个字符（5x7 点阵）
- **支持**: 数字 0-9，部分大写字母

#### `void lcd_draw_string(rt_uint16_t x, rt_uint16_t y, const char *str, rt_uint16_t color, rt_uint16_t bg_color)`
显示字符串

### QR 码显示

#### `void lcd_draw_qrcode(const char *ssid, const char *password)`
显示 WiFi QR 码
- **参数**:
  - `ssid` - WiFi 名称
  - `password` - WiFi 密码
- **格式**: `WIFI:T:WPA;S:{ssid};P:{password};;`

#### `void lcd_draw_qrcode_module(rt_uint16_t x, rt_uint16_t y, rt_uint8_t size, rt_bool_t black)`
绘制 QR 码单个模块（像素块）

### 状态显示

#### `void lcd_show_status(const char *status)`
显示状态页面（包含 Logo 和状态文字）

---

## 🎨 颜色参考

RGB565 格式颜色值：

| 颜色 | 名称 | RGB565 值 | 十六进制 |
|------|------|----------|---------|
| ⚫ 黑色 | BLACK | 0x0000 | `#000000` |
| ⚪ 白色 | WHITE | 0xFFFF | `#FFFFFF` |
| 🔴 红色 | RED | 0xF800 | `#FF0000` |
| 🟢 绿色 | GREEN | 0x07E0 | `#00FF00` |
| 🔵 蓝色 | BLUE | 0x001F | `#0000FF` |
| 🟡 黄色 | YELLOW | 0xFFE0 | `#FFFF00` |
| 🟣 品红 | MAGENTA | 0xF81F | `#FF00FF` |
| 🔷 青色 | CYAN | 0x07FF | `#00FFFF` |
| ⚪ 灰 | GRAY | 0x8410 | `#808080` |

---

## 🔍 调试方法

### MSH 命令

```bash
# 初始化 LCD
st7789_lcd_init

# 运行测试
lcd_test
```

### 日志输出

启用调试日志（修改 `drv_st7789_lcd.c`）：

```c
#define DBG_ENABLE
#define DBG_SECTION_NAME  "[LCD]:"
#define DBG_LEVEL         DBG_LOG
#include <rtdbg.h>
```

---

## ⚠️ 注意事项

### 1. GPIO 配置
根据实际硬件连接修改 `drv_st7789_lcd.c` 中的 GPIO 定义：

```c
#define LCD_CS_PIN    GPIO_PB0   /* 修改为实际 GPIO */
#define LCD_DC_PIN    GPIO_PB3
#define LCD_RST_PIN   GPIO_PB4
```

### 2. SPI 速度
默认 SPI 速度为 20MHz，如果显示异常可降低：

```c
cfg.max_hz = 10 * 1000 * 1000;  /* 降为 10MHz */
```

### 3. 显示方向
如果显示方向不对，修改初始化序列中的 `0x36` 命令：

```c
lcd_write_cmd(0x36);
lcd_write_data(0x00);  /* 尝试：0x00, 0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0 */
```

### 4. 颜色反转
如果颜色显示反了，修改：

```c
/* 启用显示反转 */
lcd_write_cmd(0x21);   /* 改为 0x20 禁用反转 */
```

### 5. 背光控制
如果需要控制背光，添加 GPIO 控制：

```c
#define LCD_BL_PIN    GPIO_PB5

/* 初始化背光 */
gpio_set_pin_function(LCD_BL_PIN, GPIO_FUNC_GPIO);
gpio_set_pin_direction(LCD_BL_PIN, GPIO_OUTPUT);
gpio_set_pin_value(LCD_BL_PIN, GPIO_HIGH);  /* 开启背光 */
```

---

## 📊 性能优化

### 1. 使用 DMA（如果 SDK 支持）
```c
/* TODO: 集成 DMA 传输 */
```

### 2. 批量传输
当前驱动已实现批量 SPI 传输，避免单字节传输开销。

### 3. 局部刷新
使用 `lcd_set_window()` 只更新需要刷新的区域。

---

## 🔗 相关资源

- ST7789 数据手册：https://www.displaytech.com.hk/sites/default/files/ST7789V.pdf
- RT-Thread SPI 文档：https://www.rt-thread.io/docs/api/group__spi.html
- RGB565 颜色计算器：http://www.barth-dev.de/online/rgb565-color-picker/

---

## 📝 更新日志

- **v1.0.0** (2026-03-29)
  - 初始版本
  - 支持基础图形绘制
  - 支持文字显示（数字）
  - 支持 QR 码显示框架
  - 集成到 LuckyPod 配网系统

---

**作者**: LuckyPod Team  
**日期**: 2026-03-29  
**版本**: 1.0.0
