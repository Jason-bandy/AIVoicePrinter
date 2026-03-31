# BK7252N LCD 引脚配置参考

## 📌 BK7252N GPIO 概述

BK7252N 芯片共有 40 个 GPIO 引脚（GPIO0 - GPIO39），支持多种复用功能。

---

## 🔧 ST7789 LCD 推荐引脚配置

### 方案 A：使用 SPI0（推荐）

| LCD 引脚 | BK7252N GPIO | 功能 | 说明 |
|---------|-------------|------|------|
| CS      | GPIO6       | SPI0 CS | 片选，低电平有效 |
| CLK     | GPIO7       | SPI0 CLK | 时钟 |
| MOSI    | GPIO8       | SPI0 MOSI | 数据输出 |
| DC      | GPIO9       | 普通 GPIO | 数据/命令选择 |
| RST     | GPIO10      | 普通 GPIO | 复位，低电平有效 |
| VCC     | 3.3V        | 电源 | 3.3V |
| GND     | GND         | 地 | GND |
| BLK     | GPIO11      | 可选 | 背光控制（PWM） |

**优点：**
- ✅ 使用硬件 SPI，速度快（最高 20-40MHz）
- ✅ 引脚连续，布线方便
- ✅ 不与其他功能冲突

---

### 方案 B：使用 SPI1

| LCD 引脚 | BK7252N GPIO | 功能 | 说明 |
|---------|-------------|------|------|
| CS      | GPIO14      | SPI1 CS | 片选 |
| CLK     | GPIO15      | SPI1 CLK | 时钟 |
| MOSI    | GPIO16      | SPI1 MOSI | 数据输出 |
| DC      | GPIO17      | 普通 GPIO | 数据/命令 |
| RST     | GPIO18      | 普通 GPIO | 复位 |

**适用场景：**
- SPI0 被其他设备占用时

---

### 方案 C：软件模拟 SPI（不推荐）

可以使用任意 GPIO 引脚模拟 SPI，但速度较慢。

| LCD 引脚 | BK7252N GPIO | 说明 |
|---------|-------------|------|
| CS      | GPIO0       | 任意 GPIO |
| CLK     | GPIO1       | 任意 GPIO |
| MOSI    | GPIO2       | 任意 GPIO |
| DC      | GPIO3       | 任意 GPIO |
| RST     | GPIO4       | 任意 GPIO |

**缺点：**
- ❌ 速度慢（通常 < 5MHz）
- ❌ 占用 CPU 资源

---

## ⚠️ 引脚选择注意事项

### 避免使用的引脚

| GPIO | 原因 | 说明 |
|------|------|------|
| GPIO0 | 启动引脚 | 可能影响启动 |
| GPIO1 | 启动引脚 | 可能影响启动 |
| GPIO32-39 | 特殊功能 | 可能用于 Flash、USB 等 |
| GPIO28-31 | 保留 | 不建议使用 |

### 推荐使用的引脚

- **普通 GPIO**: GPIO6-GPIO27
- **SPI0**: GPIO6-GPIO10
- **SPI1**: GPIO14-GPIO18
- **I2C**: GPIO20-GPIO23
- **UART**: GPIO1-GPIO4

---

## 🔌 硬件连接示例

### 原理图连接

```
        BK7252N              ST7789 LCD (240x240)
        ┌─────────┐         ┌───────────┐
   GPIO6│         │─────────│ CS        │
   GPIO7│         │─────────│ CLK       │
   GPIO8│         │─────────│ MOSI/SDA  │
   GPIO9│         │─────────│ DC        │
  GPIO10│         │─────────│ RST       │
        │         │         │           │
  3.3V ┤         ├─────────│ VCC       │
  GND  ┤         ├─────────│ GND       │
        │         │         │           │
        │         │         │  240x240  │
        │         │         │   Color   │
        │         │         │           │
        └─────────┘         └───────────┘
```

### PCB 布局建议

1. **走线长度**：SPI 信号线尽量短（< 10cm）
2. **阻抗匹配**：时钟和数据线保持等长
3. **电源去耦**：LCD VCC 就近放置 0.1uF 电容
4. **地平面**：完整的地平面减少干扰

---

## 🛠️ 软件配置

### 修改引脚定义

编辑 `drv_st7789_lcd.c`：

```c
/* 根据实际硬件连接修改 */
#define LCD_CS_PIN    GPIO6    /* 片选 */
#define LCD_DC_PIN    GPIO9    /* 数据/命令 */
#define LCD_RST_PIN   GPIO10   /* 复位 */

/* SPI 设备名称 */
#define ST7789_SPI_DEVICE  "spi0"
```

### 测试引脚是否正常

```bash
# MSH 命令测试
gpio_test 6 output 1  # 测试 CS 引脚
gpio_test 9 output 1  # 测试 DC 引脚
gpio_test 10 output 1 # 测试 RST 引脚
```

---

## 📊 GPIO 复用功能表（BK7252N）

| GPIO | 主要功能 | 可用作 |
|------|---------|--------|
| 0    | UART1_TX | ⚠️ 启动引脚 |
| 1    | UART1_RX | ⚠️ 启动引脚 |
| 2    | UART2_TX | ✅ 普通 GPIO |
| 3    | UART2_RX | ✅ 普通 GPIO |
| 4    | I2C1_SDA | ✅ 普通 GPIO |
| 5    | I2C1_SCL | ✅ 普通 GPIO |
| 6    | SPI0_CS  | ✅ **推荐 LCD CS** |
| 7    | SPI0_CLK | ✅ **推荐 LCD CLK** |
| 8    | SPI0_MOSI| ✅ **推荐 LCD MOSI** |
| 9    | SPI0_MISO| ✅ **推荐 LCD DC** |
| 10   | PWM0     | ✅ **推荐 LCD RST** |
| 11   | PWM1     | ✅ LCD 背光 |
| 12   | PWM2     | ✅ 普通 GPIO |
| 13   | PWM3     | ✅ 普通 GPIO |
| 14   | SPI1_CS  | ✅ 备用 SPI |
| 15   | SPI1_CLK | ✅ 备用 SPI |
| 16   | SPI1_MOSI| ✅ 备用 SPI |
| 17   | SPI1_MISO| ✅ 备用 SPI |
| 18-27| 多功能   | ✅ 普通 GPIO |
| 28-31| 保留     | ⚠️ 不建议 |
| 32-39| 特殊     | ⚠️ Flash/USB |

---

## 🔍 故障排查

### 问题 1：LCD 不显示

**检查：**
1. VCC 电压是否为 3.3V
2. CS 引脚是否正确连接
3. RST 引脚是否正确复位
4. SPI 时钟频率是否过高

**解决：**
```c
/* 降低 SPI 速度测试 */
cfg.max_hz = 10 * 1000 * 1000;  /* 10MHz */
```

### 问题 2：显示花屏

**检查：**
1. MOSI 信号线是否接触良好
2. 地线是否连接可靠
3. 显示方向设置是否正确

**解决：**
```c
/* 调整显示方向 */
lcd_write_cmd(0x36);
lcd_write_data(0x00);  /* 尝试不同值 */
```

### 问题 3：颜色不对

**检查：**
1. RGB 顺序是否正确
2. 是否启用了显示反转

**解决：**
```c
/* 切换显示反转 */
lcd_write_cmd(0x21);  /* 启用反转 */
/* 或 */
lcd_write_cmd(0x20);  /* 禁用反转 */
```

---

## 📚 参考资料

- BK7252N 数据手册：`docs/BK7252N 数据手册_V1.0.pdf`
- BK7252N RTOS SDK API Reference
- ST7789 数据手册
- RT-Thread SPI 驱动文档

---

**更新日期**: 2026-03-29  
**适用芯片**: BK7252N  
**驱动版本**: ST7789 1.0.0
