/*
 * AI Printer Application - BK7252N
 *
 * Flow: Boot → WiFi Connect → wait button → Record (6s) → ASR API → AI Print API → loop
 *
 * Usage: Press trigger button (GPIO AI_BTN_GPIO) to start a new print cycle.
 * All progress is printed to the serial port (UART).
 */

#include <rtthread.h>
#include <webclient.h>
#include <cJSON.h>
#include <string.h>
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

/* ASR: POST multipart/form-data with audio file */
#define AI_ASR_URL          "https://api.transkoi.luckjingle.com/api/apis/thirdParty/asr"

/* AI Print: POST with JSON body {"prompt":"..."} */
#define AI_PRINT_URL        "https://api.transkoi.luckjingle.com/luckypod/aiPrinter/testImagePrint"

/* Audio: 8kHz, 16-bit, mono, 6 seconds = 96000 bytes PCM */
#define AI_SAMPLE_RATE      8000
#define AI_RECORD_SECS      6
#define AI_PCM_BYTES        (AI_SAMPLE_RATE * AI_RECORD_SECS * 2)
#define AI_WAV_HDR_SZ       44

#define AI_BOUNDARY         "BK7252Boundary"
#define AI_WIFI_TIMEOUT_MS  30000
#define AI_HTTP_HDR_SZ      2048
#define AI_RESP_BUF_SZ      16384   /* large enough for printDataBase64 */
#define AI_TEXT_MAX         256

#define AI_TASK_STACK_SZ    8192
#define AI_TASK_PRIO        15

/* ========================= WAV Header ========================= */

static void ai_wav_header(uint8_t *h, uint32_t pcm_sz)
{
    uint32_t sr = AI_SAMPLE_RATE, br = sr * 2, chunk = pcm_sz + 36, sc1 = 16;
    uint16_t ch = 1, bps = 16, ba = 2, fmt = 1;

    memcpy(h,    "RIFF", 4); memcpy(h + 4,  &chunk, 4);
    memcpy(h + 8, "WAVE", 4); memcpy(h + 12, "fmt ", 4);
    memcpy(h + 16, &sc1, 4); memcpy(h + 20, &fmt,   2);
    memcpy(h + 22, &ch,  2); memcpy(h + 24, &sr,    4);
    memcpy(h + 28, &br,  4); memcpy(h + 32, &ba,    2);
    memcpy(h + 34, &bps, 2);
    memcpy(h + 36, "data", 4); memcpy(h + 40, &pcm_sz, 4);
}

/* ========================= URL Encoder ========================= */

static void ai_url_encode(const char *src, char *dst, int dsz)
{
    static const char *hex = "0123456789ABCDEF";
    int i = 0;
    while (*src && i + 4 < dsz) {
        unsigned char c = (unsigned char)*src++;
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            dst[i++] = c;
        } else {
            dst[i++] = '%';
            dst[i++] = hex[c >> 4];
            dst[i++] = hex[c & 0xf];
        }
    }
    dst[i] = '\0';
}

/* ========================= WiFi Connect ========================= */

static int ai_wifi_connect(void)
{
    int n = sizeof(AI_WIFI_LIST) / sizeof(AI_WIFI_LIST[0]);
    for (int i = 0; i < n; i++) {
        network_InitTypeDef_st cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.wifi_mode = BK_STATION;
        strncpy(cfg.wifi_ssid, AI_WIFI_LIST[i].ssid, sizeof(cfg.wifi_ssid) - 1);
        strncpy(cfg.wifi_key,  AI_WIFI_LIST[i].pass, sizeof(cfg.wifi_key) - 1);
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

static BUTTON_S  g_ai_button;
static rt_sem_t  g_btn_sem;

static uint8_t btn_read_level(BUTTON_S *handle)
{
    return (uint8_t)bk_gpio_input((uint32_t)handle->user_data);
}

static void btn_on_single_click(void *param)
{
    rt_kprintf("[AIPrinter] Button pressed! Starting recording...\n");
    rt_sem_release(g_btn_sem);
}

static void btn_tick_thread(void *arg)
{
    while (1) {
        button_ticks(RT_NULL);
        rt_thread_delay(5); /* matches TICKS_INTERVAL = 5ms */
    }
}

static void ai_button_init(void)
{
    bk_gpio_config_input_pup(AI_BTN_GPIO); /* input + pull-up, button connects to GND */
    button_init(&g_ai_button, btn_read_level, 0, (void *)AI_BTN_GPIO);
    button_attach(&g_ai_button, SINGLE_CLICK, btn_on_single_click);
    button_start(&g_ai_button);

    rt_thread_t t = rt_thread_create("btn_tick", btn_tick_thread, RT_NULL,
                                     512, AI_TASK_PRIO - 1, 5);
    if (t) rt_thread_startup(t);
}

/* ========================= Audio Record ========================= */

static int ai_record_pcm(uint8_t *buf, int buf_sz)
{
    rt_device_t mic = rt_device_find("mic");
    if (!mic) {
        rt_kprintf("[AIPrinter] mic device not found!\n");
        return -1;
    }
    if (rt_device_open(mic, RT_DEVICE_OFLAG_RDONLY) != RT_EOK) {
        rt_kprintf("[AIPrinter] Failed to open mic!\n");
        return -1;
    }

    rt_kprintf("[AIPrinter] Speak now! Recording %d seconds...\n", AI_RECORD_SECS);

    int total = 0;
    while (total < buf_sz) {
        /* Read in small chunks to keep up with DMA ring buffer (5120 bytes) */
        int to_read = buf_sz - total;
        if (to_read > 512) to_read = 512;
        int n = rt_device_read(mic, 0, buf + total, to_read);
        if (n > 0) {
            total += n;
        } else {
            rt_thread_delay(2);
        }
    }

    rt_device_close(mic);
    rt_kprintf("[AIPrinter] Recording done: %d bytes PCM.\n", total);
    return 0;
}

/* ========================= ASR API Call ========================= */

static int ai_asr_call(char *body_buf, int body_sz, char *text_out)
{
    struct webclient_session *sess = RT_NULL;
    uint8_t *resp = RT_NULL;
    int ret = -1;

    sess = webclient_session_create(AI_HTTP_HDR_SZ);
    if (!sess) {
        rt_kprintf("[AIPrinter] webclient_session_create failed\n");
        goto out;
    }

    {
        char ct[80];
        rt_snprintf(ct, sizeof(ct), "multipart/form-data; boundary=%s", AI_BOUNDARY);
        webclient_header_fields_add(sess, "Content-Type: %s\r\n", ct);
        webclient_header_fields_add(sess, "Content-Length: %d\r\n", body_sz);
        webclient_header_fields_add(sess, "Authorization: Bearer test-token-2026-luckypod\r\n");
    }

    rt_kprintf("[AIPrinter] POST to ASR API, body=%d bytes...\n", body_sz);
    int status = webclient_post(sess, AI_ASR_URL, body_buf, body_sz);
    rt_kprintf("[AIPrinter] ASR HTTP status: %d\n", status);

    if (status != 200) {
        rt_kprintf("[AIPrinter] ASR API returned non-200 status\n");
        goto out;
    }

    resp = rt_malloc(AI_RESP_BUF_SZ);
    if (!resp) {
        rt_kprintf("[AIPrinter] OOM for response buffer\n");
        goto out;
    }

    {
        int n, total = 0;
        do {
            n = webclient_read(sess, resp + total, AI_RESP_BUF_SZ - total - 1);
            if (n > 0) total += n;
        } while (n > 0 && total < AI_RESP_BUF_SZ - 1);
        resp[total] = '\0';
        rt_kprintf("[AIPrinter] ASR response: %s\n", (char *)resp);
    }

    {
        cJSON *json = cJSON_Parse((char *)resp);
        if (!json) {
            rt_kprintf("[AIPrinter] JSON parse failed\n");
            goto out;
        }

        /* Response: {"code":200,"data":{"recognizedText":"..."}} or {"data":{"text":"..."}} */
        cJSON *data = cJSON_GetObjectItem(json, "data");
        cJSON *recognized = data ? cJSON_GetObjectItem(data, "recognizedText") : RT_NULL;
        if (!recognized || recognized->type != cJSON_String)
            recognized = data ? cJSON_GetObjectItem(data, "text") : RT_NULL;

        if (recognized && (recognized->type == cJSON_String) && recognized->valuestring) {
            strncpy(text_out, recognized->valuestring, AI_TEXT_MAX - 1);
            text_out[AI_TEXT_MAX - 1] = '\0';
            rt_kprintf("[AIPrinter] Recognized text: \"%s\"\n", text_out);
            ret = 0;
        } else {
            rt_kprintf("[AIPrinter] ASR: no text found in response\n");
        }
        cJSON_Delete(json);
    }

out:
    if (sess) webclient_close(sess);
    if (resp) rt_free(resp);
    return ret;
}

/* ========================= Print API Call ========================= */

static int ai_print_call(const char *text)
{
    struct webclient_session *sess = RT_NULL;
    uint8_t *resp = RT_NULL;
    char *body = RT_NULL;
    int ret = -1;

    /* Build form body: prompt=<url-encoded text> */
    int enc_len = strlen(text) * 3 + 4;
    int body_len = enc_len + 16;
    char *enc = rt_malloc(enc_len);
    body = rt_malloc(body_len);
    if (!enc || !body) {
        rt_kprintf("[AIPrinter] OOM for print body\n");
        if (enc) rt_free(enc);
        goto out;
    }
    ai_url_encode(text, enc, enc_len);
    rt_snprintf(body, body_len, "prompt=%s", enc);
    rt_free(enc);
    rt_kprintf("[AIPrinter] POST print API, prompt=\"%s\"\n", text);

    sess = webclient_session_create(AI_HTTP_HDR_SZ);
    if (!sess) {
        rt_kprintf("[AIPrinter] webclient_session_create failed\n");
        goto out;
    }

    webclient_header_fields_add(sess, "Content-Type: application/x-www-form-urlencoded\r\n");
    webclient_header_fields_add(sess, "Content-Length: %d\r\n", (int)strlen(body));
    webclient_header_fields_add(sess, "Authorization: Bearer test-token-2026-luckypod\r\n");

    int status = webclient_post(sess, AI_PRINT_URL, body, strlen(body));
    rt_kprintf("[AIPrinter] Print API HTTP status: %d\n", status);
    if (status != 200) {
        rt_kprintf("[AIPrinter] Print API returned non-200 status\n");
        goto out;
    }

    resp = rt_malloc(AI_RESP_BUF_SZ);
    if (!resp) {
        rt_kprintf("[AIPrinter] OOM for print response buffer\n");
        goto out;
    }

    {
        int n, total = 0;
        do {
            n = webclient_read(sess, resp + total, AI_RESP_BUF_SZ - total - 1);
            if (n > 0) total += n;
        } while (n > 0 && total < AI_RESP_BUF_SZ - 1);
        resp[total] = '\0';
        rt_kprintf("[AIPrinter] Print API response (%d bytes)\n", total);
    }

    {
        /* Response: {"code":200,"data":{"imageUrl":"...","printDataBase64":"...",...}} */
        cJSON *json = cJSON_Parse((char *)resp);
        if (!json) {
            rt_kprintf("[AIPrinter] Print API JSON parse failed\n");
            goto out;
        }

        cJSON *data = cJSON_GetObjectItem(json, "data");
        if (data) {
            cJSON *image_url = cJSON_GetObjectItem(data, "imageUrl");
            cJSON *print_b64 = cJSON_GetObjectItem(data, "printDataBase64");

            if (image_url && (image_url->type == cJSON_String) && image_url->valuestring) {
                rt_kprintf("[AIPrinter] Image URL: %s\n", image_url->valuestring);
            } else {
                rt_kprintf("[AIPrinter] imageUrl: (null)\n");
            }

            if (print_b64 && (print_b64->type == cJSON_String) && print_b64->valuestring) {
                rt_kprintf("[AIPrinter] printDataBase64(first 50): %.50s\n", print_b64->valuestring);
                /* Send ESC/POS data to BLE printer */
                ble_printer_send_base64(print_b64->valuestring);
            } else {
                rt_kprintf("[AIPrinter] printDataBase64: (null)\n");
            }
            ret = 0;
        } else {
            rt_kprintf("[AIPrinter] Print API: 'data' field not found\n");
        }
        cJSON_Delete(json);
    }

out:
    if (sess) webclient_close(sess);
    if (body) rt_free(body);
    if (resp) rt_free(resp);
    return ret;
}

/* ========================= Main Task ========================= */

static void ai_printer_task(void *arg)
{
    rt_kprintf("\n[AIPrinter] ======================================\n");
    rt_kprintf("[AIPrinter]   AI Voice Printer  (BK7252N)\n");
    rt_kprintf("[AIPrinter]   GPIO%d button triggers recording\n", AI_BTN_GPIO);
    rt_kprintf("[AIPrinter] ======================================\n\n");

    /* Step 1: Connect WiFi once at startup */
    rt_kprintf("[AIPrinter] [1/4] Connecting to WiFi...\n");
    if (ai_wifi_connect() != 0) {
        rt_kprintf("[AIPrinter] WiFi failed! Check config and reboot.\n");
        return;
    }

    /* Step 2: Connect BLE printer (once at startup) */
    rt_kprintf("[AIPrinter] [2/3] Connecting to BLE printer...\n");
    if (ble_printer_connect() != 0) {
        rt_kprintf("[AIPrinter] BLE printer connect failed! Check printer is on.\n");
        /* Continue anyway - print data will fail gracefully */
    }

    /* Main loop: wait button → record → ASR → print → repeat */
    while (1) {
        char  asr_text[AI_TEXT_MAX] = {0};
        char  prefix[200], suffix[64];
        int   pre_len, suf_len, pcm_off, body_sz;
        char *body = RT_NULL;

        rt_kprintf("[AIPrinter] Ready. Press button (GPIO%d) to start.\n", AI_BTN_GPIO);
        rt_sem_take(g_btn_sem, RT_WAITING_FOREVER);

        /* Step 2: Record */
        rt_kprintf("[AIPrinter] [2/4] Speak now! Recording %ds...\n", AI_RECORD_SECS);

        pre_len = rt_snprintf(prefix, sizeof(prefix),
            "--%s\r\n"
            "Content-Disposition: form-data; name=\"audio\"; filename=\"audio.wav\"\r\n"
            "Content-Type: audio/wav\r\n"
            "\r\n",
            AI_BOUNDARY);
        suf_len = rt_snprintf(suffix, sizeof(suffix), "\r\n--%s--\r\n", AI_BOUNDARY);
        pcm_off = pre_len + AI_WAV_HDR_SZ;
        body_sz = pcm_off + AI_PCM_BYTES + suf_len;

        body = rt_malloc(body_sz);
        if (!body) {
            rt_kprintf("[AIPrinter] OOM: cannot allocate %d bytes!\n", body_sz);
            continue;
        }

        memcpy(body, prefix, pre_len);
        ai_wav_header((uint8_t *)(body + pre_len), AI_PCM_BYTES);

        if (ai_record_pcm((uint8_t *)(body + pcm_off), AI_PCM_BYTES) != 0) {
            rt_kprintf("[AIPrinter] Recording failed!\n");
            rt_free(body);
            continue;
        }
        memcpy(body + pcm_off + AI_PCM_BYTES, suffix, suf_len);

        /* Step 3: ASR */
        rt_kprintf("[AIPrinter] [3/4] Sending audio to ASR API...\n");
        if (ai_asr_call(body, body_sz, asr_text) != 0 || !asr_text[0]) {
            rt_kprintf("[AIPrinter] ASR failed or empty result!\n");
            rt_free(body);
            continue;
        }
        rt_free(body);

        /* Step 4: Print */
        rt_kprintf("[AIPrinter] [4/4] Text: \"%s\" → Print API\n", asr_text);
        ai_print_call(asr_text);

        rt_kprintf("[AIPrinter] Done! Press button for next print.\n\n");
    }
}

/* ========================= Entry Point ========================= */

void ai_printer_start(void)
{
    g_btn_sem = rt_sem_create("btn_sem", 0, RT_IPC_FLAG_FIFO);
    ai_button_init();
    ble_printer_init();

    rt_thread_t t = rt_thread_create("ai_printer",
                                     ai_printer_task, RT_NULL,
                                     AI_TASK_STACK_SZ, AI_TASK_PRIO, 20);
    if (t) {
        rt_thread_startup(t);
    } else {
        rt_kprintf("[AIPrinter] ERROR: failed to create thread!\n");
    }
}
