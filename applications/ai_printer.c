/*
 * AI Printer Application - BK7252N
 *
 * Flow: Boot → WiFi Connect → hold button → HTTP upload PCM → receive print data → loop
 *
 * WebSocket endpoint: ws://host:port/ws/voicePrint
 *   - Binary frames: PCM audio (8kHz/16bit/mono) streamed while recording
 *   - Text {"type":"stop"}: sent on button release
 *   - Server replies: asr_partial, asr, stage, print, complete
 *
 * Usage: Press and HOLD trigger button (GPIO AI_BTN_GPIO) to record, release to stop.
 */

#include <rtthread.h>
#include <string.h>
#include <stdlib.h>
#include <librws.h>
#include <webclient.h>
#include <ntp.h>
#include "wlan_ui_pub.h"
#include "rw_msg_pub.h"
#include "gpio_pub.h"
#include "multi_button.h"

/* ========================= Configuration ========================= */
#define AI_BTN_GPIO     4   /* GPIO pin connected to trigger button (button → GND) */

/* Mode switch: 0 = HTTP (default, upload → poll result), 1 = WSS streaming */
#define AI_USE_WSS_MODE 0

/* Development: force HTTP to bypass TLS verification issues (CA cert not loaded).
 * Set to 1 when proper CA cert is installed on device. */
#define AI_FORCE_HTTP 1

/* HTTP API endpoints */
#define AI_HTTP_UPLOAD_PATH  "/luckypod/aiPrinter/voiceStreamPrint"  /* POST PCM audio → returns task_id */
#define AI_HTTP_RESULT_PATH  "/luckypod/aiPrinter/%s/result"          /* GET task result → returns print data */

/* HTTP config */
#define AI_HTTP_RESULT_POLL_MS   3000   /* poll interval for result */
#define AI_HTTP_RESULT_TIMEOUT_MS 60000 /* max wait for server processing */
#define AI_HTTP_RESP_BUF_MAX     131072 /* 128KB max response buffer for print data */

/* WiFi AP list: tries each in order until one connects */
static const struct { const char *ssid; const char *pass; } AI_WIFI_LIST[] = {
    { "XMLJ",       "lj20251210" },
    { "Xiaomi_402", "88996677"   },
};

/* Server defaults */
#define AI_DEFAULT_HOST     "test.api.transkoi.luckjingle.com"
#define AI_DEFAULT_PORT     80
/* NOTE: To reach a local dev server use: sethost 192.168.x.x:9005 */
#define AI_HOST_SEL_MS      5000

/* Device API key — must match token.test-token in server application.properties */
#define AI_DEVICE_TOKEN     "da60e317-7114-48c6-8224-e99c750af2b9"

/* WebSocket path */
#define AI_WS_PATH          "/ws/voicePrint"

/* Audio: 16kHz, 16-bit, mono */
#define AI_SAMPLE_RATE      16000
#define AI_LANGUAGE         "zh"
#define AI_RECORD_MAX_SECS  3
#define AI_PCM_FRAME        3200   /* 100ms @ 16kHz/16bit = 3200 bytes */
#define AI_PCM_MAX_BYTES    (AI_SAMPLE_RATE * AI_RECORD_MAX_SECS * 2)  /* 96KB */
#define AI_PCM_MIN_BYTES    (AI_SAMPLE_RATE / 2 * 2)                     /* 16KB (0.5s) */

#define AI_B64_BUF_MAX      102400  /* max ESC/POS base64 size */

#define AI_WIFI_TIMEOUT_MS  30000
#define AI_WS_CONNECT_MS    10000  /* WS connect timeout */
#define AI_WS_RESULT_MS     30000  /* wait for complete/print after stop */

#define AI_TASK_STACK_SZ    16384
#define AI_TASK_PRIO        10   /* 提高优先级，避免被 WiFi/BLE 线程抢占导致 ADC 数据丢失 */

/* ========================= Runtime Host Config ========================= */

static char g_ws_host[128] = AI_DEFAULT_HOST;
static int  g_ws_port      = AI_DEFAULT_PORT;
static int  g_ws_secure    = 0;   /* 0 = http/ws, 1 = https/wss (TODO: 修复 TLS 证书后改回 1) */

static int ai_is_local(const char *host)
{
    return strstr(host, "127.0.0.1") != RT_NULL ||
           strstr(host, "localhost")  != RT_NULL ||
           strstr(host, "192.168.")   != RT_NULL ||
           strstr(host, "10.")        != RT_NULL ||
           strstr(host, "172.")       != RT_NULL;
}

/* Parse "host:port" or just "host"; update global WS config */
static void ai_set_host(const char *hostport)
{
    char tmp[128];
    rt_strncpy(tmp, hostport, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    char *colon = strrchr(tmp, ':');
    if (colon) {
        *colon = '\0';
        g_ws_port = atoi(colon + 1);
    } else {
        g_ws_port = ai_is_local(tmp) ? 80 : 443;
    }
    rt_strncpy(g_ws_host, tmp, sizeof(g_ws_host) - 1);
#if AI_FORCE_HTTP
    g_ws_secure = 0;   /* Force HTTP to bypass TLS verification (no CA cert on device) */
    g_ws_port = 80;    /* HTTP runs on port 80 */
#else
    g_ws_secure = ai_is_local(g_ws_host) ? 0 : 1;
#endif

#if AI_USE_WSS_MODE
    rt_kprintf("[AIPrinter] URL set: %s://%s:%d%s\n",
               g_ws_secure ? "wss" : "ws", g_ws_host, g_ws_port, AI_WS_PATH);
#else
    rt_kprintf("[AIPrinter] API host set: %s://%s:%d\n",
               g_ws_secure ? "https" : "http", g_ws_host, g_ws_port);
#endif
}

/* MSH command: sethost 192.168.1.100:9005  or  sethost default */
static int cmd_sethost(int argc, char **argv)
{
    if (argc < 2) {
        rt_kprintf("[AIPrinter] Usage: sethost <host:port|default>\n");
        rt_kprintf("[AIPrinter]   e.g. sethost 192.168.1.100:9005\n");
        rt_kprintf("[AIPrinter]        sethost default\n");
#if AI_USE_WSS_MODE
        rt_kprintf("[AIPrinter] Current: %s://%s:%d\n",
                   g_ws_secure ? "wss" : "ws", g_ws_host, g_ws_port);
#else
        rt_kprintf("[AIPrinter] Current: %s://%s:%d\n",
                   g_ws_secure ? "https" : "http", g_ws_host, g_ws_port);
#endif
        return 0;
    }
    const char *arg = argv[1];
    ai_set_host(rt_strcmp(arg, "default") == 0 ? AI_DEFAULT_HOST : arg);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_sethost, sethost, set AI printer API host);

/* ========================= WiFi Connect ========================= */

/* User-specified WiFi via MSH command: setwifi <ssid> <password> */
static char g_user_wifi_ssid[32] = {0};
static char g_user_wifi_pass[64] = {0};
static int  g_user_wifi_set = 0;

static void ai_set_wifi(const char *ssid, const char *pass)
{
    if (ssid && ssid[0]) {
        rt_strncpy(g_user_wifi_ssid, ssid, sizeof(g_user_wifi_ssid) - 1);
        rt_strncpy(g_user_wifi_pass, pass ? pass : "", sizeof(g_user_wifi_pass) - 1);
        g_user_wifi_set = 1;
        rt_kprintf("[AIPrinter] WiFi set: %s (password hidden)\n", g_user_wifi_ssid);
    } else {
        g_user_wifi_set = 0;
        rt_kprintf("[AIPrinter] WiFi cleared, using defaults\n");
    }
}

static int cmd_setwifi(int argc, char **argv)
{
    if (argc < 2) {
        rt_kprintf("[AIPrinter] Usage: setwifi <ssid> [password]\n");
        rt_kprintf("[AIPrinter]   e.g. setwifi XMLJ lj20251210\n");
        if (g_user_wifi_set) {
            rt_kprintf("[AIPrinter] Current: %s\n", g_user_wifi_ssid);
        } else {
            rt_kprintf("[AIPrinter] Using default AP list\n");
        }
        return 0;
    }
    ai_set_wifi(argv[1], argc >= 3 ? argv[2] : "");
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_setwifi, setwifi, set WiFi SSID and password);

static int ai_wifi_connect(void)
{
    network_InitTypeDef_st cfg;

    /* If user specified WiFi via setwifi, try that first */
    if (g_user_wifi_set && g_user_wifi_ssid[0]) {
        memset(&cfg, 0, sizeof(cfg));
        cfg.wifi_mode = BK_STATION;
        strncpy(cfg.wifi_ssid, g_user_wifi_ssid, sizeof(cfg.wifi_ssid) - 1);
        strncpy(cfg.wifi_key,  g_user_wifi_pass,  sizeof(cfg.wifi_key)  - 1);
        cfg.dhcp_mode = DHCP_CLIENT;

        rt_kprintf("[AIPrinter] Trying user-specified WiFi: %s\n", g_user_wifi_ssid);
        bk_wlan_start(&cfg);

        for (int waited = 0; waited < AI_WIFI_TIMEOUT_MS; waited += 300) {
            rw_evt_type st = mhdr_get_station_status();
            if (st == RW_EVT_STA_GOT_IP) {
                rt_kprintf("[AIPrinter] WiFi connected: %s\n", g_user_wifi_ssid);
                return 0;
            }
            if (st == RW_EVT_STA_PASSWORD_WRONG || st == RW_EVT_STA_NO_AP_FOUND) {
                break;
            }
            rt_thread_delay(300);
        }
        rt_kprintf("[AIPrinter] User WiFi failed, trying default list...\n");
    }

    /* Try default AP list */
    int n = sizeof(AI_WIFI_LIST) / sizeof(AI_WIFI_LIST[0]);
    for (int i = 0; i < n; i++) {
        memset(&cfg, 0, sizeof(cfg));
        cfg.wifi_mode = BK_STATION;
        strncpy(cfg.wifi_ssid, AI_WIFI_LIST[i].ssid, sizeof(cfg.wifi_ssid) - 1);
        strncpy(cfg.wifi_key,  AI_WIFI_LIST[i].pass,  sizeof(cfg.wifi_key)  - 1);
        cfg.dhcp_mode = DHCP_CLIENT;

        rt_kprintf("[AIPrinter] Trying WiFi: %s\n", AI_WIFI_LIST[i].ssid);
        bk_wlan_start(&cfg);

        for (int waited = 0; waited < AI_WIFI_TIMEOUT_MS; waited += 300) {
            rw_evt_type st = mhdr_get_station_status();
            if (st == RW_EVT_STA_GOT_IP) {
                rt_kprintf("[AIPrinter] WiFi connected: %s\n", AI_WIFI_LIST[i].ssid);
                return 0;
            }
            if (st == RW_EVT_STA_PASSWORD_WRONG || st == RW_EVT_STA_NO_AP_FOUND) {
                rt_kprintf("[AIPrinter] WiFi %s failed, trying next...\n", AI_WIFI_LIST[i].ssid);
                break;
            }
            rt_thread_delay(300);
        }
    }
    rt_kprintf("[AIPrinter] WiFi: all APs failed!\n");
    return -1;
}

/* ========================= Button Trigger ========================= */

static BUTTON_S          g_ai_button;
static rt_sem_t          g_btn_sem;
static volatile int      g_btn_held = 0;

static uint8_t btn_read_level(BUTTON_S *handle)
{
    return (uint8_t)bk_gpio_input((uint32_t)handle->user_data);
}

static void btn_on_press_down(void *param)
{
    g_btn_held = 1;
    rt_kprintf("[AIPrinter] Button held — recording...\n");
    rt_sem_release(g_btn_sem);
}

static void btn_on_press_up(void *param)
{
    g_btn_held = 0;
    rt_kprintf("[AIPrinter] Button released — stopping recording.\n");
}

static void btn_tick_thread(void *arg)
{
    while (1) {
        button_ticks(RT_NULL);
        rt_thread_delay(5);
    }
}

static void ai_button_init(void)
{
    bk_gpio_config_input_pup(AI_BTN_GPIO);
    button_init(&g_ai_button, btn_read_level, 0, (void *)AI_BTN_GPIO);
    button_attach(&g_ai_button, PRESS_DOWN, btn_on_press_down);
    button_attach(&g_ai_button, PRESS_UP,   btn_on_press_up);
    button_start(&g_ai_button);

    rt_thread_t t = rt_thread_create("btn_tick", btn_tick_thread, RT_NULL,
                                     512, AI_TASK_PRIO - 1, 5);
    if (t) rt_thread_startup(t);
}

/* ========================= WebSocket Session Context ========================= */

typedef struct {
    rws_socket   sock;          /* socket reference for callback checks */
    rt_sem_t     connected_sem; /* signaled by on_connected */
    rt_sem_t     done_sem;      /* signaled by on_text(complete/print/error) or on_disconnected */
    char        *b64;           /* heap: ESC/POS base64 print data */
    int          b64_len;
    volatile int got_result;    /* 1 after complete/print received */
    volatile int error;         /* 1 on error or disconnect-before-result */
    volatile int disconnecting; /* 1 during cleanup, protect against callbacks */
} ai_ws_ctx_t;

static ai_ws_ctx_t g_ws_ctx;

/* ---- Helpers: safe strncopy + JSON field extraction ---- */

/*
 * Extract a quoted integer value for a JSON key.
 * Finds "key":<value> in buf[0..len] and stores into *out.
 * Returns 0 on success, -1 if not found.
 */
static int json_get_int(const char *buf, unsigned int len,
                        const char *key, int *out)
{
    char pat[64];
    rt_snprintf(pat, sizeof(pat), "\"%s\":", key);
    const char *p = strstr(buf, pat);
    if (!p || (unsigned int)(p - buf) >= len) return -1;
    p += rt_strlen(pat);
    *out = atoi(p);
    return 0;
}

/*
 * Extract a quoted string value for a JSON key.
 * Finds "key":"<value>" in buf[0..len] and copies value into out[out_max].
 * Returns length of value, -1 if not found.
 */
static int json_get_str(const char *buf, unsigned int len,
                        const char *key, char *out, int out_max)
{
    /* Build search pattern: "key":"  */
    char pat[64];
    rt_snprintf(pat, sizeof(pat), "\"%s\":\"", key);
    const char *p = strstr(buf, pat);
    if (!p || (unsigned int)(p - buf) >= len) return -1;
    p += rt_strlen(pat);
    const char *e = p;
    /* Find closing quote (base64 and typical JSON values don't contain '"') */
    while (*e && *e != '"' && (unsigned int)(e - buf) < len) e++;
    int vlen = (int)(e - p);
    if (vlen <= 0 || vlen >= out_max) return -1;
    rt_memcpy(out, p, vlen);
    out[vlen] = '\0';
    return vlen;
}

/* ---- WebSocket callbacks (called from librws internal thread) ---- */

#if AI_USE_WSS_MODE

static void ws_on_connected(rws_socket sock)
{
    ai_ws_ctx_t *ctx = (ai_ws_ctx_t *)rws_socket_get_user_object(sock);
    rt_kprintf("[AIPrinter] WS connected\n");
    rt_sem_release(ctx->connected_sem);
}

static void ws_on_disconnected(rws_socket sock)
{
    ai_ws_ctx_t *ctx = (ai_ws_ctx_t *)rws_socket_get_user_object(sock);
    if (!ctx || !ctx->sock || ctx->disconnecting) return;

    rws_error err = rws_socket_get_error(sock);
    if (err) {
        rt_kprintf("[AIPrinter] WS disconnected (error %d: %s)\n",
                   rws_error_get_code(err), rws_error_get_description(err));
    } else {
        rt_kprintf("[AIPrinter] WS disconnected\n");
    }
    /* If no result yet, signal done with error so main task doesn't hang */
    if (!ctx->got_result) {
        ctx->error = 1;
        rt_sem_release(ctx->done_sem);
    }
}

static void ws_on_text(rws_socket sock, const char *text, unsigned int len)
{
    ai_ws_ctx_t *ctx = (ai_ws_ctx_t *)rws_socket_get_user_object(sock);
    if (!ctx || !ctx->sock || ctx->disconnecting || !text || len == 0) return;

    /* Extract "type" field */
    char type[32] = {0};
    if (json_get_str(text, len, "type", type, sizeof(type)) < 0) return;

    if (rt_strcmp(type, "asr_partial") == 0) {
        char txt[128] = {0};
        if (json_get_str(text, len, "text", txt, sizeof(txt)) > 0)
            rt_kprintf("[AIPrinter] ~~ %s\n", txt);

    } else if (rt_strcmp(type, "asr") == 0) {
        char txt[256] = {0};
        if (json_get_str(text, len, "text", txt, sizeof(txt)) > 0)
            rt_kprintf("[AIPrinter] === ASR: %s ===\n", txt);

    } else if (rt_strcmp(type, "stage") == 0) {
        char msg[128] = {0};
        if (json_get_str(text, len, "message", msg, sizeof(msg)) > 0)
            rt_kprintf("[AIPrinter] >> %s\n", msg);

    } else if (rt_strcmp(type, "print") == 0) {
        /* "data" field holds ESC/POS base64 — can be up to 100KB */
        const char *key = "\"data\":\"";
        const char *p = strstr(text, key);
        if (p && ctx->b64) {
            p += rt_strlen(key);
            const char *e = p;
            while (*e && *e != '"') e++;
            int dlen = (int)(e - p);
            if (dlen > 0 && dlen <= AI_B64_BUF_MAX) {
                rt_memcpy(ctx->b64, p, dlen);
                ctx->b64[dlen] = '\0';
                ctx->b64_len = dlen;
                rt_kprintf("[AIPrinter] Got print data (%d chars)\n", dlen);
            }
        }
        ctx->got_result = 1;
        rt_sem_release(ctx->done_sem);

    } else if (rt_strcmp(type, "complete") == 0) {
        /* No print data (hasScreen=false, needPrint=true should give print, not complete first) */
        rt_kprintf("[AIPrinter] WS complete (no print data)\n");
        ctx->got_result = 1;
        rt_sem_release(ctx->done_sem);

    } else if (rt_strcmp(type, "error") == 0) {
        char msg[256] = {0};
        json_get_str(text, len, "message", msg, sizeof(msg));
        rt_kprintf("[AIPrinter] WS server error: %s\n", msg);
        ctx->error = 1;
        rt_sem_release(ctx->done_sem);
    }
}

#endif /* AI_USE_WSS_MODE */

/* ========================= Voice WebSocket Print ========================= */

#if AI_USE_WSS_MODE
/*
 * Connect to /ws/voicePrint, stream PCM while button held, send stop,
 * wait for ESC/POS print data, forward to BLE printer.
 */
static void ai_voice_ws_print(void)
{
    rws_socket sock = RT_NULL;
    rt_tick_t t_start = rt_tick_get();

    /* Drain semaphores from any previous session (e.g. disconnect during cleanup) */
    while (rt_sem_take(g_ws_ctx.connected_sem, 0) == RT_EOK) {}
    while (rt_sem_take(g_ws_ctx.done_sem,      0) == RT_EOK) {}

    /* Build WS path with query params (token first, then image params) */
    char path[512];
    rt_snprintf(path, sizeof(path),
                "%s?token=%s"
                "&language=%s&format=pcm&sampleRate=%d"
                "&printMode=image&hasScreen=false&needPrint=true"
                "&isEscpos=true&imageWidth=384&imageHeight=384&style=sketch",
                AI_WS_PATH, AI_DEVICE_TOKEN, AI_LANGUAGE, AI_SAMPLE_RATE);

    /* Allocate print data buffer */
    g_ws_ctx.b64 = rt_malloc(AI_B64_BUF_MAX + 1);
    if (!g_ws_ctx.b64) {
        rt_kprintf("[AIPrinter] OOM: cannot allocate b64 buffer\n");
        return;
    }
    g_ws_ctx.b64_len    = 0;
    g_ws_ctx.got_result = 0;
    g_ws_ctx.error      = 0;
    g_ws_ctx.disconnecting = 0;

    /* Create and configure WebSocket */
    sock = rws_socket_create();
    if (!sock) {
        rt_kprintf("[AIPrinter] rws_socket_create failed\n");
        goto cleanup;
    }

    rws_socket_set_url(sock, g_ws_secure ? "wss" : "ws",
                       g_ws_host, g_ws_port, path);
    rws_socket_set_on_connected(sock,     ws_on_connected);
    rws_socket_set_on_disconnected(sock,  ws_on_disconnected);
    rws_socket_set_on_received_text(sock, ws_on_text);
    rws_socket_set_user_object(sock, &g_ws_ctx);
    g_ws_ctx.sock = sock;  /* Store socket reference for callback checks */

    rt_kprintf("[AIPrinter] Connecting %s://%s:%d%s ...\n",
               g_ws_secure ? "wss" : "ws", g_ws_host, g_ws_port, AI_WS_PATH);

    if (rws_socket_connect(sock) == rws_false) {
        rt_kprintf("[AIPrinter] rws_socket_connect() returned false\n");
        goto cleanup;
    }

    /* Wait for WS handshake */
    if (rt_sem_take(g_ws_ctx.connected_sem, AI_WS_CONNECT_MS) != RT_EOK) {
        rt_kprintf("[AIPrinter] WS connect timeout (%dms)\n", AI_WS_CONNECT_MS);
        goto cleanup;
    }
    rt_kprintf("[AIPrinter] WS ready (%ums)\n",
               (unsigned)(rt_tick_get() - t_start));

    /* ---- Open mic and stream PCM frames ---- */
    rt_device_t mic = rt_device_find("mic");
    if (!mic) {
        rt_kprintf("[AIPrinter] mic device not found!\n");
        rws_socket_send_text(sock, "{\"type\":\"stop\"}");
        goto wait_result;
    }
    if (rt_device_open(mic, RT_DEVICE_OFLAG_RDONLY) != RT_EOK) {
        rt_kprintf("[AIPrinter] Failed to open mic!\n");
        rws_socket_send_text(sock, "{\"type\":\"stop\"}");
        goto wait_result;
    }

    {
        uint8_t frame[AI_PCM_FRAME];
        int total = 0;

        rt_kprintf("[AIPrinter] Streaming PCM (release button to stop)...\n");

        while (g_btn_held && total < AI_PCM_MAX_BYTES) {
            int n = rt_device_read(mic, 0, frame, sizeof(frame));
            if (n > 0) {
                /* Send each chunk as a complete binary WS message (opcode=2, fin=true) */
                rws_socket_send_bin(sock, frame, (size_t)n, 2, rws_true);
                total += n;
            } else {
                rt_thread_delay(2);
            }
        }

        rt_device_close(mic);
        rt_kprintf("[AIPrinter] PCM stream done: %d bytes (%dms)\n",
                   total, total * 1000 / (AI_SAMPLE_RATE * 2));

        if (total < AI_PCM_MIN_BYTES) {
            rt_kprintf("[AIPrinter] Recording too short, aborting.\n");
            rws_socket_send_text(sock, "{\"type\":\"stop\"}");
            g_ws_ctx.error = 1;
            goto cleanup;
        }
    }

    /* Signal end of recording */
    rws_socket_send_text(sock, "{\"type\":\"stop\"}");
    rt_kprintf("[AIPrinter] Sent stop, waiting for result...\n");

wait_result:
    /* Wait for print/complete/error from server (up to 30s for image gen) */
    if (rt_sem_take(g_ws_ctx.done_sem, AI_WS_RESULT_MS) != RT_EOK) {
        rt_kprintf("[AIPrinter] WS result timeout (%dms)\n", AI_WS_RESULT_MS);
        g_ws_ctx.error = 1;
    }

    rt_kprintf("[AIPrinter] Total: %ums, error=%d, b64_len=%d\n",
               (unsigned)(rt_tick_get() - t_start), g_ws_ctx.error, g_ws_ctx.b64_len);

    /* TODO: 处理 print 数据（显示到屏幕/发送到打印机） */
    if (!g_ws_ctx.error && g_ws_ctx.b64_len > 0) {
        rt_kprintf("[AIPrinter] Got print data (%d bytes)\n", g_ws_ctx.b64_len);
    }

cleanup:
    g_ws_ctx.disconnecting = 1;  /* Signal callbacks to exit early */
    if (sock) {
        rws_socket_disconnect_and_release(sock);
    }
    if (g_ws_ctx.b64) {
        rt_free(g_ws_ctx.b64);
        g_ws_ctx.b64 = RT_NULL;
    }
    g_ws_ctx.sock = RT_NULL;  /* Clear socket reference */
}
#endif /* AI_USE_WSS_MODE */

/* ========================= HTTP Print (Upload + Poll) ========================= */

/*
 * Single-step SSE streaming flow:
 *   POST /luckypod/aiPrinter/voiceStreamPrint  (multipart/form-data)
 *   → Server pushes SSE events: asr, stage, print, complete, error
 *   → Process each event as it arrives, no need to poll or store full response
 */
static void ai_voice_http_print(void)
{
    rt_device_t mic = RT_NULL;
    uint8_t *pcm_buf = RT_NULL;
    int total_pcm = 0;
    rt_tick_t t_start = rt_tick_get();

    /* Step 0: Record PCM locally */
    extern void lcd_clear(rt_uint16_t color);
    lcd_clear(0);  /* 黑屏 */

    mic = rt_device_find("mic");
    if (!mic) {
        rt_kprintf("[AIPrinter] mic device not found!\n");
        goto cleanup;
    }
    if (rt_device_open(mic, RT_DEVICE_OFLAG_RDONLY) != RT_EOK) {
        rt_kprintf("[AIPrinter] Failed to open mic!\n");
        goto cleanup;
    }

    pcm_buf = rt_malloc(AI_PCM_MAX_BYTES);
    if (!pcm_buf) {
        rt_kprintf("[AIPrinter] OOM: cannot allocate PCM buffer\n");
        rt_device_close(mic);
        goto cleanup;
    }

    rt_kprintf("[AIPrinter] Recording PCM (release button to stop)...\n");
    {
        uint8_t frame[AI_PCM_FRAME];
        while (g_btn_held && total_pcm < AI_PCM_MAX_BYTES) {
            int n = rt_device_read(mic, 0, frame, sizeof(frame));
            if (n > 0) {
                rt_memcpy(pcm_buf + total_pcm, frame, n);
                total_pcm += n;
            } else {
                rt_thread_delay(2);
            }
        }
    }
    rt_device_close(mic);
    rt_kprintf("[AIPrinter] PCM recorded: %d bytes (%dms)\n",
               total_pcm, (unsigned)(rt_tick_get() - t_start));

    if (total_pcm < AI_PCM_MIN_BYTES) {
        rt_kprintf("[AIPrinter] Recording too short, aborting.\n");
        goto cleanup;
    }

    /* Step 1: Build multipart/form-data body and POST to SSE endpoint
     *
     * IMPORTANT: All headers (Content-Type, Content-Length) must be added
     * BEFORE webclient_post(), which calls webclient_send_header() that
     * terminates the header block with \r\n\r\n. Headers added after
     * webclient_post() end up in the body, causing Tomcat's multipart
     * parser to see "Content-Type: ..." as body data and fail with
     * "Stream ended unexpectedly".
     */
    {
        char boundary[64];
        rt_snprintf(boundary, sizeof(boundary), "----WebKitFormBoundary%08X", (unsigned)rt_tick_get());

        /* Pre-calculate part sizes for Content-Length */
        char part_printMode[128], part_hasScreen[64], part_isEscpos[64], part_audio_hdr[256], part_language[128], part_close[32];

        int part_printMode_len = rt_snprintf(part_printMode, sizeof(part_printMode),
                               "--%s\r\n"
                               "Content-Disposition: form-data; name=\"printMode\"\r\n"
                               "\r\n"
                               "image\r\n", boundary);
        int part_hasScreen_len = rt_snprintf(part_hasScreen, sizeof(part_hasScreen),
                               "--%s\r\n"
                               "Content-Disposition: form-data; name=\"hasScreen\"\r\n"
                               "\r\n"
                               "true\r\n", boundary);
        int part_isEscpos_len = rt_snprintf(part_isEscpos, sizeof(part_isEscpos),
                               "--%s\r\n"
                               "Content-Disposition: form-data; name=\"isEscpos\"\r\n"
                               "\r\n"
                               "true\r\n", boundary);
        int part_audio_hdr_len = rt_snprintf(part_audio_hdr, sizeof(part_audio_hdr),
                               "--%s\r\n"
                               "Content-Disposition: form-data; name=\"audioFile\"; filename=\"audio.pcm\"\r\n"
                               "Content-Type: application/octet-stream\r\n"
                               "\r\n", boundary);
        int part_language_len = rt_snprintf(part_language, sizeof(part_language),
                               "--%s\r\n"
                               "Content-Disposition: form-data; name=\"language\"\r\n"
                               "\r\n"
                               "%s\r\n", boundary, AI_LANGUAGE);
        int part_close_len = rt_snprintf(part_close, sizeof(part_close),
                               "--%s--\r\n", boundary);

        int content_len = part_printMode_len + part_hasScreen_len + part_isEscpos_len +
                          part_audio_hdr_len + total_pcm + part_language_len + part_close_len;

        char upload_url[256];
        rt_snprintf(upload_url, sizeof(upload_url),
                    "%s://%s:%d%s",
                    g_ws_secure ? "https" : "http",
                    g_ws_host, g_ws_port, AI_HTTP_UPLOAD_PATH);

        struct webclient_session *session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
        if (!session) {
            rt_kprintf("[AIPrinter] OOM: cannot create webclient session\n");
            goto cleanup;
        }
        webclient_set_timeout(session, 30000);
        rt_kprintf("[AIPrinter] SSE POST to %s (Content-Length=%d)\n", upload_url, content_len);

        /* ALL headers must be added BEFORE webclient_post() — this is the root cause fix.
         * webclient_post() → webclient_send_header() terminates the HTTP header block.
         * Anything added after that ends up in the request body, breaking multipart parsing.
         *
         * NOTE: Do NOT send "Connection: close" — it causes nginx (proxying to Tomcat)
         * to close the upstream connection prematurely when the device sends slowly
         * (21+ seconds for PCM data), resulting in "Stream ended unexpectedly" on the server. */
        webclient_header_fields_add(session, "Authorization: Bearer %s\r\n", AI_DEVICE_TOKEN);
        webclient_header_fields_add(session, "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
        webclient_header_fields_add(session, "Content-Length: %d\r\n", content_len);

        /* webclient_post(session, url, NULL, 0) connects and sends ALL headers (with \r\n\r\n terminator).
         * It does NOT send any body data since post_data is NULL. */
        rt_tick_t t_http = rt_tick_get();
        int status = webclient_post(session, upload_url, RT_NULL, 0);
        rt_kprintf("[AIPrinter] POST connect returned %d (took %dms)\n", status, (int)(rt_tick_get() - t_http));
        if (status < 0) {
            rt_kprintf("[AIPrinter] HTTP connect failed\n");
            webclient_close(session);
            goto cleanup;
        }

        /* Now write multipart body parts — headers are already sent, these go into body only.
         * Each write is checked to avoid partial/truncated body. */
        int total_sent = 0;
        int w;
        rt_tick_t t_part;

        /* Hex dump the first part to verify multipart format on wire */
        rt_kprintf("[AIPrinter] HEX first 128 bytes of printMode:\n");
        for (int i = 0; i < part_printMode_len && i < 128; i += 16) {
            char hex[48], ascii[17] = {0};
            int hex_pos = 0, ascii_pos = 0;
            for (int j = 0; j < 16 && (i + j) < part_printMode_len; j++) {
                hex_pos += rt_snprintf(hex + hex_pos, sizeof(hex) - hex_pos, "%02x ", part_printMode[i + j]);
                unsigned char c = part_printMode[i + j];
                ascii[ascii_pos++] = (c >= 0x20 && c < 0x7f) ? c : '.';
            }
            rt_kprintf("  %04x  %-48s  %s\n", i, hex, ascii);
        }

        t_part = rt_tick_get();
        w = webclient_write(session, (const uint8_t *)part_printMode, part_printMode_len);
        rt_kprintf("[AIPrinter] [1/7] printMode: sent=%d/%d, %dms\n", w, part_printMode_len, (int)(rt_tick_get() - t_part));
        if (w != part_printMode_len) { rt_kprintf("[AIPrinter] send printMode failed (%d/%d)\n", w, part_printMode_len); webclient_close(session); goto cleanup; }
        total_sent += w;

        t_part = rt_tick_get();
        w = webclient_write(session, (const uint8_t *)part_hasScreen, part_hasScreen_len);
        rt_kprintf("[AIPrinter] [2/7] hasScreen: sent=%d/%d, %dms\n", w, part_hasScreen_len, (int)(rt_tick_get() - t_part));
        if (w != part_hasScreen_len) { rt_kprintf("[AIPrinter] send hasScreen failed (%d/%d)\n", w, part_hasScreen_len); webclient_close(session); goto cleanup; }
        total_sent += w;

        t_part = rt_tick_get();
        w = webclient_write(session, (const uint8_t *)part_isEscpos, part_isEscpos_len);
        rt_kprintf("[AIPrinter] [3/7] isEscpos: sent=%d/%d, %dms\n", w, part_isEscpos_len, (int)(rt_tick_get() - t_part));
        if (w != part_isEscpos_len) { rt_kprintf("[AIPrinter] send isEscpos failed (%d/%d)\n", w, part_isEscpos_len); webclient_close(session); goto cleanup; }
        total_sent += w;

        t_part = rt_tick_get();
        w = webclient_write(session, (const uint8_t *)part_audio_hdr, part_audio_hdr_len);
        rt_kprintf("[AIPrinter] [4/7] audio_hdr: sent=%d/%d, %dms (total=%d)\n", w, part_audio_hdr_len, (int)(rt_tick_get() - t_part), total_sent + w);
        if (w != part_audio_hdr_len) { rt_kprintf("[AIPrinter] send audio_hdr failed (%d/%d)\n", w, part_audio_hdr_len); webclient_close(session); goto cleanup; }
        total_sent += w;

        {
            /* Send PCM in 2KB chunks — keeps write buffers small */
            int offset = 0;
            int pcm_chunks = 0;
            rt_tick_t t_pcm_start = rt_tick_get();
            while (offset < total_pcm) {
                int chunk = total_pcm - offset;
                if (chunk > 2048) chunk = 2048;
                w = webclient_write(session, pcm_buf + offset, chunk);
                if (w != chunk) {
                    rt_kprintf("[AIPrinter] [5/7] PCM chunk %d FAILED at offset %d (got %d/%d), total_sent=%d\n",
                               pcm_chunks, offset, w, chunk, total_sent);
                    webclient_close(session);
                    goto cleanup;
                }
                offset += w;
                total_sent += w;
                pcm_chunks++;
                /* 每 5 个 chunk 报告一次进度 */
                if (pcm_chunks % 5 == 0 || offset >= total_pcm) {
                    rt_kprintf("[AIPrinter] [5/7] PCM: %d/%d bytes (%d chunks, %dms)\n",
                               offset, total_pcm, pcm_chunks, (int)(rt_tick_get() - t_pcm_start));
                }
            }
        }

        t_part = rt_tick_get();
        w = webclient_write(session, (const uint8_t *)part_language, part_language_len);
        rt_kprintf("[AIPrinter] [6/7] language: sent=%d/%d, %dms (total=%d)\n", w, part_language_len, (int)(rt_tick_get() - t_part), total_sent + w);
        if (w != part_language_len) { rt_kprintf("[AIPrinter] send language failed (%d/%d)\n", w, part_language_len); webclient_close(session); goto cleanup; }
        total_sent += w;

        t_part = rt_tick_get();
        w = webclient_write(session, (const uint8_t *)part_close, part_close_len);
        rt_kprintf("[AIPrinter] [7/7] close: sent=%d/%d, %dms (total=%d)\n", w, part_close_len, (int)(rt_tick_get() - t_part), total_sent + w);
        if (w != part_close_len) { rt_kprintf("[AIPrinter] send close failed (%d/%d)\n", w, part_close_len); webclient_close(session); goto cleanup; }
        total_sent += w;

        rt_kprintf("[AIPrinter] Multipart body sent COMPLETE, total=%dB (expected %d, match=%s)\n",
                   total_sent, content_len, (total_sent == content_len) ? "YES" : "NO");

        /* Parse response headers before reading SSE body */
        extern int webclient_handle_response(struct webclient_session *session);
        t_part = rt_tick_get();
        rt_kprintf("[AIPrinter] Calling webclient_handle_response...\n");
        int resp = webclient_handle_response(session);
        rt_kprintf("[AIPrinter] HTTP response status: %d (waited %dms)\n", resp, (int)(rt_tick_get() - t_part));
        if (resp != 200) {
            rt_kprintf("[AIPrinter] Unexpected HTTP status %d\n", resp);
            /* Read full response body to see actual error from server */
            char err_buf[2048];
            int err_total = 0;
            while (err_total < sizeof(err_buf) - 1) {
                int err_len = webclient_read(session, err_buf + err_total, sizeof(err_buf) - 1 - err_total);
                if (err_len <= 0) break;
                err_total += err_len;
            }
            err_buf[err_total] = '\0';
            rt_kprintf("[AIPrinter] Server error body (%d bytes): %s\n", err_total, err_buf);
            webclient_close(session);
            goto cleanup;
        }
        {
            char line_buf[512];
            int line_pos = 0;
            char last_event[32] = {0};
            int read_timeout = 0;
            int expecting_image_data = 0;  /* After "event:image", next data: line is image */
            int streaming_image = 0;       /* Currently streaming base64 bytes directly */
            int image_skip_data = 0;       /* Skip "data:" prefix bytes */

            /* Buffer for accumulating image base64 data */
            char *img_b64_buf = RT_NULL;
            int img_b64_len = 0;
            int img_b64_cap = 0;

            /* Read buffer for chunked reads */
            uint8_t read_buf[1024];
            int read_pos = 0, read_avail = 0;

            while (read_timeout < 120) {  /* 2 min timeout for SSE */
                /* Refill read buffer */
                if (read_pos >= read_avail) {
                    int n = webclient_read(session, read_buf, sizeof(read_buf));
                    if (n < 0) {
                        rt_kprintf("[AIPrinter] SSE read error %d\n", n);
                        break;
                    }
                    if (n == 0) {
                        read_timeout++;
                        rt_thread_delay(100);
                        continue;
                    }
                    read_timeout = 0;
                    read_pos = 0;
                    read_avail = n;
                }

                uint8_t byte = read_buf[read_pos++];

                /* ---- Streaming image base64 directly (no line_buf) ---- */
                if (streaming_image) {
                    if (byte == '\n' || byte == '\r') {
                        /* End of image data line — null-terminate for safety */
                        if (img_b64_buf && img_b64_len < img_b64_cap) {
                            img_b64_buf[img_b64_len] = '\0';
                        }
                        streaming_image = 0;
                        image_skip_data = 0;
                        rt_kprintf("[AIPrinter] SSE [image]: total %d bytes\n", img_b64_len);
                        /* Skip empty lines (SSE event separator) */
                    } else {
                        /* Accumulate base64 byte directly */
                        if (image_skip_data < 5) {
                            /* Skip "data: " prefix */
                            image_skip_data++;
                        } else {
                            if (img_b64_len + 1 >= img_b64_cap) {
                                img_b64_cap = (img_b64_len + 1024) * 2;
                                if (img_b64_cap > 200000) img_b64_cap = 200000;  /* cap at 300KB */
                                img_b64_buf = rt_realloc(img_b64_buf, img_b64_cap);
                            }
                            if (img_b64_buf) {
                                img_b64_buf[img_b64_len++] = (char)byte;
                            }
                        }
                    }
                    continue;
                }

                /* ---- Normal line-by-line SSE parsing ---- */
                if (byte == '\n' || byte == '\r') {
                    if (line_pos > 0) {
                        line_buf[line_pos] = '\0';
                        rt_kprintf("[AIPrinter] SSE [%s]: %.100s\n", last_event, line_buf);

                        /* Parse SSE event + data */
                        if (rt_strncmp(line_buf, "event:", 6) == 0) {
                            char *p = line_buf + 6;
                            while (*p == ' ') p++;
                            rt_strncpy(last_event, p, sizeof(last_event) - 1);
                            if (rt_strcmp(last_event, "image") == 0) {
                                expecting_image_data = 1;
                            }
                        } else if (rt_strncmp(line_buf, "data:", 5) == 0) {
                            const char *data = line_buf + 5;
                            while (*data == ' ') data++;

                            if (expecting_image_data) {
                                /* Shouldn't reach here — streaming_image handles it —
                                   but fallback for short data: lines */
                                expecting_image_data = 0;
                                int dlen = rt_strlen(data);
                                if (dlen > 0 && img_b64_len + dlen < 300000) {
                                    if (img_b64_len + dlen >= img_b64_cap) {
                                        img_b64_cap = (img_b64_len + dlen + 1) * 2;
                                        if (img_b64_cap > 200000) img_b64_cap = 200000;
                                        img_b64_buf = rt_realloc(img_b64_buf, img_b64_cap);
                                    }
                                    if (img_b64_buf) {
                                        rt_memcpy(img_b64_buf + img_b64_len, data, dlen);
                                        img_b64_len += dlen;
                                        img_b64_buf[img_b64_len] = '\0';
                                    }
                                }
                            } else if (rt_strcmp(last_event, "asr") == 0) {
                                char text[256] = {0};
                                json_get_str((char *)data, rt_strlen(data), "text", text, sizeof(text));
                                if (text[0]) rt_kprintf("[AIPrinter] ASR: %s\n", text);
                            } else if (rt_strcmp(last_event, "stage") == 0) {
                                char msg[128] = {0};
                                json_get_str((char *)data, rt_strlen(data), "message", msg, sizeof(msg));
                                if (msg[0]) rt_kprintf("[AIPrinter] Stage: %s\n", msg);
                            } else if (rt_strcmp(last_event, "complete") == 0) {
                                rt_kprintf("[AIPrinter] Complete! img_b64: %d chars\n", img_b64_len);
                                int img_w = 384, img_h = 384;
                                json_get_int(data, rt_strlen(data), "width", &img_w);
                                json_get_int(data, rt_strlen(data), "height", &img_h);
                                rt_kprintf("[AIPrinter] Image: %dx%d (JPEG)\n", img_w, img_h);
                                char prompt[128] = {0};
                                json_get_str((char *)data, rt_strlen(data), "imagePrompt", prompt, sizeof(prompt));
                                if (prompt[0]) rt_kprintf("[AIPrinter] Prompt: %s\n", prompt);
                            } else if (rt_strcmp(last_event, "error") == 0) {
                                char msg[256] = {0};
                                json_get_str((char *)data, rt_strlen(data), "message", msg, sizeof(msg));
                                rt_kprintf("[AIPrinter] SSE error: %s\n", msg);
                                g_ws_ctx.error = 1;
                            }
                        }

                        line_pos = 0;
                    }
                    /* Skip empty lines (SSE event separator) */
                } else {
                    if (expecting_image_data) {
                        /* First byte of "data:" line — switch to streaming mode */
                        expecting_image_data = 0;
                        streaming_image = 1;
                        image_skip_data = 0;
                        /* Re-process this byte in streaming mode */
                        if (streaming_image) {
                            if (image_skip_data < 5) {
                                image_skip_data++;
                            } else {
                                if (img_b64_len + 1 >= img_b64_cap) {
                                    img_b64_cap = (img_b64_len + 1024) * 2;
                                    if (img_b64_cap > 200000) img_b64_cap = 200000;
                                    img_b64_buf = rt_realloc(img_b64_buf, img_b64_cap);
                                }
                                if (img_b64_buf) {
                                    img_b64_buf[img_b64_len++] = (char)byte;
                                }
                            }
                        }
                    } else if (line_pos < (int)sizeof(line_buf) - 1) {
                        line_buf[line_pos++] = (char)byte;
                    }
                }

                if (g_ws_ctx.error) break;
            }

            if (read_timeout >= 120) {
                rt_kprintf("[AIPrinter] SSE read timeout\n");
                g_ws_ctx.error = 1;
            }

            /* Transfer accumulated image data to global context */
            if (img_b64_len > 0 && img_b64_buf) {
                rt_kprintf("[AIPrinter] Got JPEG base64: %d chars\n", img_b64_len);
                g_ws_ctx.b64 = img_b64_buf;
                g_ws_ctx.b64_len = img_b64_len;
                g_ws_ctx.got_result = 1;
                img_b64_buf = RT_NULL;
            }

            if (img_b64_buf) rt_free(img_b64_buf);
        }

        rt_kprintf("[AIPrinter] Total: %ums, error=%d, b64_len=%d\n",
                   (unsigned)(rt_tick_get() - t_start), g_ws_ctx.error, g_ws_ctx.b64_len);

        webclient_close(session);
    }

cleanup:
    if (pcm_buf) rt_free(pcm_buf);
    if (mic) rt_device_close(mic);
}

/* ========================= Test Image Print (POST /testImagePrint) ========================= */

/* Beken built-in base64 decoder — see beken378/func/base64/base_64.c */
extern unsigned char base64_decode(const unsigned char *src, int len,
                                   int *out_len, unsigned char *out);

/* LCD APIs — see drivers/drv_st7789_lcd.c */
extern void lcd_clear(rt_uint16_t color);
extern void lcd_show_grayscale(unsigned short x, unsigned short y,
                               unsigned short w, unsigned short h,
                               const unsigned char *gray);

/* URL-encode src into dst. Returns bytes written (excluding NUL). */
static int url_encode(const char *src, char *dst, int dst_max)
{
    static const char hex[] = "0123456789ABCDEF";
    int i = 0;
    while (*src && i + 4 < dst_max) {
        unsigned char c = (unsigned char)*src++;
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            dst[i++] = (char)c;
        } else {
            dst[i++] = '%';
            dst[i++] = hex[c >> 4];
            dst[i++] = hex[c & 0xF];
        }
    }
    dst[i] = '\0';
    return i;
}

/* Test image-only flow:
 *   POST /testImagePrint  body=lang=zh&isEscpos=false&hasScreen=true&imageWidth=W&imageHeight=H&style=...&prompt=...
 *   → response JSON has imageDataBase64 (raw 8-bit grayscale, base64-encoded)
 *   → decode base64 → push to LCD via lcd_show_grayscale
 *
 * img_w/img_h must match the screen size budget (≤ 240×240 for our 240×320 panel).
 */
static int ai_test_image_print(const char *prompt, const char *style,
                               int img_w, int img_h)
{
    if (!prompt || !prompt[0]) {
        rt_kprintf("[AIPrinter] testImagePrint: empty prompt\n");
        return -1;
    }

    /* Sanitise dimensions to fit on screen (LCD is 240x320 portrait) */
    if (img_w <= 0 || img_w > 240) img_w = 240;
    if (img_h <= 0 || img_h > 320) img_h = 240;

    rt_tick_t t_start = rt_tick_get();
    char  *body     = RT_NULL;
    char  *resp_buf = RT_NULL;
    rt_uint8_t *gray_buf = RT_NULL;
    struct webclient_session *session = RT_NULL;
    int rc = -1;

    /* ---- Build URL-encoded form body ---- */
    /* Worst case: prompt is all multi-byte UTF-8 (3 bytes/char → 9 chars after encoding).
     * 1.5KB body buffer is enough for typical prompts (≤ ~150 Chinese chars). */
    body = rt_malloc(1536);
    if (!body) { rt_kprintf("[AIPrinter] OOM: body buf\n"); goto out; }

    int blen = rt_snprintf(body, 1536,
        "lang=zh&isEscpos=false&hasScreen=true&imageWidth=%d&imageHeight=%d",
        img_w, img_h);
    if (style && style[0]) {
        blen += rt_snprintf(body + blen, 1536 - blen, "&style=%s", style);
    }
    blen += rt_snprintf(body + blen, 1536 - blen, "&prompt=");
    blen += url_encode(prompt, body + blen, 1536 - blen);

    /* ---- Free up RAM before allocating big buffers (kick QR / pre-cleanup) ---- */
    lcd_clear(0x0000);  /* black */

    /* ---- POST ---- */
    char url[256];
    rt_snprintf(url, sizeof(url), "%s://%s:%d/testImagePrint",
                g_ws_secure ? "https" : "http", g_ws_host, g_ws_port);
    rt_kprintf("[AIPrinter] POST %s  (body=%dB)\n", url, blen);

    session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (!session) { rt_kprintf("[AIPrinter] OOM: session\n"); goto out; }
    webclient_set_timeout(session, 60000);
    webclient_header_fields_add(session, "Content-Length: %d\r\n", blen);
    webclient_header_fields_add(session, "Content-Type: application/x-www-form-urlencoded\r\n");
    webclient_header_fields_add(session, "Authorization: Bearer %s\r\n", AI_DEVICE_TOKEN);

    int status = webclient_post(session, url, body, blen);
    if (status != 200) {
        rt_kprintf("[AIPrinter] testImagePrint failed, status=%d\n", status);
        goto out;
    }

    /* ---- Read response ----
     * Body holds JSON with imageDataBase64. With isEscpos=false the response is
     * dominated by the base64 string: img_w*img_h * 4/3 ≈ image bytes.
     * Add ~4KB JSON envelope.
     */
    int max_resp = (img_w * img_h * 4 / 3) + 4096;
    if (max_resp > AI_HTTP_RESP_BUF_MAX) max_resp = AI_HTTP_RESP_BUF_MAX;
    int content_len = session->content_length;
    if (content_len <= 0 || content_len > max_resp) content_len = max_resp;

    resp_buf = rt_malloc(content_len + 1);
    if (!resp_buf) { rt_kprintf("[AIPrinter] OOM: resp_buf %d\n", content_len + 1); goto out; }

    int total_read = 0;
    while (total_read < content_len) {
        int n = webclient_read(session, resp_buf + total_read, content_len - total_read);
        if (n <= 0) break;
        total_read += n;
    }
    resp_buf[total_read] = '\0';
    webclient_close(session);
    session = RT_NULL;
    rt_kprintf("[AIPrinter] response: %d bytes (%ums)\n",
               total_read, (unsigned)(rt_tick_get() - t_start));

    /* ---- Locate imageDataBase64 value within response ---- */
    const char *key = "\"imageDataBase64\":\"";
    char *p = strstr(resp_buf, key);
    if (!p) {
        rt_kprintf("[AIPrinter] imageDataBase64 not found. snippet: %.200s\n", resp_buf);
        goto out;
    }
    p += rt_strlen(key);
    char *e = p;
    while (*e && *e != '"') e++;
    if (*e != '"') { rt_kprintf("[AIPrinter] imageDataBase64 unterminated\n"); goto out; }
    int b64_len = (int)(e - p);
    rt_kprintf("[AIPrinter] imageDataBase64: %d chars\n", b64_len);

    /* ---- Decode base64 → grayscale buffer ---- */
    int gray_size = img_w * img_h;
    gray_buf = rt_malloc(gray_size);
    if (!gray_buf) { rt_kprintf("[AIPrinter] OOM: gray %d\n", gray_size); goto out; }

    int decoded = 0;
    if (!base64_decode((const unsigned char *)p, b64_len, &decoded, gray_buf)) {
        rt_kprintf("[AIPrinter] base64 decode failed\n");
        goto out;
    }
    rt_kprintf("[AIPrinter] decoded %d bytes (expect %d)\n", decoded, gray_size);
    if (decoded < gray_size) {
        rt_kprintf("[AIPrinter] WARN: short decode, padding may distort image\n");
    }

    /* Free response buffer ASAP — frees ~80KB before we touch the LCD SPI loop */
    rt_free(resp_buf);
    resp_buf = RT_NULL;

    /* ---- Display centred on screen ---- */
    rt_uint16_t x = (240 > img_w) ? (240 - img_w) / 2 : 0;
    rt_uint16_t y = (320 > img_h) ? (320 - img_h) / 2 : 0;
    lcd_show_grayscale(x, y, (rt_uint16_t)img_w, (rt_uint16_t)img_h, gray_buf);

    rt_kprintf("[AIPrinter] image displayed at (%d,%d) %dx%d (total %ums)\n",
               x, y, img_w, img_h, (unsigned)(rt_tick_get() - t_start));
    rc = 0;

out:
    if (session)  webclient_close(session);
    if (body)     rt_free(body);
    if (resp_buf) rt_free(resp_buf);
    if (gray_buf) rt_free(gray_buf);
    return rc;
}

/* MSH command: testimg <prompt> [style] [size]
 *   style: sketch / anime / cartoon  (default: server picks)
 *   size:  pixel side (default 240, capped to 240 wide / 320 tall)
 *   e.g.   testimg "可爱的小猫" sketch 240
 */
static int cmd_testimg(int argc, char **argv)
{
    if (argc < 2) {
        rt_kprintf("Usage: testimg <prompt> [style] [size]\n");
        rt_kprintf("  e.g. testimg \"a cute cat\" sketch 240\n");
        rt_kprintf("       testimg 可爱的小猫\n");
        return 0;
    }
    const char *prompt = argv[1];
    const char *style  = (argc >= 3) ? argv[2] : RT_NULL;
    int size           = (argc >= 4) ? atoi(argv[3]) : 240;
    return ai_test_image_print(prompt, style, size, size);
}
MSH_CMD_EXPORT_ALIAS(cmd_testimg, testimg, test text-to-image API);

/* ========================= Main Task ========================= */

static void ai_printer_task(void *arg)
{
    rt_kprintf("\n[AIPrinter] ======================================\n");
    rt_kprintf("[AIPrinter]   AI Voice Printer  (BK7252N)\n");
    rt_kprintf("[AIPrinter]   Hold GPIO%d to record, release to stop\n", AI_BTN_GPIO);
    rt_kprintf("[AIPrinter] ======================================\n\n");

    /* Step 1: Connect WiFi */
    rt_kprintf("[AIPrinter] [1/3] Connecting to WiFi...\n");
    if (ai_wifi_connect() != 0) {
        rt_kprintf("[AIPrinter] WiFi failed! Check config and reboot.\n");
        return;
    }

    /* Step 1.5: NTP sync — fix system time so HTTPS/TLS certificate validation passes */
#ifdef PKG_NETUTILS_NTP
    rt_kprintf("[AIPrinter] Syncing time via NTP...\n");
    {
        /* Packages NTP uses compile-time NTP_HOSTNAME. Try with default first.
         * If it fails, the user can try `__cmd_ntp_sync` from msh.
         * We proceed anyway — server time may still be close enough. */
        if (ntp_sync_to_rtc() != 0) {
            rt_kprintf("[AIPrinter] NTP sync failed (default server), proceeding anyway\n");
        } else {
            rt_kprintf("[AIPrinter] NTP time synced\n");
        }
    }
#endif

    /* Step 2: Host selection window */
    rt_kprintf("[AIPrinter]\n");
#if AI_USE_WSS_MODE
    rt_kprintf("[AIPrinter] Mode: WSS streaming (WebSocket real-time)\n");
#else
    rt_kprintf("[AIPrinter] Mode: HTTP (upload → poll result)\n");
#endif
    rt_kprintf("[AIPrinter] ┌─ Host selection (%ds) ──────────────────────────┐\n",
               AI_HOST_SEL_MS / 1000);
    rt_kprintf("[AIPrinter] │  sethost <PC_LAN_IP>:9005  (PC LAN IP)          │\n");
    rt_kprintf("[AIPrinter] │    e.g.  sethost 192.168.1.100:9005             │\n");
    rt_kprintf("[AIPrinter] │  sethost default  → %s  │\n", AI_DEFAULT_HOST);
    rt_kprintf("[AIPrinter] └────────────────────────────────────────────────┘\n");
    rt_thread_delay(AI_HOST_SEL_MS);
    rt_kprintf("[AIPrinter] [2/3] Host: %s://%s:%d\n\n",
               g_ws_secure ? (AI_USE_WSS_MODE ? "wss" : "https")
                           : (AI_USE_WSS_MODE ? "ws" : "http"),
               g_ws_host, g_ws_port);

    /* Main loop: hold button → WS stream → print → repeat */
    while (1) {
        rt_kprintf("[AIPrinter] Ready. Press & hold GPIO%d to record.\n", AI_BTN_GPIO);
        rt_sem_take(g_btn_sem, RT_WAITING_FOREVER);

        rt_kprintf("[AIPrinter] [3/3] Hold and speak...\n");
#if AI_USE_WSS_MODE
        ai_voice_ws_print();
        rt_kprintf("[AIPrinter] Done! (WSS mode)\n\n");
#else
        ai_voice_http_print();
        rt_kprintf("[AIPrinter] Done! (HTTP mode)\n\n");
#endif

        /* Cooldown: wait for button up, drain stale triggers */
        while (g_btn_held) rt_thread_delay(10);
        while (rt_sem_take(g_btn_sem, 0) == RT_EOK) {}
        rt_thread_delay(300);
    }
}

/* ========================= Entry Point ========================= */

void ai_printer_start(void)
{
    /* Init semaphores (reused across button presses) */
#if AI_USE_WSS_MODE
    g_ws_ctx.connected_sem = rt_sem_create("ws_conn", 0, RT_IPC_FLAG_FIFO);
    g_ws_ctx.done_sem      = rt_sem_create("ws_done", 0, RT_IPC_FLAG_FIFO);
#endif
    g_ws_ctx.b64           = RT_NULL;
    g_ws_ctx.sock          = RT_NULL;

    g_btn_sem = rt_sem_create("btn_sem", 0, RT_IPC_FLAG_FIFO);
    ai_button_init();

    /* Init default host */
    ai_set_host(AI_DEFAULT_HOST);

    rt_thread_t t = rt_thread_create("ai_printer",
                                     ai_printer_task, RT_NULL,
                                     AI_TASK_STACK_SZ, AI_TASK_PRIO, 20);
    if (t) {
        rt_thread_startup(t);
    } else {
        rt_kprintf("[AIPrinter] ERROR: failed to create thread!\n");
    }
}
