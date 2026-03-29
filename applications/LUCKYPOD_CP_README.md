# LuckyPod Captive Portal 配网方案

## 📋 功能概述

实现语音式 AI 打印机的 WiFi 配网功能，使用 Captive Portal（强制门户）技术，类似路由器/酒店 WiFi 的配网体验。

### 完整用户体验流程

```
1. 设备启动 → 进入 AP 模式
   ↓
2. 屏幕显示 QR 码（SSID + 密码）
   ↓
3. 用户手机扫码 → 自动连接 LuckyPod 热点
   ↓
4. 手机自动弹出配网页面（Captive Portal）
   ↓
5. 用户选择家里 WiFi + 输入密码
   ↓
6. 设备保存配置并连接
   ↓
7. 屏幕显示"配网成功"
   ↓
8. 设备重启进入 STA 模式，正常工作
```

---

## 🛠️ 实现组件

### 1. SoftAP + DHCP Server
- **模式**: `BK_SOFT_AP`
- **SSID**: `LuckyPod-XX` (基于 MAC 地址)
- **密码**: `luckypod2026`
- **频道**: 6
- **最大连接数**: 4

### 2. HTTP Server
- **框架**: WebNet (RT-Thread 内置)
- **端口**: 80
- **根目录**: `/`

### 3. Captive Portal 探测响应
拦截并响应各大操作系统的网络探测请求：

| 操作系统 | 探测 URL | 响应 |
|---------|---------|------|
| iOS/macOS | `/library/test/success.html` | 200 OK + HTML |
| Android | `/generate_204` | 204 No Content |
| Windows | `/ncsi.txt` | 200 OK |
| Amazon | `/kindle-wifi/wifisignincheck.json` | JSON |

### 4. HTML 配网表单
- **路径**: `/cgi-bin/config`
- **功能**:
  - 扫描并显示 WiFi 列表
  - 输入 WiFi 密码
  - 提交连接请求
  - 显示连接状态

### 5. EasyFlash 保存凭据
```c
ef_set_env("wifi_ssid", ssid);
ef_set_env("wifi_pass", password);
ef_set_env("dev_mode", "sta");
```

### 6. ST7789 显示屏 (240×240)
- **接口**: SPI
- **显示内容**:
  - QR 码（WiFi 连接信息）
  - 配网状态文字
  - 连接进度

---

## 📁 文件结构

```
applications/
├── luckypod_captive_portal.c    # 主程序
├── luckypod_webroot/
│   └── index.html               # 配网页面 HTML
└── LUCKYPOD_CP_README.md        # 本文档
```

---

## 🔧 配置说明

### 硬件 GPIO 配置（根据实际修改）

```c
#define LCD_CS_GPIO   GPIO_PB0
#define LCD_CLK_GPIO  GPIO_PB1
#define LCD_MOSI_GPIO GPIO_PB2
#define LCD_DC_GPIO   GPIO_PB3
#define LCD_RST_GPIO  GPIO_PB4
```

### WiFi AP 配置

```c
#define LUCKYPOD_AP_SSID_PREFIX   "LuckyPod-"
#define LUCKYPOD_AP_PASSWORD      "luckypod2026"
#define LUCKYPOD_AP_CHANNEL       6
#define LUCKYPOD_AP_MAX_CONN      4
```

### EasyFlash 配置键

```c
#define EF_KEY_WIFI_SSID    "wifi_ssid"
#define EF_KEY_WIFI_PASSWORD "wifi_pass"
#define EF_KEY_DEVICE_MODE  "dev_mode"
```

---

## 🚀 使用方法

### 1. 编译固件

在 SDK 根目录执行：
```bash
scons
```

### 2. 烧录固件

使用 BK7252N 烧录工具烧录生成的固件。

### 3. 启动设备

设备上电后自动进入 AP 配网模式。

### 4. 手机配网

#### 方式 A: 扫码配网（推荐）
1. 打开微信/相机扫描设备屏幕上的 QR 码
2. 点击"加入网络"
3. 自动弹出配网页面
4. 选择 WiFi + 输入密码
5. 等待连接成功

#### 方式 B: 手动配网
1. 打开 WiFi 设置
2. 连接 `LuckyPod-XX` 热点（密码：`luckypod2026`）
3. 打开浏览器访问 `http://192.168.4.1`
4. 后续步骤同上

### 5. 配网完成

设备显示"配网成功"，自动重启进入 STA 模式，开始正常工作。

---

## 🌐 API 接口

### GET `/cgi-bin/get_wifi_list`

获取周围 WiFi 列表

**响应:**
```json
{
  "action": "getWifiList",
  "status": "success",
  "data": [
    {
      "ssid": "Xiaomi_402",
      "rssi": -65,
      "security": 1
    },
    {
      "ssid": "XMLJ",
      "rssi": -70,
      "security": 1
    }
  ]
}
```

### GET `/cgi-bin/connect_wifi?ssid=xxx&password=xxx`

连接到指定 WiFi

**请求参数:**
- `ssid`: WiFi 名称
- `password`: WiFi 密码

**响应:**
```json
{
  "status": "success",
  "message": "Connecting..."
}
```

### GET `/cgi-bin/get_status`

获取设备状态

**响应:**
```json
{
  "action": "getStatus",
  "status": "success",
  "data": {
    "mode": 0,              // 0=AP, 1=STA, 2=CONNECTING
    "ap_ssid": "LuckyPod-6C",
    "sta_ssid": "Xiaomi_402",
    "device_id": "luckypod-001",
    "version": "1.0.0"
  }
}
```

### GET `/cgi-bin/config`

配网页面（HTML）

---

## 🔍 调试方法

### MSH 命令

```bash
# 手动启动配网模式
luckypod_captive_portal_init

# 查看 WiFi 状态
wlan_status

# 扫描 WiFi
wlan_scan

# 查看 EasyFlash 配置
ef_print_env
```

### 日志输出

设备通过 UART 输出调试日志：
```
[LCD] ST7789 初始化 (240x240)
[AP] 启动热点：LuckyPod-6C (密码：luckypod2026)
[HTTP] WebNet 启动 (端口 80)
[WiFi] 开始扫描...
[WiFi] 扫描完成，发现 5 个网络
[HTTP] 连接请求：SSID=Xiaomi_402
[WiFi] 连接成功！
```

---

## ⚠️ 注意事项

### 1. QR 码生成
当前代码使用文字代替 QR 码，实际部署需要集成 QR 码生成库：
- 推荐：https://github.com/nayuki/QR-Code-generator
- 格式：`WIFI:T:WPA;S:{ssid};P:{password};;`

### 2. ST7789 驱动
需要根据实际硬件实现 LCD 驱动函数：
- `lcd_init()` - 初始化
- `lcd_clear()` - 清屏
- `lcd_draw_qrcode()` - 绘制 QR 码
- `lcd_show_status()` - 显示状态

### 3. 安全考虑
- AP 密码建议改为动态生成（基于设备 MAC）
- 配网页面建议添加 HTTPS 支持
- WiFi 密码在传输过程中建议加密

### 4. 功耗优化
- AP 模式功耗较高，配网成功后应立即切换到 STA 模式
- 可添加超时机制（如 5 分钟未配网自动关机）

---

## 📊 状态机

```
┌─────────────┐
│   Boot      │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  AP Mode    │ ← 显示 QR 码，等待连接
│ (配网中)    │
└──────┬──────┘
       │ 用户提交 WiFi 配置
       ▼
┌─────────────┐
│ Connecting  │ ← 尝试连接 WiFi
│  (连接中)   │
└──────┬──────┘
       │ 成功/失败
       ▼
┌─────────────┐     失败      ┌─────────────┐
│  STA Mode   │ ←─────────────│  重试/超时  │
│ (正常工作)  │               │  返回 AP    │
└─────────────┘               └─────────────┘
```

---

## 🎯 下一步优化

1. **QR 码生成**: 集成 QR 库，显示真实 WiFi QR 码
2. **蓝牙辅助配网**: 同时支持 BLE 配网作为备选
3. **声音提示**: 添加蜂鸣器提示音（扫码成功、连接成功）
4. **多语言支持**: 配网页面支持中/英/日等多语言
5. **设备绑定**: 配网时同时完成设备云端绑定
6. **信号强度显示**: 在配网页面显示 WiFi 信号强度图标

---

## 📞 技术支持

遇到问题请查看：
- RT-Thread 文档：https://www.rt-thread.io/docs/
- WebNet 组件文档：https://www.rt-thread.io/docs/api/group__webnet.html
- EasyFlash 文档：https://github.com/armink/EasyFlash

---

**版本**: 1.0.0  
**日期**: 2026-03-29  
**作者**: LuckyPod Team
