/*
 * AI Printer Application - BK7252N
 *
 * Flow: Boot → WiFi Connect → hold button → WebSocket stream PCM → receive print data → BLE print → loop
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
#include "wlan_ui_pub.h"
#include "rw_msg_pub.h"
#include "gpio_pub.h"
#include "multi_button.h"
#include "ble_printer.h"

/* ========================= Configuration ========================= */
#define AI_BTN_GPIO     4   /* GPIO pin connected to trigger button (button → GND) */

/* Mode switch: 0 = HTTP (default, upload → poll result), 1 = WSS streaming */
#define AI_USE_WSS_MODE 0

/* HTTP API endpoints */
#define AI_HTTP_UPLOAD_PATH  "/api/voicePrint"           /* POST PCM audio → returns task_id */
#define AI_HTTP_RESULT_PATH  "/api/voicePrint/%s/result"  /* GET task result → returns print data */

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
#define AI_DEFAULT_PORT     443
/* NOTE: To reach a local dev server use: sethost 192.168.x.x:9005 */
#define AI_HOST_SEL_MS      5000

/* Device API key — must match token.test-token in server application.properties */
#define AI_DEVICE_TOKEN     "da60e317-7114-48c6-8224-e99c750af2b9"

/* WebSocket path */
#define AI_WS_PATH          "/ws/voicePrint"

/* Audio: 16kHz, 16-bit, mono */
#define AI_SAMPLE_RATE      16000
#define AI_LANGUAGE         "zh"
#define AI_RECORD_MAX_SECS  6
#define AI_PCM_FRAME        3200   /* 100ms @ 16kHz/16bit = 3200 bytes */
#define AI_PCM_MAX_BYTES    (AI_SAMPLE_RATE * AI_RECORD_MAX_SECS * 2)  /* 192KB */
#define AI_PCM_MIN_BYTES    (AI_SAMPLE_RATE * 1 * 2)                   /* 32KB */

#define AI_B64_BUF_MAX      102400  /* max ESC/POS base64 size */

#define AI_WIFI_TIMEOUT_MS  30000
#define AI_WS_CONNECT_MS    10000  /* WS connect timeout */
#define AI_WS_RESULT_MS     30000  /* wait for complete/print after stop */

#define AI_TASK_STACK_SZ    8192
#define AI_TASK_PRIO        15

/* ========================= Runtime Host Config ========================= */

static char g_ws_host[128] = AI_DEFAULT_HOST;
static int  g_ws_port      = AI_DEFAULT_PORT;
static int  g_ws_secure    = 1;   /* 0 = ws://, 1 = wss:// */

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
    g_ws_secure = ai_is_local(g_ws_host) ? 0 : 1;

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

    /* Send to BLE printer */
    if (!g_ws_ctx.error && g_ws_ctx.b64_len > 0) {
        rt_kprintf("[AIPrinter] Sending to BLE printer...\n");
        ble_printer_send_base64(g_ws_ctx.b64);
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
 * Two-step HTTP flow:
 *   1. POST PCM audio to /api/voicePrint → returns {"taskId":"xxx"}
 *   2. GET  /api/voicePrint/{taskId}/result → polls until processing complete
 *      Returns: {"type":"print","data":"<base64>"} or {"type":"complete"} or {"type":"error","message":"..."}
 */
static void ai_voice_http_print(void)
{
    rt_device_t mic = RT_NULL;
    uint8_t *pcm_buf = RT_NULL;
    char *resp_buf = RT_NULL;
    int total_pcm = 0;
    rt_tick_t t_start = rt_tick_get();

    /* Step 0: Record PCM locally (button already held by the time we're called) */
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

    /* Step 1: POST PCM to upload endpoint */
    {
        char upload_url[256];
        rt_snprintf(upload_url, sizeof(upload_url),
                    "%s://%s:%d%s?token=%s&language=%s&format=pcm&sampleRate=%d"
                    "&printMode=image&hasScreen=false&needPrint=true"
                    "&isEscpos=true&imageWidth=384&imageHeight=384&style=sketch",
                    g_ws_secure ? "https" : "http",
                    g_ws_host, g_ws_port, AI_HTTP_UPLOAD_PATH,
                    AI_DEVICE_TOKEN, AI_LANGUAGE, AI_SAMPLE_RATE);

        struct webclient_session *session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
        if (!session) {
            rt_kprintf("[AIPrinter] OOM: cannot create webclient session\n");
            goto cleanup;
        }
        webclient_set_timeout(session, 15000);

        webclient_header_fields_add(session, "Content-Length: %d\r\n", total_pcm);
        webclient_header_fields_add(session, "Content-Type: application/octet-stream\r\n");
        webclient_header_fields_add(session, "Authorization: Bearer %s\r\n", AI_DEVICE_TOKEN);

        int status = webclient_post(session, upload_url, pcm_buf, total_pcm);
        if (status != 200) {
            rt_kprintf("[AIPrinter] HTTP upload failed, status=%d\n", status);
            webclient_close(session);
            goto cleanup;
        }

        /* Read upload response to extract taskId */
        resp_buf = rt_malloc(1024);
        if (!resp_buf) {
            rt_kprintf("[AIPrinter] OOM: cannot allocate response buffer\n");
            webclient_close(session);
            goto cleanup;
        }
        int resp_len = 0;
        int n = webclient_read(session, resp_buf, 1023);
        if (n > 0) {
            resp_buf[n] = '\0';
            resp_len = n;
        }
        webclient_close(session);

        /* Extract taskId from JSON response */
        char task_id[128] = {0};
        if (json_get_str(resp_buf, resp_len, "taskId", task_id, sizeof(task_id)) < 0) {
            rt_kprintf("[AIPrinter] HTTP upload response missing taskId: %.*s\n", resp_len, resp_buf);
            rt_free(resp_buf);
            resp_buf = RT_NULL;
            goto cleanup;
        }
        rt_free(resp_buf);
        resp_buf = RT_NULL;
        rt_kprintf("[AIPrinter] Upload OK, taskId=%s\n", task_id);

        /* Step 2: Poll GET result until processing complete */
        int poll_ms = 0;
        while (poll_ms < AI_HTTP_RESULT_TIMEOUT_MS) {
            char poll_url[384];
            rt_snprintf(poll_url, sizeof(poll_url),
                        "%s://%s:%d/api/voicePrint/%s/result?token=%s",
                        g_ws_secure ? "https" : "http",
                        g_ws_host, g_ws_port, task_id, AI_DEVICE_TOKEN);

            session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
            if (!session) {
                rt_kprintf("[AIPrinter] OOM: cannot create webclient session for poll\n");
                break;
            }
            webclient_set_timeout(session, 15000);
            webclient_header_fields_add(session, "Authorization: Bearer %s\r\n", AI_DEVICE_TOKEN);

            status = webclient_get(session, poll_url);
            if (status != 200) {
                webclient_close(session);
                rt_kprintf("[AIPrinter] HTTP poll failed, status=%d\n", status);
                break;
            }

            /* Read result — may be large (base64 print data) */
            int content_len = session->content_length > 0 ? session->content_length : AI_HTTP_RESP_BUF_MAX;
            if (content_len > AI_HTTP_RESP_BUF_MAX) content_len = AI_HTTP_RESP_BUF_MAX;

            if (!resp_buf) resp_buf = rt_malloc(content_len + 1);
            if (!resp_buf) {
                rt_kprintf("[AIPrinter] OOM: cannot allocate result buffer\n");
                webclient_close(session);
                break;
            }
            int total_read = 0;
            while (total_read < content_len) {
                n = webclient_read(session, resp_buf + total_read, content_len - total_read);
                if (n <= 0) break;
                total_read += n;
            }
            resp_buf[total_read] = '\0';
            webclient_close(session);

            /* Parse response type */
            char type[32] = {0};
            if (json_get_str(resp_buf, total_read, "type", type, sizeof(type)) < 0) {
                rt_kprintf("[AIPrinter] Poll response missing type: %.*s\n",
                           total_read > 200 ? 200 : total_read, resp_buf);
                rt_thread_delay(AI_HTTP_RESULT_POLL_MS);
                poll_ms += AI_HTTP_RESULT_POLL_MS;
                continue;
            }

            if (rt_strcmp(type, "print") == 0) {
                const char *key = "\"data\":\"";
                const char *p = strstr(resp_buf, key);
                if (p) {
                    p += rt_strlen(key);
                    const char *e = p;
                    while (*e && *e != '"') e++;
                    int dlen = (int)(e - p);
                    if (dlen > 0 && dlen <= AI_B64_BUF_MAX) {
                        g_ws_ctx.b64 = rt_malloc(dlen + 1);
                        if (g_ws_ctx.b64) {
                            rt_memcpy(g_ws_ctx.b64, p, dlen);
                            g_ws_ctx.b64[dlen] = '\0';
                            g_ws_ctx.b64_len = dlen;
                            rt_kprintf("[AIPrinter] Got print data (%d chars)\n", dlen);
                        }
                    }
                }
                g_ws_ctx.got_result = 1;
                break;
            } else if (rt_strcmp(type, "complete") == 0) {
                rt_kprintf("[AIPrinter] HTTP complete (no print data)\n");
                g_ws_ctx.got_result = 1;
                break;
            } else if (rt_strcmp(type, "error") == 0) {
                char msg[256] = {0};
                json_get_str(resp_buf, total_read, "message", msg, sizeof(msg));
                rt_kprintf("[AIPrinter] HTTP server error: %s\n", msg);
                g_ws_ctx.error = 1;
                break;
            } else if (rt_strcmp(type, "processing") == 0) {
                rt_kprintf("[AIPrinter] Server still processing...\n");
            } else {
                rt_kprintf("[AIPrinter] Unknown poll type: %s\n", type);
            }

            rt_thread_delay(AI_HTTP_RESULT_POLL_MS);
            poll_ms += AI_HTTP_RESULT_POLL_MS;
        }

        if (poll_ms >= AI_HTTP_RESULT_TIMEOUT_MS) {
            rt_kprintf("[AIPrinter] HTTP result timeout (%dms)\n", AI_HTTP_RESULT_TIMEOUT_MS);
            g_ws_ctx.error = 1;
        }

        rt_kprintf("[AIPrinter] Total: %ums, error=%d, b64_len=%d\n",
                   (unsigned)(rt_tick_get() - t_start), g_ws_ctx.error, g_ws_ctx.b64_len);

        if (!g_ws_ctx.error && g_ws_ctx.b64_len > 0) {
            rt_kprintf("[AIPrinter] Sending to BLE printer...\n");
            ble_printer_send_base64(g_ws_ctx.b64);
        }
    }

cleanup:
    if (pcm_buf) rt_free(pcm_buf);
    if (resp_buf) rt_free(resp_buf);
    if (mic) rt_device_close(mic);
}

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
               g_ws_secure ? "wss" : "ws", g_ws_host, g_ws_port);

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
    ble_printer_init();

    /* Init default host */
    ai_set_host(AI_DEFAULT_HOST);

    /* Start BLE connection in background */
    rt_thread_t bt = rt_thread_create("ble_conn",
                                      ble_printer_connect_task,
                                      RT_NULL, 2048, AI_TASK_PRIO + 1, 20);
    if (bt) rt_thread_startup(bt);

    rt_thread_t t = rt_thread_create("ai_printer",
                                     ai_printer_task, RT_NULL,
                                     AI_TASK_STACK_SZ, AI_TASK_PRIO, 20);
    if (t) {
        rt_thread_startup(t);
    } else {
        rt_kprintf("[AIPrinter] ERROR: failed to create thread!\n");
    }
}
