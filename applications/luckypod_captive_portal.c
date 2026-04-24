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

/* LCD 驱动 */
#include "../drivers/drv_st7789_lcd.h"

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

static void lcd_init(void)
{
    rt_kprintf("[LCD] ST7789 初始化 (240x240)\n");
    st7789_lcd_init();  /* 调用真正的初始化函数 */
    lcd_backlight_on();
}

/* 使用 drv_st7789_lcd.h 中声明的函数:
 * - lcd_clear(rt_uint16_t color)
 * - lcd_draw_qrcode(const char *ssid, const char *password)
 * - lcd_show_status(const char *status)
 */

/* ========================= WiFi Scan & Connect ========================= */

/* 全局扫描结果缓存（最多 32 个 AP） */
#define LUCKYPOD_SCAN_MAX   32
static SCAN_RST_ITEM_T g_scan_results[LUCKYPOD_SCAN_MAX];

/* 扫描 WiFi 列表（同步：触发扫描后等待结果） */
static int luckypod_wifi_scan(void)
{
    rt_kprintf("[WiFi] 开始扫描...\n");
    g_lp_state.wifi_scanning = RT_TRUE;

    bk_wlan_start_scan();
    rt_thread_mdelay(3000);  /* 等待扫描完成 */

    unsigned char ap_num = bk_wlan_get_scan_ap_result_numbers();
    if (ap_num > LUCKYPOD_SCAN_MAX)
        ap_num = LUCKYPOD_SCAN_MAX;

    bk_wlan_get_scan_ap_result(g_scan_results, ap_num);
    g_lp_state.scan_count = ap_num;

    rt_kprintf("[WiFi] 扫描完成，发现 %d 个网络\n", ap_num);
    for (int i = 0; i < ap_num; i++) {
        rt_kprintf("  [%d] SSID: %s, RSSI: %d, Auth: %d\n",
                   i, g_scan_results[i].ssid, g_scan_results[i].level,
                   g_scan_results[i].security);
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
    ef_save_env();

    /* 更新状态 */
    rt_strncpy(g_lp_state.sta_ssid, ssid, sizeof(g_lp_state.sta_ssid) - 1);
    rt_strncpy(g_lp_state.sta_pass, password, sizeof(g_lp_state.sta_pass) - 1);
    g_lp_state.mode = LP_MODE_CONNECTING;

    /* 使用 bk_wlan_start 连接 STA */
    network_InitTypeDef_st net_cfg;
    rt_memset(&net_cfg, 0, sizeof(net_cfg));
    net_cfg.wifi_mode = BK_STATION;
    net_cfg.dhcp_mode = DHCP_CLIENT;
    rt_strncpy(net_cfg.wifi_ssid, ssid, sizeof(net_cfg.wifi_ssid) - 1);
    rt_strncpy(net_cfg.wifi_key, password, sizeof(net_cfg.wifi_key) - 1);

    OSStatus ret = bk_wlan_start(&net_cfg);
    if (ret == kNoErr) {
        rt_kprintf("[WiFi] 连接请求已发送\n");
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
    char mac[6] = {0};
    bk_wifi_get_softap_mac_address(mac);
    rt_snprintf(ap_ssid, sizeof(ap_ssid), "%s%02X%02X",
                LUCKYPOD_AP_SSID_PREFIX, (unsigned char)mac[4], (unsigned char)mac[5]);

    rt_strncpy(g_lp_state.ap_ssid, ap_ssid, sizeof(g_lp_state.ap_ssid) - 1);

    rt_kprintf("[AP] 启动热点：%s (密码：%s)\n", ap_ssid, LUCKYPOD_AP_PASSWORD);

    /* 使用 network_InitTypeDef_ap_st 启动 AP */
    network_InitTypeDef_ap_st ap_cfg;
    rt_memset(&ap_cfg, 0, sizeof(ap_cfg));
    rt_strncpy(ap_cfg.wifi_ssid, ap_ssid, sizeof(ap_cfg.wifi_ssid) - 1);
    rt_strncpy(ap_cfg.wifi_key, LUCKYPOD_AP_PASSWORD, sizeof(ap_cfg.wifi_key) - 1);
    ap_cfg.channel   = LUCKYPOD_AP_CHANNEL;
    ap_cfg.max_con   = LUCKYPOD_AP_MAX_CONN;
    ap_cfg.security  = BK_SECURITY_TYPE_WPA2_AES;
    ap_cfg.dhcp_mode = DHCP_SERVER;
    rt_strncpy(ap_cfg.local_ip_addr,    "192.168.4.1",   sizeof(ap_cfg.local_ip_addr) - 1);
    rt_strncpy(ap_cfg.net_mask,         "255.255.255.0", sizeof(ap_cfg.net_mask) - 1);
    rt_strncpy(ap_cfg.gateway_ip_addr,  "192.168.4.1",   sizeof(ap_cfg.gateway_ip_addr) - 1);

    OSStatus ret = bk_wlan_start_ap_adv(&ap_cfg);
    if (ret == kNoErr) {
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
    luckypod_wifi_scan();

    for (int i = 0; i < g_lp_state.scan_count; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "ssid", g_scan_results[i].ssid);
        cJSON_AddNumberToObject(item, "rssi", g_scan_results[i].level);
        cJSON_AddNumberToObject(item, "security", g_scan_results[i].security);
        cJSON_AddItemToArray(array, item);
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

/* Captive Portal 探测响应 — 让系统认为有 captive portal 需要登录 */
static void cgi_captive_portal(struct webnet_session* session)
{
    rt_kprintf("[CP] Captive portal detected, URL: %s\n",
               session->request ? session->request->path : "unknown");

    /* 返回 200 + HTML，让手机认为需要认证，弹出浏览器 */
    const char *response =
        "<!DOCTYPE html><html><head>"
        "<meta http-equiv=\"refresh\" content=\"0;url=/\"/>"
        "</head><body><a href=\"/\">配网</a></body></html>";

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
"fetch('/get_wifi_list').then(r=>r.json()).then(d=>{"
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
"fetch('/connect_wifi?ssid='+encodeURIComponent(ssid)+'&password='+encodeURIComponent(pass))"
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

/* CGI 根路径为 / 时，所有请求都会走到这里，需要兜底处理 */
static void cgi_fallback(struct webnet_session* session)
{
    const char *path = session->request->path;
    rt_kprintf("[HTTP] 未匹配 CGI 路径: %s\n", path ? path : "unknown");

    /* 根路径或任意未知路径都返回配网页面 */
    webnet_session_set_header(session, "text/html", 200, "OK", rt_strlen(g_config_html));
    webnet_session_write(session, (rt_uint8_t*)g_config_html, rt_strlen(g_config_html));
}

/* 注册 CGI 处理函数 */
static void luckypod_cgi_register(void)
{
    /* 根路径兜底 — 最后注册，作为 catch-all（注册顺序反向匹配） */
    webnet_cgi_register("/", cgi_fallback);

    /* 页面处理 — CGI 根为 / 时匹配名去掉前导 / */
    webnet_cgi_register("config", cgi_config_page);

    /* Captive Portal 探测 URL — 覆盖 iOS/Android/Windows 主流系统
     * 注意：CGI 根为 / 时，wn_module_cgi.c 会跳过路径的前导 /，所以注册名不带 / */
    webnet_cgi_register("library/test/success.html", cgi_captive_portal);
    webnet_cgi_register("generate_204", cgi_captive_portal);
    webnet_cgi_register("ncsi.txt", cgi_captive_portal);
    webnet_cgi_register("kindle-wifi/wifisignincheck.json", cgi_captive_portal);

    /* iOS 实际使用的探测 URL */
    webnet_cgi_register("hotspot-detect.html", cgi_captive_portal);

    /* API 端点 */
    webnet_cgi_register("get_wifi_list", cgi_get_wifi_list);
    webnet_cgi_register("connect_wifi", cgi_connect_wifi);
    webnet_cgi_register("get_status", cgi_get_status);
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
    webnet_cgi_set_root("/");  /* CGI 匹配根路径，使 /hotspot-detect.html 等也能匹配 */

    /* 注册 CGI — 注意：CGI 匹配顺序为反向注册，最后注册的先匹配 */
    luckypod_cgi_register();

    /* 启动 HTTP Server */
    rt_kprintf("[HTTP] WebNet 启动 (端口 80)\n");
    webnet_init();

    /* QR 码已显示，不再重复清屏 */
    rt_kprintf("[LCD] 等待扫码...\n");
    
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
