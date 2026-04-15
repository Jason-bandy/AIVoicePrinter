# ST7789 LCD 软件模拟 SPI 驱动说明

## 概述

本驱动使用**软件模拟 SPI**方式驱动 ST7789 320x240 LCD 显示屏，可以直接使用 BK7252N 开发板的 VIDEO IF 排针，无需飞线。

## 硬件连接

### LCD 模组引脚定义（12 Pin）

| 序号 | 引脚 | 类型 | 说明 |
|------|------|------|------|
| 1 | SDA | I/O | SPI 数据输入/输出 |
| 2 | SCL | I | SPI 时钟 |
| 3 | RESET | I | 复位（低电平有效）|
| 4 | RS/A0 | I | 寄存器选择（DC）|
| 5 | CS | I | 片选（低电平有效）|
| 6 | GND | - | 模拟地 |
| 7 | VDD | P | 电源（2.8V，可接 3.3V）|
| 8-10 | NC | - | 空脚 |
| 11 | K- | P | LED 背光阴极 |
| 12 | A+ | P | LED 背光阳极 |

### VIDEO IF 排针引脚定义

```
VIDEO IF (底部 16 针排针)
┌─────────────────────────────────────────────────────────┐
│ HSYNC/P30   D1/P29    SYNC/P34   TX1/P22               │
│ D2/P34      D3/P35    D4/P36     RX1/P23               │
│ D5/P37      D6/P38    D7/P39     GND                   │
│ VBAT        24V       NC         NC                    │
└─────────────────────────────────────────────────────────┘
```

### LCD 到 BK7252N 连接表

| LCD 引脚 | 功能 | BK7252N | VIDEO IF 位置 | 备注 |
|---------|------|---------|--------------|------|
| 1 SDA | SPI 数据 | GPIO38 | D6/P38 | MOSI |
| 2 SCL | SPI 时钟 | GPIO37 | D5/P37 | CLK |
| 3 RESET | 复位 | GPIO34 | D2/P34 | 低电平有效 |
| 4 RS/A0 | DC | GPIO35 | D3/P35 | 数据/命令 |
| 5 CS | 片选 | GPIO36 | D4/P36 | 低电平有效 |
| 6 GND | 地 | GND | 第 3 行右侧 | - |
| 7 VDD | 电源 | VBAT | 第 3 行左侧 | 建议串 10Ω 电阻 |
| 11 K- | LED 阴极 | GND | - | 与 GND 共地 |
| 12 A+ | LED 阳极 | VBAT | - | 串 22-47Ω 电阻 |

> **注意**：
> 1. LCD VDD 标称 2.8V，BK7252N 的 VBAT 是 3.3V，建议串联 10Ω 电阻限流
> 2. 背光 A+ 必须串联 22-47Ω 电阻，否则电流过大可能损坏 LED
> 3. GND、K-、LCD GND 需要共地

## 软件模拟 SPI 原理

软件模拟 SPI 通过 GPIO 翻转来模拟 SPI 时序：

```
SPI Mode 0 (CPOL=0, CPHA=0):

    CLK: ──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──
           └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘
              ↑    ↑    ↑    ↑    ↑    ↑    ↑    ↑
    MOSI:  [D7] [D6] [D5] [D4] [D3] [D2] [D1] [D0]
    
    数据在时钟上升沿被锁存
```

### 时序参数

- 当前延迟设置：约 2-3 MHz SPI 速度
- 可通过调整 `spi_delay()` 函数中的循环次数来改变速度
- 速度越快，显示刷新率越高，但信号质量可能下降

## API 接口

### 初始化

```c
int st7789_lcd_init(void);
```
初始化 LCD，自动调用后可直接使用。

### 基础操作

```c
void lcd_clear(rt_uint16_t color);           // 清屏
void lcd_draw_point(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);  // 画点
void lcd_draw_line(rt_uint16_t x1, rt_uint16_t y1, 
                   rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color); // 画线
void lcd_draw_rectangle(rt_uint16_t x, rt_uint16_t y, 
                        rt_uint16_t width, rt_uint16_t height, 
                        rt_uint16_t color);  // 画矩形
void lcd_fill_rectangle(rt_uint16_t x, rt_uint16_t y, 
                        rt_uint16_t width, rt_uint16_t height, 
                        rt_uint16_t color);  // 填充矩形
void lcd_draw_circle(rt_uint16_t x0, rt_uint16_t y0, 
                     rt_uint16_t r, rt_uint16_t color);  // 画圆
```

### 文字显示

```c
void lcd_draw_char(rt_uint16_t x, rt_uint16_t y, char chr, 
                   rt_uint16_t color, rt_uint16_t bg_color);  // 画字符
void lcd_draw_string(rt_uint16_t x, rt_uint16_t y, const char *str, 
                     rt_uint16_t color, rt_uint16_t bg_color); // 画字符串
```

### 背光控制

```c
void lcd_backlight_on(void);      // 开启背光
void lcd_backlight_off(void);     // 关闭背光
void lcd_backlight_set(rt_uint8_t level);  // 设置亮度 (0-100)
```

### 状态显示

```c
void lcd_show_status(const char *status);   // 显示状态信息
void lcd_draw_qrcode(const char *ssid, const char *password);  // 显示 WiFi QR 码
```

## MSH 命令

在 msh 中可以使用以下命令：

```bash
# 初始化 LCD
st7789_lcd_init

# LCD 测试
lcd_test

# 背光控制
lcd_bl_on
lcd_bl_off
```

## 屏幕参数

- 分辨率：**320x240**
- 驱动 IC：**ST7789**
- 颜色格式：**RGB565** (16 位)
- 显示方向：横向（可调整）

## 颜色定义

驱动已预定义常用颜色（RGB565 格式）：

```c
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define GRAY        0x8410
#define LIGHT_GRAY  0xC618
```

## 电源连接

**重要**：LCD 需要 2 个电源连接：

| 电源 | LCD 引脚 | 用途 | 接法 |
|-----|---------|------|------|
| VDD | Pin 7 | 显示屏逻辑电源 | 串联 10Ω 电阻后接 VBAT (3.3V) |
| LED_A | Pin 12 | 背光阳极 | 串联 22-47Ω 电阻后接 VBAT |
| GND | Pin 6, Pin 11 | 地 | 接 GND（共地）|

### 背光限流电阻计算

```
R = (Vcc - Vf) / If

其中：
- Vcc = 3.3V (供电电压)
- Vf ≈ 3.0V (LED 正向压降，白色背光)
- If = 20mA (期望电流)

R = (3.3 - 3.0) / 0.02 = 15Ω

建议使用 22-47Ω 电阻，亮度适中且安全
```

### VDD 限流电阻

LCD VDD 标称 2.8V，接 3.3V 时需要限流：
```
R = (3.3V - 2.8V) / Icc
建议取值：10-22Ω
```

### 接线示例

```
BK7252N VBAT ────────┬──────── 10Ω ──────── LCD Pin 7 (VDD)
                     │
                     ├─────── 22Ω 电阻 ───── LCD Pin 12 (LED_A)
                     │
BK7252N GND ─────────┴───────────────────── LCD Pin 6 (GND) + Pin 11 (K-)
```

## 性能优化建议

### 当前性能
- SPI 速度：约 2-3 MHz
- 清屏时间：约 100-200 ms
- 适合显示静态内容、文字、图标

### 如需提升速度

1. **调整延迟**：减小 `spi_delay()` 中的循环次数
   ```c
   static void spi_delay(void)
   {
       // 减少循环次数可以提升速度
       // 但太快可能导致显示不稳定
   }
   ```

2. **使用硬件 SPI**（需要飞线）：
   - 使用 GPIO6-8 (SPI0) 或 GPIO14-16 (SPI1)
   - 速度可达 20-40 MHz
   - 参考 [LCD_PINOUT_REFERENCE.md](LCD_PINOUT_REFERENCE.md)

## 故障排查

### 问题 1：LCD 不显示

**检查：**
1. Pin 7 (VDD) 电压是否为 2.8-3.3V
2. Pin 6、Pin 11 (GND) 是否连接可靠
3. Pin 5 (CS)、Pin 4 (RS)、Pin 3 (RESET) 引脚连接是否正确
4. 背光是否亮起（判断是否供电正常）

**解决：**
```bash
# 在 msh 中测试引脚
gpio_test 36 output 1  # 测试 CS (Pin 5)
gpio_test 37 output 1  # 测试 CLK (Pin 2)
gpio_test 38 output 1  # 测试 MOSI (Pin 1)
gpio_test 35 output 1  # 测试 DC (Pin 4)
gpio_test 34 output 1  # 测试 RST (Pin 3)
```

### 问题 2：显示花屏或有噪点

**检查：**
1. 连线是否过长（建议 < 10cm）
2. 信号线是否交叉干扰
3. 地线连接是否可靠
4. VDD 电压是否稳定

**解决：**
- 降低 SPI 速度（增加 `spi_delay()` 循环次数）
- 检查连线顺序是否正确
- 确保 GND 连接良好

### 问题 3：颜色不对

**检查：**
- RGB 顺序是否正确
- 是否启用了显示反转

**解决：**
```c
// 在初始化序列中调整 MADCTL (0x36)
// 如果颜色反转：
lcd_write_cmd(0x21);  // 启用反转
// 或
lcd_write_cmd(0x20);  // 禁用反转

// 如果 RGB 顺序不对，修改 0x36 命令：
lcd_write_cmd(0x36);
lcd_write_data(0x00);  // 改为 RGB 顺序
```

### 问题 4：显示方向不对

**解决：**
修改 MADCTL 寄存器 (0x36) 的值：
```c
// 尝试不同的值来调整方向
lcd_write_cmd(0x36);
lcd_write_data(0x00);  // 0°
// lcd_write_data(0x60);  // 90°
// lcd_write_data(0xC0);  // 180°
// lcd_write_data(0xA0);  // 270°
```

## 参考资料

- BK7252N 数据手册
- ST7789 数据手册
- RT-Thread GPIO 驱动文档

---

**更新日期**: 2026-04-15  
**驱动版本**: ST7789 Soft-SPI 1.0  
**屏幕规格**: 320x240 IPS, 12 Pin 接口, ST7789 驱动 IC
