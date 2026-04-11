## ZhiPlus AI Printer Firmware (BK7252N / RT-Thread SDK 3.0.78)

该工程在博通 `BK7252N`（RT-Thread SDK 3.0.78）上实现“AI 语音打印机”的端侧固件能力。

当前固件的实现路径是：

`按住物理按键 -> 采集麦克风(8kHz/16bit/mono PCM) -> WebSocket 串流音频到云端(ASR + 生成 ESC/POS) -> 接收 base64 ESC/POS -> BLE 热敏打印机打印`

### 1. 关键入口代码

- 系统启动入口：`applications/main.c`
  - 打印启动 banner
  - 调用 `ai_printer_start()`
- AI 语音打印逻辑：`applications/ai_printer.c`
  - WiFi 连接
  - 按键触发录音与 WebSocket 会话
  - 接收云端返回的 `type=print` 或 `type=complete`
  - 将 ESC/POS base64 转发给 BLE 打印模块
- BLE 打印模块：`applications/ble_printer.c`
  - 扫描指定蓝牙热敏打印机名称
  - 发现写入特征（UUID 0xFF02）
  - 将 ESC/POS 字节流按 MTU 切片写入；必要时回退到 UART 串口

### 2. 语音到打印的端侧流程（对应 `docs/AI打印机技术方案.md`）

1. 上电后初始化 ROMFS、网络与音频相关能力，然后进入主任务。
2. `ai_printer_task()`：
   - 连接 WiFi（按配置列表依次尝试）
   - 等待按键“按下并保持”
3. 按键保持期间：
   - 从 RT-Thread 设备 `mic` 持续读取 PCM 帧（100ms 一帧）
   - 通过 WebSocket 二进制帧发送到云端
4. 按键松开：
   - 发送 WebSocket 文本消息 `{"type":"stop"}`
   - 等待云端返回：
     - `{"type":"print","data":"<base64 escpos bytes>"}`（优先处理）
     - 或 `{"type":"complete"}` / `{"type":"error"}`（错误/无打印数据）
5. 收到 `print.data` 后：
   - base64 解码为 ESC/POS 原始字节流
   - 调用 `ble_printer_send()` / `ble_printer_send_base64()` 完成打印

### 3. 可调关键参数（在 `applications/ai_printer.c`）

- 按键输入
  - `AI_BTN_GPIO`：按键 GPIO（button → GND）
- WiFi
  - `AI_WIFI_LIST[]`：SSID/密码列表（固定写在代码里）
  - `AI_WIFI_TIMEOUT_MS`：WiFi 连接超时
- WebSocket
  - `AI_DEFAULT_HOST` / `AI_DEFAULT_PORT`
  - `AI_WS_PATH`：`/ws/voicePrint`
  - `AI_DEVICE_TOKEN`：设备鉴权 token（当前是代码内硬编码）
  - 音频格式与采样率：
    - `AI_SAMPLE_RATE = 8000`
    - `AI_PCM_FRAME = 1600`（100ms）
  - 录音时长约束：
    - `AI_RECORD_MAX_SECS = 6`
- shell 命令（MSH）
  - `sethost <host:port|default>`：可切换 WS 主机地址（便于本地调试）

### 4. 蓝牙打印机适配（在 `applications/ble_printer.c`）

- 打印机名称匹配：`PRINTER_NAME = "TPC50S_A07B_BLE"`
- 写入特征（GATT write characteristic）
  - 期望 UUID 0xFF02（写特征）
- 数据切片
  - 默认 `BLE_CHUNK_SIZE = 20` 字节（安全值）
  - 若能从打印机通知到 MTU，会使用 `mtu - 10` 计算每片大小
- 回退
  - 若 BLE 未就绪，尝试使用 `UART2`（设备名 `uart2`，波特率 `921600`）

### 5. 重要注意事项

- 当前实现“语音识别/生成打印内容”走 WebSocket（不是 MQTT）。
  - 工程里包含了 `packages/pahomqtt`，但 `ai_printer.c` 并未使用 MQTT 下发打印任务。
- `AI_DEVICE_TOKEN`、WiFi SSID/密码等目前是代码硬编码。
  - 若要面向量产，建议改为从 `config.json`、OTP、或 OTA 下发配置读取，避免泄露与“改配置要重新发固件”。

### 6. 你下一步可以做什么

如果你希望我继续深入“AI语音打印机”这条链路，请告诉我你想重点看哪一块：

1. WebSocket 协议字段与云端服务的对应（`asr_partial/asr/stage/print/complete`）
2. PCM 数据格式是否与云端解码完全一致（字节序、声道、100ms 分帧）
3. BLE 打印机写入特征 `0xFF02` 适配与 MTU 处理是否需要兼容更多打印机型号

