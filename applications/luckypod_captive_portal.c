/*
 * LuckyPod Captive Portal - WiFi 配网方案
 * 
 * 功能：
 * 1. SoftAP 模式创建热点 "LuckyPod-XXXX"
 * 2. HTTP Server 提供配网页面
 * 3. Captive Portal 探测响应（iOS/Android 自动弹出）
 * 4. 扫描 WiFi 列表并连接
 * 5. EasyFlash 保存凭据
 * 6. ST7789 显示 QR 码和状态
 *
 * 使用：
 * 1. 设备启动进入 AP 模式
 * 2. 屏幕显示 QR 码（SSID + 密码）
 * 3. 手机扫码连接热点
 * 4. 自动弹出配网页面
 * 5. 选择 WiFi + 输入密码
 * 6. 设备连接并保存配置
 */

#include <rtthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* WiFi & Network */
#include "wlan_ui_pub.h"
#include "rw_msg_pub.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"

/* WebNet HTTP Server */
#include <webnet.h>
#include <wn_module.h>
#include <cJSON.h>

/* EasyFlash */
#include <easyflash.h>

/* GPIO */
#include "gpio_pub.h"

/* ========================= Configuration ========================= */

#define LUCKYPOD_AP_SSID_PREFIX   "LuckyPod-"
#define LUCKYPOD_AP_PASSWORD      "luckypod2026"  /* 默认热点密码 */
#define LUCKYPOD_AP_CHANNEL       6
#define LUCKYPOD_AP_MAX_CONN      4

/* Captive Portal 探测 URL */
#define CP_URL_APPLE              "/library/test/success.html"
#define CP_URL_ANDROID            "/generate_204"
#define CP_URL_MICROSOFT          "/ncsi.txt"
#define CP_URL_AMAZON             "/kindle-wifi/wifisignincheck.json"

/* EasyFlash 配置键 */
#define EF_KEY_WIFI_SSID          "wifi_ssid"
#define EF_KEY_WIFI_PASSWORD      "wifi_pass"
#define EF_KEY_DEVICE_MODE        "dev_mode"

/* ST7789 GPIO (根据实际硬件修改) */
#define LCD_CS_GPIO               GPIO_PB0
#define LCD_CLK_GPIO              GPIO_PB1
#define LCD_MOSI_GPIO             GPIO_PB2
#define LCD_DC_GPIO               GPIO_PB3
#define LCD_RST_GPIO              GPIO_PB4

/* 状态机 */
typedef enum {
    LP_MODE_AP = 0,              /* AP 模式 - 配网中 */
    LP_MODE_STA = 1,             /* STA 模式 - 正常工作 */
    LP_MODE_CONNECTING = 2,      /* 正在连接 WiFi */
} luckypod_mode_t;

/* 全局状态 */
static struct {
    luckypod_mode_t mode;
    char ap_ssid[32];
    char sta_ssid[32];
    char sta_pass[64];
    rt_bool_t wifi_scanning;
    rt_uint8_t scan_count;
} g_lp_state = {0};

/* ========================= ST7789 Display ========================= */

/* TODO: 根据实际 SPI 驱动实现 */
static void lcd_init(void)
{
    rt_kprintf("[LCD] ST7789 初始化 (240x240)\n");
    /* 实际实现：
     * 1. 初始化 SPI
     * 2. 发送 ST7789 初始化序列
     * 3. 设置显示方向、伽马等
     */
}

static void lcd_clear(rt_uint16_t color)
{
    /* 清屏为指定颜色 */
    (void)color;
}

static void lcd_draw_qrcode(const char *ssid, const char *password)
{
    /* 绘制 WiFi QR 码
     * 格式：WIFI:T:WPA;S:{ssid};P:{password};;
     * 使用 QR 码库生成并绘制到屏幕
     */
    rt_kprintf("[LCD] 显示 QR 码：SSID=%s\n", ssid);
    
    /* 示例：显示文字代替 QR 码（实际应使用 QR 库）
     * 推荐库：https://github.com/nayuki/QR-Code-generator
     */
    lcd_clear(0x0000);  /* 黑色背景 */
    
    /* 显示 SSID */
    char qr_text[64];
    rt_snprintf(qr_text, sizeof(qr_text), "WIFI:%s", ssid);
    /* lcd_draw_string(10, 10, qr_text); */
    
    /* 显示密码 */
    rt_snprintf(qr_text, sizeof(qr_text), "PWD:%s", password);
    /* lcd_draw_string(10, 30, qr_text); */
}

static void lcd_show_status(const char *status)
{
    /* 显示状态文字 */
    rt_kprintf("[LCD] 状态：%s\n", status);
    /* lcd_draw_string(10, 100, status); */
}

/* ========================= WiFi Scan & Connect ========================= */

/* 扫描 WiFi 列表 */
static int luckypod_wifi_scan(void)
{
    struct rw_scan_result *results = RT_NULL;
    rt_uint32_t count = 0;
    
    rt_kprintf("[WiFi] 开始扫描...\n");
    g_lp_state.wifi_scanning = RT_TRUE;
    
    /* 执行扫描 */
    if (rw_scan(&results, &count) == RT_EOK) {
        g_lp_state.scan_count = count;
        rt_kprintf("[WiFi] 扫描完成，发现 %d 个网络\n", count);
        
        for (rt_uint32_t i = 0; i < count; i++) {
            rt_kprintf("  [%d] SSID: %s, RSSI: %d, Auth: %d\n", 
                       i, results[i].ssid, results[i].rssi, results[i].security);
        }
        
        rt_free(results);
    } else {
        rt_kprintf("[WiFi] 扫描失败\n");
        g_lp_state.scan_count = 0;
    }
    
    g_lp_state.wifi_scanning = RT_FALSE;
    return g_lp_state.scan_count;
}

/* 连接到指定 WiFi */
static int luckypod_wifi_connect(const char *ssid, const char *password)
{
    rt_kprintf("[WiFi] 连接：SSID=%s\n", ssid);
    lcd_show_status("正在连接 WiFi...");
    
    /* 保存配置到 EasyFlash */
    ef_set_env(EF_KEY_WIFI_SSID, ssid);
    ef_set_env(EF_KEY_WIFI_PASSWORD, password);
    ef_set_env(EF_KEY_DEVICE_MODE, "sta");
    
    /* 更新状态 */
    rt_strncpy(g_lp_state.sta_ssid, ssid, sizeof(g_lp_state.sta_ssid) - 1);
    rt_strncpy(g_lp_state.sta_pass, password, sizeof(g_lp_state.sta_pass) - 1);
    g_lp_state.mode = LP_MODE_CONNECTING;
    
    /* 连接到 WiFi */
    wlan_ui_config_t config = {0};
    rt_strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    rt_strncpy(config.password, password, sizeof(config.password) - 1);
    
    if (wlan_ui_connect(&config) == RT_EOK) {
        rt_kprintf("[WiFi] 连接成功！\n");
        lcd_show_status("配网成功！");
        g_lp_state.mode = LP_MODE_STA;
        return RT_EOK;
    } else {
        rt_kprintf("[WiFi] 连接失败\n");
        lcd_show_status("连接失败，请重试");
        g_lp_state.mode = LP_MODE_AP;
        return -RT_ERROR;
    }
}

/* ========================= SoftAP Setup ========================= */

/* 创建 LuckyPod 热点 */
static int luckypod_ap_start(void)
{
    char ap_ssid[32] = {0};
    
    /* 生成唯一 SSID（使用 MAC 地址后 4 位） */
    rt_uint8_t mac[6] = {0};
    /* wlan_get_mac(mac); */  /* TODO: 获取实际 MAC */
    rt_snprintf(ap_ssid, sizeof(ap_ssid), "%s%02X%02X", 
                LUCKYPOD_AP_SSID_PREFIX, mac[4], mac[5]);
    
    rt_strncpy(g_lp_state.ap_ssid, ap_ssid, sizeof(g_lp_state.ap_ssid) - 1);
    
    rt_kprintf("[AP] 启动热点：%s (密码：%s)\n", ap_ssid, LUCKYPOD_AP_PASSWORD);
    
    /* 配置 SoftAP */
    wlan_ap_config_t ap_config = {0};
    rt_strncpy(ap_config.ssid, ap_ssid, sizeof(ap_config.ssid) - 1);
    rt_strncpy(ap_config.password, LUCKYPOD_AP_PASSWORD, sizeof(ap_config.password) - 1);
    ap_config.channel = LUCKYPOD_AP_CHANNEL;
    ap_config.max_connections = LUCKYPOD_AP_MAX_CONN;
    ap_config.security = SECURITY_WPA2_PSK;
    
    /* 启动 AP */
    if (wlan_ap_start(&ap_config) == RT_EOK) {
        rt_kprintf("[AP] 热点启动成功\n");
        g_lp_state.mode = LP_MODE_AP;
        
        /* 显示 QR 码 */
        lcd_show_status("扫码配网");
        lcd_draw_qrcode(ap_ssid, LUCKYPOD_AP_PASSWORD);
        
        return RT_EOK;
    } else {
        rt_kprintf("[AP] 热点启动失败\n");
        return -RT_ERROR;
    }
}

/* ========================= HTTP Server & CGI ========================= */

/* CGI: 获取 WiFi 列表 */
static void cgi_get_wifi_list(struct webnet_session* session)
{
    const char* mimetype = "application/json";
    cJSON *json = cJSON_CreateObject();
    
    if (!json) {
        webnet_session_set_header(session, mimetype, 500, "Error", -1);
        return;
    }
    
    cJSON_AddStringToObject(json, "action", "getWifiList");
    cJSON_AddStringToObject(json, "status", "success");
    
    cJSON *array = cJSON_CreateArray();
    
    /* 扫描 WiFi */
    struct rw_scan_result *results = RT_NULL;
    rt_uint32_t count = 0;
    
    if (rw_scan(&results, &count) == RT_EOK) {
        for (rt_uint32_t i = 0; i < count; i++) {
            cJSON *item = cJSON_CreateObject();
            cJSON_AddStringToObject(item, "ssid", results[i].ssid);
            cJSON_AddNumberToObject(item, "rssi", results[i].rssi);
            cJSON_AddNumberToObject(item, "security", results[i].security);
            cJSON_AddItemToArray(array, item);
        }
        rt_free(results);
    }
    
    cJSON_AddItemToObject(json, "data", array);
    
    char *body = cJSON_PrintUnformatted(json);
    webnet_session_set_header(session, mimetype, 200, "OK", rt_strlen(body));
    webnet_session_write(session, (rt_uint8_t*)body, rt_strlen(body));
    
    free(body);
    cJSON_Delete(json);
}

/* CGI: 连接 WiFi */
static void cgi_connect_wifi(struct webnet_session* session)
{
    const char* mimetype = "application/json";
    cJSON *json = cJSON_CreateObject();
    
    /* 解析请求 */
    const char *ssid = webnet_request_get_query(session->request, "ssid");
    const char *password = webnet_request_get_query(session->request, "password");
    
    if (!ssid) {
        cJSON_AddStringToObject(json, "status", "error");
        cJSON_AddStringToObject(json, "message", "Missing SSID");
    } else {
        rt_kprintf("[HTTP] 连接请求：SSID=%s\n", ssid);
        
        /* 保存配置并连接 */
        luckypod_wifi_connect(ssid, password ? password : "");
        
        cJSON_AddStringToObject(json, "status", "success");
        cJSON_AddStringToObject(json, "message", "Connecting...");
    }
    
    char *body = cJSON_PrintUnformatted(json);
    webnet_session_set_header(session, mimetype, 200, "OK", rt_strlen(body));
    webnet_session_write(session, (rt_uint8_t*)body, rt_strlen(body));
    
    free(body);
    cJSON_Delete(json);
}

/* CGI: 获取设备状态 */
static void cgi_get_status(struct webnet_session* session)
{
    const char* mimetype = "application/json";
    cJSON *json = cJSON_CreateObject();
    
    cJSON_AddStringToObject(json, "action", "getStatus");
    cJSON_AddStringToObject(json, "status", "success");
    
    cJSON *data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "mode", g_lp_state.mode);
    cJSON_AddStringToObject(data, "ap_ssid", g_lp_state.ap_ssid);
    cJSON_AddStringToObject(data, "sta_ssid", g_lp_state.sta_ssid);
    cJSON_AddStringToObject(data, "device_id", "luckypod-001");
    cJSON_AddStringToObject(data, "version", "1.0.0");
    
    cJSON_AddItemToObject(json, "data", data);
    
    char *body = cJSON_PrintUnformatted(json);
    webnet_session_set_header(session, mimetype, 200, "OK", rt_strlen(body));
    webnet_session_write(session, (rt_uint8_t*)body, rt_strlen(body));
    
    free(body);
    cJSON_Delete(json);
}

/* Captive Portal 探测响应 */
static void cgi_captive_portal(struct webnet_session* session)
{
    /* 返回 204 No Content 或简单 HTML，让手机认为已连接互联网 */
    const char* response = "<html><head><meta http-equiv=\"refresh\" content=\"0;url=/\"/></head></html>";
    
    webnet_session_set_header(session, "text/html", 200, "OK", rt_strlen(response));
    webnet_session_write(session, (rt_uint8_t*)response, rt_strlen(response));
}

/* 主配网页面 HTML */
static const char* g_config_html = 
"<!DOCTYPE html>"
"<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>LuckyPod 配网</title>"
"<style>"
"body{font-family:Arial,sans-serif;max-width:600px;margin:20px auto;padding:20px;background:#f5f5f5}"
".card{background:#fff;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1)}"
"h1{color:#333;text-align:center}"
".form-group{margin:15px 0}"
"label{display:block;margin-bottom:5px;color:#666}"
"input[type='text'],input[type='password'],select{width:100%;padding:10px;border:1px solid #ddd;border-radius:5px;box-sizing:border-box}"
"button{width:100%;padding:12px;background:#667eea;color:#fff;border:none;border-radius:5px;font-size:16px;cursor:pointer}"
"button:hover{background:#5568d3}"
".status{margin-top:15px;padding:10px;background:#e8f5e9;border-radius:5px;text-align:center}"
".loading{display:none;text-align:center;color:#666}"
"</style></head>"
"<body>"
"<div class='card'>"
"<h1>🎯 LuckyPod 配网</h1>"
"<div class='form-group'>"
"<label for='wifi-ssid'>选择 WiFi 网络</label>"
"<select id='wifi-ssid' onchange='document.getElementById(\"wifi-password\").focus()'>"
"<option value=''>扫描中...</option>"
"</select>"
"</div>"
"<div class='form-group'>"
"<label for='wifi-password'>WiFi 密码</label>"
"<input type='password' id='wifi-password' placeholder='输入 WiFi 密码'>"
"</div>"
"<button onclick='connectWifi()'>连接</button>"
"<div id='status' class='status' style='display:none'></div>"
"<div id='loading' class='loading'>⏳ 正在连接...</div>"
"</div>"
"<script>"
"function loadWifiList(){"
"fetch('/cgi-bin/get_wifi_list').then(r=>r.json()).then(d=>{"
"const sel=document.getElementById('wifi-ssid');sel.innerHTML='';"
"if(d.data&&d.data.length>0){d.data.forEach(w=>{"
"const opt=document.createElement('option');opt.value=w.ssid;opt.text=w.ssid+(w.security?' 🔒':'');sel.add(opt);"
"})}else{sel.innerHTML='<option>未找到网络</option>';}"
"}).catch(e=>{console.error(e);})}"
"function connectWifi(){"
"const ssid=document.getElementById('wifi-ssid').value;"
"const pass=document.getElementById('wifi-password').value;"
"if(!ssid){alert('请选择 WiFi');return;}"
"document.getElementById('loading').style.display='block';"
"document.getElementById('status').style.display='none';"
"fetch('/cgi-bin/connect_wifi?ssid='+encodeURIComponent(ssid)+'&password='+encodeURIComponent(pass))"
".then(r=>r.json()).then(d=>{"
"document.getElementById('loading').style.display='none';"
"const st=document.getElementById('status');st.style.display='block';"
"if(d.status==='success'){st.innerHTML='✅ 配网成功！设备将重启';st.style.background='#e8f5e9';"
"setTimeout(()=>{window.location.reload();},3000);}else{st.innerHTML='❌ '+d.message;st.style.background='#ffebee';}"
"}).catch(e=>{document.getElementById('loading').style.display='none';alert('连接失败');});"
"}"
"loadWifiList();"
"</script></body></html>";

static void cgi_config_page(struct webnet_session* session)
{
    webnet_session_set_header(session, "text/html", 200, "OK", rt_strlen(g_config_html));
    webnet_session_write(session, (rt_uint8_t*)g_config_html, rt_strlen(g_config_html));
}

/* 注册 CGI 处理函数 */
static void luckypod_cgi_register(void)
{
    webnet_cgi_register("get_wifi_list", cgi_get_wifi_list);
    webnet_cgi_register("connect_wifi", cgi_connect_wifi);
    webnet_cgi_register("get_status", cgi_get_status);
    webnet_cgi_register("config", cgi_config_page);
    
    /* Captive Portal 探测 URL */
    webnet_cgi_register(CP_URL_APPLE, cgi_captive_portal);
    webnet_cgi_register(CP_URL_ANDROID, cgi_captive_portal);
    webnet_cgi_register(CP_URL_MICROSOFT, cgi_captive_portal);
    webnet_cgi_register(CP_URL_AMAZON, cgi_captive_portal);
}

/* ========================= Main Entry ========================= */

/* 配网模式线程 */
static void luckypod_ap_thread_entry(void *parameter)
{
    rt_kprintf("\n");
    rt_kprintf("╔════════════════════════════════════════╗\n");
    rt_kprintf("║   LuckyPod Captive Portal 配网模式    ║\n");
    rt_kprintf("╚════════════════════════════════════════╝\n");
    
    /* 初始化 LCD */
    lcd_init();
    lcd_show_status("启动配网模式...");
    
    /* 启动 SoftAP */
    if (luckypod_ap_start() != RT_EOK) {
        rt_kprintf("[ERROR] AP 启动失败\n");
        lcd_show_status("启动失败，请重启");
        return;
    }
    
    /* 配置 WebNet */
    webnet_set_port(80);
    webnet_set_root("/");
    
    /* 注册 CGI */
    luckypod_cgi_register();
    
    /* 启动 HTTP Server */
    rt_kprintf("[HTTP] WebNet 启动 (端口 80)\n");
    webnet_init();
    
    lcd_show_status("等待扫码...");
    
    /* 主循环：检查 WiFi 连接状态 */
    while (1) {
        rt_thread_mdelay(1000);
        
        /* 检查是否已连接到 STA */
        if (g_lp_state.mode == LP_MODE_STA) {
            rt_kprintf("[INFO] 已连接到 WiFi，退出 AP 模式\n");
            lcd_show_status("配网完成！");
            
            /* 延迟 2 秒后重启进入 STA 模式 */
            rt_thread_mdelay(2000);
            
            /* TODO: 重启设备进入 STA 模式 */
            /* sys_reboot(); */
            
            break;
        }
    }
}

/* 初始化函数 */
int luckypod_captive_portal_init(void)
{
    /* 创建 AP 模式线程 */
    rt_thread_t tid = rt_thread_create("lp_ap",
                                        luckypod_ap_thread_entry,
                                        RT_NULL,
                                        4096,
                                        20,
                                        10);
    if (tid) {
        rt_thread_startup(tid);
        rt_kprintf("[INFO] Captive Portal 线程已创建\n");
    }
    
    return 0;
}
INIT_APP_EXPORT(luckypod_captive_portal_init);

/* MSH 命令 */
#ifdef FINSH_USING_MSH
#include <finsh.h>

MSH_CMD_EXPORT(luckypod_captive_portal_init, 启动 LuckyPod 配网模式);

#endif /* FINSH_USING_MSH */
