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
#include "wlan_ui_pub.h"
#include "rw_msg_pub.h"
#include "gpio_pub.h"
#include "multi_button.h"
#include "ble_printer.h"

/* ========================= Configuration ========================= */
#define AI_BTN_GPIO     4   /* GPIO pin connected to trigger button (button → GND) */

/* WiFi AP list: tries each in order until one connects */
static const struct { const char *ssid; const char *pass; } AI_WIFI_LIST[] = {
    { "Xiaomi_402", "88996677"   },
    { "XMLJ",       "lj20251210" },
};

/* Server defaults */
#define AI_DEFAULT_HOST     "api.transkoi.luckjingle.com"
#define AI_DEFAULT_PORT     443
/* NOTE: To reach a local dev server use: sethost 192.168.x.x:9005 */
#define AI_HOST_SEL_MS      5000

/* Device API key — must match token.test-token in server application.properties */
#define AI_DEVICE_TOKEN     "luckypod-bk7252-2026"

/* WebSocket path */
#define AI_WS_PATH          "/ws/voicePrint"

/* Audio: 8kHz, 16-bit, mono */
#define AI_SAMPLE_RATE      8000
#define AI_LANGUAGE         "zh"
#define AI_RECORD_MAX_SECS  6
#define AI_PCM_FRAME        1600   /* 100ms @ 8kHz/16bit = 1600 bytes */
#define AI_PCM_MAX_BYTES    (AI_SAMPLE_RATE * AI_RECORD_MAX_SECS * 2)  /* 96KB */
#define AI_PCM_MIN_BYTES    (AI_SAMPLE_RATE * 1 * 2)                   /* 1s min */

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

    rt_kprintf("[AIPrinter] URL set: %s://%s:%d%s\n",
               g_ws_secure ? "wss" : "ws", g_ws_host, g_ws_port, AI_WS_PATH);
}

/* MSH command: sethost 192.168.1.100:9005  or  sethost default */
static int cmd_sethost(int argc, char **argv)
{
    if (argc < 2) {
        rt_kprintf("[AIPrinter] Usage: sethost <host:port|default>\n");
        rt_kprintf("[AIPrinter]   e.g. sethost 192.168.1.100:9005\n");
        rt_kprintf("[AIPrinter]        sethost default\n");
        rt_kprintf("[AIPrinter] Current: %s://%s:%d\n",
                   g_ws_secure ? "wss" : "ws", g_ws_host, g_ws_port);
        return 0;
    }
    const char *arg = argv[1];
    ai_set_host(rt_strcmp(arg, "default") == 0 ? AI_DEFAULT_HOST : arg);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_sethost, sethost, set AI printer API host);

/* ========================= WiFi Connect ========================= */

static int ai_wifi_connect(void)
{
    int n = sizeof(AI_WIFI_LIST) / sizeof(AI_WIFI_LIST[0]);
    for (int i = 0; i < n; i++) {
        network_InitTypeDef_st cfg;
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

/* ========================= Voice WebSocket Print ========================= */

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

    /* Build WS path with query params (auth token last) */
    char path[320];
    rt_snprintf(path, sizeof(path),
                "%s?language=%s&format=pcm&sampleRate=%d"
                "&printMode=image&hasScreen=false&needPrint=true"
                "&token=%s",
                AI_WS_PATH, AI_LANGUAGE, AI_SAMPLE_RATE, AI_DEVICE_TOKEN);

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
        ai_voice_ws_print();
        rt_kprintf("[AIPrinter] Done! Press button for next print.\n\n");

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
    g_ws_ctx.connected_sem = rt_sem_create("ws_conn", 0, RT_IPC_FLAG_FIFO);
    g_ws_ctx.done_sem      = rt_sem_create("ws_done", 0, RT_IPC_FLAG_FIFO);
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
