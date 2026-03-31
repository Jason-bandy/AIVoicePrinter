# AI打印机

**AI 打印机** 的两种典型使用模式：

1.  **语音 → 文字 → 打印文字标签**（如说“盐巴” → 打印「盐巴」）
    

*    快速、实用，适合家庭物品管理 
    
*   取代原手机复杂操作流程
    

1.  **语音 → 文字 → 文生图 → 打印图像标签**（如说“小恐龙” → 生成恐龙图 → 打印）
    

*   有趣、互动强，适合儿童教育娱乐
    
*   延迟较高（3-8秒，取决于图像生成速度）
    
*   后端可加入风格控制（如“卡通风格”等）
    

![/Users/zhengzhican/Downloads/mermaid-diagram-2026-01-06-102857.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/3BMqYybKAwAJdqwZ/img/0612291c-f84b-4bb2-836e-06bd9185cd18.png?x-oss-process=image/crop,x_0,y_311,w_2298,h_759/ignore-error,1)

| 运行环节 | 参数 |
| --- | --- |
| 打印图像宽度 | 464 pixels（对应 58mm @ 203 DPI） |
| ESC/POS 每行字节数 | 58 bytes（464 ÷ 8） |
| 预览图宽度 | 128 pixels（仅用于 LCD 显示，与打印无关） |
| 预览图格式 | 2-bit 灰度 RAW（4 KB） |
| 打印图格式 | 1-bit 黑白 ESC/POS 字节流 |

云端需将图像 转换为 ESC/POS 点阵格式，每行 464 bits = 58 bytes（因为 464 ÷ 8 = 58）

```plaintext
// GS v 0 m xL xH yL yH d1...dk
0x1D, 0x76, 0x30,   // GS v 0
0x00,               // m = 0 (normal mode)
58, 0,              // xL=58, xH=0 → width = 58 bytes = 464 dots
100, 0,             // yL=100, yH=0 → height = 100 dots
[...464×100/8 = 5800 bytes of image data...]
```

比如，发送一段 ESC/POS 指令：

```plaintext
1D 76 30 00 3A 00 01 00 FF FF FF ... (58 bytes of 0xFF)
```

设计：

*   打印端：充分利用 58mm 纸宽，输出清晰标签
    
*   预览端：在小屏上快速显示可识别内容，节省资源
    

---

### 模式一：语音转文字标签打印

![/Users/zhengzhican/Downloads/mermaid-diagram-2026-01-06-101937.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/3BMqYybKAwAJdqwZ/img/ef86fd71-b126-4226-9ebc-a7c09cbf9372.png?x-oss-process=image/crop,x_135,y_0,w_2031,h_1482/ignore-error,1)

---

### 模式二：语音转图文标签打印

![/Users/zhengzhican/Downloads/mermaid-diagram-2026-01-06-101937.png](https://alidocs.oss-cn-zhangjiakou.aliyuncs.com/res/3BMqYybKAwAJdqwZ/img/a43b8363-0066-4d39-841c-6bbd695d7030.png?x-oss-process=image/crop,x_138,y_0,w_2015,h_1482/ignore-error,1)

---

## 通信协议设计

设备连接 WiFi 后，涉及两类数据流，采用不同协议：

| 数据流 | 协议 | 原因 |
| --- | --- | --- |
| 语音数据 → 云端识别 | **WebSocket** | 实时流式传输，全双工，低延迟 |
| 云端 → 设备远程打印 | **MQTT** | 设备在 NAT 后，broker 中转解决入站问题，QoS 保证送达 |
| 设备状态上报 | **MQTT** | 复用同一 MQTT 连接 |

### 为什么远程打印用 MQTT 而非 WebSocket

设备连接 WiFi 后处于内网（NAT 后），Java 后台在云端**无法直接连接**设备。解决方案对比：

| 方案 | 可行性 | 问题 |
| --- | --- | --- |
| Java 后台 HTTP push 到设备 | ❌ | 设备无公网 IP，局域网才能用 |
| 设备做 WebSocket Server | ❌ | 同上，NAT 穿透困难 |
| 设备做 WebSocket Client 保持长连接 | ⚠️ | 断线重连复杂，无 QoS 保证 |
| **MQTT（设备主动连 broker）** | ✅ | 设备出站连接，天然穿透 NAT，QoS 保证，离线消息缓存 |

### 架构图

```
Java 后台（云端）
    │
    │  publish: print/{device_id}/job
    ▼
MQTT Broker（公网，如 EMQX）
    │
    │  subscribe（设备主动出站连接）
    ▼
BK7252N 设备（WiFi，内网）
    │
    ▼
热敏打印机（BLE/UART）
```

### MQTT Topic 设计

| Topic | 方向 | 用途 |
| --- | --- | --- |
| `print/{device_id}/job` | 后台 → 设备 | 下发打印任务 |
| `print/{device_id}/status` | 设备 → 后台 | 打印结果回报（成功/失败/缺纸） |
| `device/{device_id}/online` | 设备 → 后台 | 设备上线心跳（retain=true） |

### 打印任务消息格式

```json
{
  "job_id": "550e8400-e29b-41d4-a716-446655440000",
  "type": "text",
  "content": "订单号：20260331-001\n商品：美式咖啡×2\n合计：36元",
  "copies": 1,
  "font_size": 2
}
```

`type` 可扩展为 `text`（纯文本）/ `escpos`（原始 ESC/POS 字节流，Base64 编码）/ `image`（图片）。

### 设备端实现要点

项目已集成 `packages/pahomqtt`，启用步骤：

1. 在 `rtconfig.h` 中启用 `PKG_USING_PAHOMQTT`
2. 连接 WiFi 后启动 MQTT 客户端，订阅 `print/{device_id}/job`
3. 收到消息 → 解析 JSON → 调用打印驱动

QoS 建议使用 **QoS 1**（至少送达一次），配合设备端去重（job_id 防重复打印）。

### 与语音识别的关系

两条通道互相独立，可并行运行：

```
设备
 ├── MQTT client（常驻后台）─── broker ─── Java 后台（远程打印）
 └── WebSocket client（按需）─── AI 服务（语音识别）
```

语音识别完成后，云端也可以通过 MQTT 把打印任务推回设备，统一走 MQTT 通道，避免设备维护两个长连接。