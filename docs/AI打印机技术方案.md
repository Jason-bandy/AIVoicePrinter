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