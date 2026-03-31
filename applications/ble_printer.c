/*
 * BLE Printer Module - BK7252N
 *
 * Connects to TPC50S_A07B_BLE thermal printer via BLE GATT and sends ESC/POS data.
 *
 * Flow:
 *   ble_printer_init()    - register callbacks once
 *   ble_printer_connect() - scan → find device → connect → discover 0xFF02 handle
 *   ble_printer_send_base64(b64) - decode base64 → write ESC/POS in 20-byte chunks
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include "ble_api_5_x.h"
#include "ble_ui.h"
#include "app_ble.h"
#include "app_sdp.h"
#include "base_64.h"
#include "ble_printer.h"

/* ========================= Config ========================= */

#define PRINTER_NAME         "TPC50S_A07B_BLE"
#define BLE_CHUNK_SIZE       20          /* safe default, fits any MTU */
#define BLE_SCAN_TIMEOUT_MS  30000
#define BLE_DISC_TIMEOUT_MS  30000
#define BLE_WRITE_TIMEOUT_MS 3000
#define BLE_CHUNK_DELAY_MS   30   /* inter-chunk delay for WRITE_NO_RESP (ms) */
#define BLE_DEFAULT_MTU      185  /* used when printer never sends MTU notification */

/* UART2 fallback: P0=TX, P1=RX.
 * Change UART_PRN_BAUD_RATE to match your printer's wired serial setting. */
#define UART_PRN_DEV_NAME    "uart2"
#define UART_PRN_BAUD_RATE   921600

/* 0xFF02 write characteristic UUID (little-endian) */
#define WRITE_UUID_B0   0x02
#define WRITE_UUID_B1   0xFF

/* ========================= State ========================= */

typedef enum {
    BLE_PRN_IDLE = 0,
    BLE_PRN_SCANNING,
    BLE_PRN_CONNECTING,
    BLE_PRN_DISCOVERING,
    BLE_PRN_READY,
} ble_prn_state_t;

static ble_prn_state_t  s_state      = BLE_PRN_IDLE;
static uint8_t          s_scan_idx   = 0xFF;
static uint8_t          s_conn_idx   = 0xFF;
static uint16_t         s_write_hdl  = 0xFFFF;
static uint8_t          s_peer_addr[6];
static uint8_t          s_peer_addr_type;
static int              s_mtu_peer   = 0;   /* set by printer notification; 0 = use default */

static rt_device_t      s_uart_dev = RT_NULL;   /* UART2 fallback (P0/P1) */

static rt_sem_t         s_found_sem;   /* printer advertisement found */
static rt_sem_t         s_cmd_sem;     /* BLE cmd step done (scan_stop / init_create) */
static rt_sem_t         s_conn_sem;    /* BLE connected */
static rt_sem_t         s_disc_sem;    /* service discovery done */
static rt_sem_t         s_write_sem;   /* one write chunk done */

/* ========================= AD Parser ========================= */

static int ad_find_name(const uint8_t *data, uint8_t len,
                        char *name_out, int name_max)
{
    int i = 0;
    while (i < (int)len) {
        uint8_t field_len = data[i];
        if (field_len == 0 || i + field_len >= len) break;
        uint8_t type = data[i + 1];
        if (type == 0x08 || type == 0x09) { /* shortened / complete local name */
            int nlen = field_len - 1;
            if (nlen >= name_max) nlen = name_max - 1;
            memcpy(name_out, &data[i + 2], nlen);
            name_out[nlen] = '\0';
            return 1;
        }
        i += field_len + 1;
    }
    return 0;
}

/* ========================= UART Fallback (P0=TX / P1=RX) ========================= */

static int uart_printer_send(const uint8_t *data, int len)
{
    if (!s_uart_dev) {
        rt_kprintf("[BLEPrinter] UART fallback not available.\n");
        return -1;
    }
    rt_size_t written = rt_device_write(s_uart_dev, 0, data, (rt_size_t)len);
    rt_kprintf("[BLEPrinter] UART sent %d/%d bytes.\n", (int)written, len);
    return (written == (rt_size_t)len) ? 0 : -1;
}

/* ========================= SDP Callbacks ========================= */

/* Scan svr_list for the 0xFF02 write characteristic; sets s_write_hdl if found.
 * register_app_sdp_characteristic_callback() is NOT used: the SDK stores that
 * callback but never calls it in the BLE 5.2 stack. */
static void printer_scan_svc_db(uint8_t con_idx)
{
    struct sdp_env_tag *env = sdp_get_env_use_conidx(con_idx);
    if (!env) return;
    struct sdp_db *p_db = (struct sdp_db *)env->svr_list.first;
    while (p_db) {
        struct db *svc = &p_db->svr;
        for (uint8_t i = 0; i < svc->chars_nb; i++) {
            struct bk_prf_char_def *ch = &svc->chars[i];
            uint8_t *u = (uint8_t *)ch->uuid;
            /* uuid_type 0x00 == GATT_UUID_16; bytes stored little-endian */
            if (ch->uuid_type == 0x00 &&
                u[0] == WRITE_UUID_B0 && u[1] == WRITE_UUID_B1) {
                s_write_hdl = ch->val_hdl;
                rt_kprintf("[BLEPrinter] Found 0xFF02, handle=0x%04x\n", s_write_hdl);
                return;
            }
        }
        p_db = (struct sdp_db *)p_db->hdr.next;
    }
}

/* Called for SDP-level events */
static void printer_sdp_notice_cb(sdp_notice_t notice, void *param)
{
    sdp_event_t *e = (sdp_event_t *)param;
    uint8_t cidx;

    switch (notice) {
    case SDP_DISCOVER_SVR:
        /* Fired for each service pushed into the DB - scan immediately. */
        if (s_state != BLE_PRN_DISCOVERING || s_write_hdl != 0xFFFF) break;
        cidx = e ? (uint8_t)e->con_idx : s_conn_idx;
        printer_scan_svc_db(cidx);
        if (s_write_hdl != 0xFFFF) {
            s_state = BLE_PRN_READY;
            rt_sem_release(s_disc_sem);
        }
        break;

    case SDP_DISCOVER_SVR_DONE:
        /* All GATT discovery complete - release sem if not already done above. */
        rt_kprintf("[BLEPrinter] Discovery complete, write_hdl=0x%04x\n", s_write_hdl);
        if (s_state == BLE_PRN_READY) break;   /* already released via SDP_DISCOVER_SVR */
        cidx = e ? (uint8_t)e->con_idx : s_conn_idx;
        if (s_write_hdl == 0xFFFF)
            printer_scan_svc_db(cidx);         /* final fallback full-scan */
        s_state = (s_write_hdl != 0xFFFF) ? BLE_PRN_READY : BLE_PRN_IDLE;
        rt_sem_release(s_disc_sem);
        break;

    case SDP_CHARAC_WRITE_DONE:
        rt_sem_release(s_write_sem);
        break;

    default:
        break;
    }
}

/* ========================= BLE Notice Callback ========================= */

static void printer_ble_notice_cb(ble_notice_t notice, void *param)
{
    switch (notice) {

    case BLE_5_REPORT_ADV: {
        if (s_state != BLE_PRN_SCANNING) break;
        recv_adv_t *adv = (recv_adv_t *)param;
        char name[32] = {0};
        if (!ad_find_name(adv->data, adv->data_len, name, sizeof(name))) break;
        if (strncmp(name, PRINTER_NAME, strlen(PRINTER_NAME)) != 0) break;

        rt_kprintf("[BLEPrinter] Found: %s  MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",
                   name,
                   adv->adv_addr[0], adv->adv_addr[1], adv->adv_addr[2],
                   adv->adv_addr[3], adv->adv_addr[4], adv->adv_addr[5]);
        memcpy(s_peer_addr, adv->adv_addr, 6);
        s_peer_addr_type = adv->adv_addr_type;
        s_state = BLE_PRN_CONNECTING;
        rt_sem_release(s_found_sem);
        break;
    }

    case BLE_5_INIT_CONNECT_EVENT: {
        conn_ind_t *c = (conn_ind_t *)param;
        s_conn_idx = c->conn_idx;
        s_state = BLE_PRN_DISCOVERING;
        rt_kprintf("[BLEPrinter] Connected, conn_idx=%d\n", s_conn_idx);
        rt_sem_release(s_conn_sem);
        break;
    }

    case BLE_5_INIT_CONNECT_FAILED_EVENT:
        rt_kprintf("[BLEPrinter] Connection failed!\n");
        s_state = BLE_PRN_IDLE;
        rt_sem_release(s_conn_sem);
        break;

    case BLE_5_INIT_DISCONNECT_EVENT:
        rt_kprintf("[BLEPrinter] Disconnected.\n");
        s_state    = BLE_PRN_IDLE;
        s_conn_idx = 0xFF;
        s_write_hdl = 0xFFFF;
        s_mtu_peer  = 0;
        break;

    default:
        break;
    }
}

static void printer_ble_cmd_cb(ble_cmd_t cmd, ble_cmd_param_t *param)
{
    (void)param;
    /* Release step semaphore for operations we explicitly wait on */
    if (cmd == BLE_DEINIT_SCAN || cmd == BLE_INIT_CREATE) {
        rt_sem_release(s_cmd_sem);
    }
}

/* ========================= Public API ========================= */

int ble_printer_init(void)
{
    s_found_sem = rt_sem_create("prn_fnd", 0, RT_IPC_FLAG_FIFO);
    s_cmd_sem   = rt_sem_create("prn_cmd", 0, RT_IPC_FLAG_FIFO);
    s_conn_sem  = rt_sem_create("prn_con", 0, RT_IPC_FLAG_FIFO);
    s_disc_sem  = rt_sem_create("prn_dsc", 0, RT_IPC_FLAG_FIFO);
    s_write_sem = rt_sem_create("prn_wrt", 0, RT_IPC_FLAG_FIFO);

    if (!s_found_sem || !s_cmd_sem || !s_conn_sem || !s_disc_sem || !s_write_sem) {
        rt_kprintf("[BLEPrinter] OOM creating semaphores!\n");
        return -1;
    }

    ble_set_notice_cb(printer_ble_notice_cb);
    sdp_set_notice_cb(printer_sdp_notice_cb);

    /* Open UART2 (P0=TX, P1=RX) for wired fallback */
    s_uart_dev = rt_device_find(UART_PRN_DEV_NAME);
    if (s_uart_dev) {
        struct serial_configure cfg = RT_SERIAL_CONFIG_DEFAULT;
        cfg.baud_rate = UART_PRN_BAUD_RATE;
        rt_device_open(s_uart_dev, RT_DEVICE_FLAG_RDWR);
        rt_device_control(s_uart_dev, RT_DEVICE_CTRL_CONFIG, &cfg);
        rt_kprintf("[BLEPrinter] UART2 fallback ready (P0/P1, %d baud).\n",
                   UART_PRN_BAUD_RATE);
    } else {
        rt_kprintf("[BLEPrinter] UART2 not found, no wired fallback.\n");
    }

    rt_kprintf("[BLEPrinter] Initialized.\n");
    return 0;
}

int ble_printer_connect(void)
{
    if (s_state == BLE_PRN_READY) {
        rt_kprintf("[BLEPrinter] Already connected.\n");
        return 0;
    }

    /* Step 1: Scan */
    struct scan_param scan = {
        .channel_map = 7,
        .interval    = 100,
        .window      = 50,
    };
    s_scan_idx = app_ble_get_idle_actv_idx_handle(SCAN_ACTV);
    s_state    = BLE_PRN_SCANNING;
    rt_kprintf("[BLEPrinter] Scanning for %s...\n", PRINTER_NAME);
    bk_ble_scan_start(s_scan_idx, &scan, printer_ble_cmd_cb);

    if (rt_sem_take(s_found_sem,
                    rt_tick_from_millisecond(BLE_SCAN_TIMEOUT_MS)) != RT_EOK) {
        rt_kprintf("[BLEPrinter] Printer not found (timeout)!\n");
        bk_ble_scan_stop(s_scan_idx, NULL);
        s_state = BLE_PRN_IDLE;
        return -1;
    }
    /* Stop scan and wait for BLE_DEINIT_SCAN callback (BLE env → READY) */
    bk_ble_scan_stop(s_scan_idx, printer_ble_cmd_cb);
    if (rt_sem_take(s_cmd_sem,
                    rt_tick_from_millisecond(5000)) != RT_EOK) {
        rt_kprintf("[BLEPrinter] Scan stop timeout!\n");
        s_state = BLE_PRN_IDLE;
        return -1;
    }

    /* Step 2: Connect */
    rt_kprintf("[BLEPrinter] Connecting...\n");
    uint8_t init_idx = app_ble_get_idle_conn_idx_handle(INIT_ACTV);
    struct bd_addr addr;
    memcpy(addr.addr, s_peer_addr, 6);

    /* Create init activity and wait for BLE_INIT_CREATE callback */
    bk_ble_create_init(init_idx, printer_ble_cmd_cb);
    if (rt_sem_take(s_cmd_sem,
                    rt_tick_from_millisecond(5000)) != RT_EOK) {
        rt_kprintf("[BLEPrinter] Init create timeout!\n");
        s_state = BLE_PRN_IDLE;
        return -1;
    }

    /* Set peer address then start connection */
    bk_ble_init_set_connect_dev_addr(init_idx, &addr, s_peer_addr_type);
    bk_ble_init_start_conn(init_idx, 10000, printer_ble_cmd_cb);

    if (rt_sem_take(s_conn_sem,
                    rt_tick_from_millisecond(BLE_SCAN_TIMEOUT_MS)) != RT_EOK
        || s_conn_idx == 0xFF) {
        rt_kprintf("[BLEPrinter] Connect timeout!\n");
        s_state = BLE_PRN_IDLE;
        return -1;
    }

    /* Step 3: Service discovery */
    rt_kprintf("[BLEPrinter] Discovering services...\n");
    /* Give printer 300ms to stabilize before issuing GATT commands.
     * sdp_update_gatt_mtu is intentionally omitted: sending an ATT MTU Request
     * immediately after connection triggers reason-0x3e disconnects on some
     * printer firmware versions. */
    rt_thread_delay(rt_tick_from_millisecond(300));
    sdp_get_all_service(s_conn_idx);

    if (rt_sem_take(s_disc_sem,
                    rt_tick_from_millisecond(BLE_DISC_TIMEOUT_MS)) != RT_EOK) {
        rt_kprintf("[BLEPrinter] Discovery timeout!\n");
        s_state = BLE_PRN_IDLE;
        ble_printer_disconnect();   /* clean up so next retry starts fresh */
        return -1;
    }
    if (s_write_hdl == 0xFFFF) {
        rt_kprintf("[BLEPrinter] 0xFF02 characteristic not found!\n");
        s_state = BLE_PRN_IDLE;
        ble_printer_disconnect();
        return -1;
    }

    if (s_mtu_peer == 0) s_mtu_peer = BLE_DEFAULT_MTU;
    rt_kprintf("[BLEPrinter] Ready to print! mtu_peer=%d chunk_size=%d\n",
               s_mtu_peer, s_mtu_peer - 10);
    return 0;
}

int ble_printer_send(const uint8_t *data, int len)
{
    /* Hex dump first 48 bytes for debugging (16 bytes per line) */
    {
        int dump = len < 48 ? len : 48;
        rt_kprintf("[BLEPrinter] --- hex dump (total %d bytes) ---\n", len);
        for (int i = 0; i < dump; i++) {
            if (i % 16 == 0) rt_kprintf("[%02d]", i);
            rt_kprintf(" %02X", data[i]);
            if (i % 16 == 15 || i == dump - 1) rt_kprintf("\n");
        }
        rt_kprintf("[BLEPrinter] ---\n");
    }

    /* If ble_conn thread is still mid-discovery, wait up to 30s for it to finish */
    if (s_state == BLE_PRN_DISCOVERING) {
        rt_kprintf("[BLEPrinter] BLE discovery in progress, waiting...\n");
        int waited = 0;
        while (s_state == BLE_PRN_DISCOVERING && waited < 30000) {
            rt_thread_delay(rt_tick_from_millisecond(500));
            waited += 500;
        }
        rt_kprintf("[BLEPrinter] Discovery wait ended: state=%d write_hdl=0x%04x waited=%dms\n",
                   s_state, s_write_hdl, waited);
    }

    /* BLE not ready - UART fallback */
    if (s_state != BLE_PRN_READY || s_write_hdl == 0xFFFF) {
        rt_kprintf("[BLEPrinter] BLE not ready (state=%d), trying UART fallback...\n", s_state);
        return uart_printer_send(data, len);
    }

    int max_chunk = (s_mtu_peer > 10) ? (s_mtu_peer - 10) : BLE_CHUNK_SIZE;
    rt_kprintf("[BLEPrinter] Sending %d bytes via BLE (%d-byte chunks, %dms gap)...\n",
               len, max_chunk, BLE_CHUNK_DELAY_MS);

    int offset = 0;
    while (offset < len) {
        int chunk = len - offset;
        if (chunk > max_chunk) chunk = max_chunk;

        sdp_svc_write_characteristic(s_conn_idx, s_write_hdl,
                                     chunk, (uint8_t *)(data + offset));
        offset += chunk;

        /* SDP_CHARAC_WRITE_DONE never fires on this SDK stack for WRITE_NO_RESP;
         * use a fixed inter-chunk delay to avoid overwhelming the printer buffer. */
        if (offset < len)
            rt_thread_delay(rt_tick_from_millisecond(BLE_CHUNK_DELAY_MS));
    }

    rt_kprintf("[BLEPrinter] Print data sent via BLE (%d bytes).\n", len);
    return 0;
}

/* RT-Thread task entry: connect with up to 3 retries (use for rt_thread_create) */
void ble_printer_connect_task(void *arg)
{
    (void)arg;
    for (int i = 0; i < 3; i++) {
        if (ble_printer_connect() == 0) return;
        rt_kprintf("[BLEPrinter] Connect attempt %d failed; retrying in 3s...\n", i + 1);
        rt_thread_delay(rt_tick_from_millisecond(3000));
    }
    rt_kprintf("[BLEPrinter] All connection attempts failed.\n");
}

int ble_printer_send_base64(const char *b64_str)
{
    int b64_len = strlen(b64_str);
    int out_len = 0;
    uint32_t max_len = base64_calc_decode_length(
                           (const unsigned char *)b64_str, b64_len);

    uint8_t *buf = rt_malloc(max_len);
    if (!buf) {
        rt_kprintf("[BLEPrinter] OOM for decode buffer (%u bytes)!\n", max_len);
        return -1;
    }

    base64_decode((const unsigned char *)b64_str, b64_len, &out_len, buf);
    rt_kprintf("[BLEPrinter] Decoded %d bytes from base64\n", out_len);

    int ret = ble_printer_send(buf, out_len);
    rt_free(buf);
    return ret;
}

void ble_printer_disconnect(void)
{
    if (s_conn_idx != 0xFF) {
        bk_ble_init_stop_conn(s_conn_idx, NULL);
    }
    s_state     = BLE_PRN_IDLE;
    s_conn_idx  = 0xFF;
    s_write_hdl = 0xFFFF;
}
