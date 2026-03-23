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
#include <string.h>
#include "ble_api_5_x.h"
#include "app_sdp.h"
#include "base_64.h"
#include "ble_printer.h"

/* ========================= Config ========================= */

#define PRINTER_NAME        "TPC50S_A07B_BLE"
#define BLE_CHUNK_SIZE      20          /* safe default, fits any MTU */
#define BLE_SCAN_TIMEOUT_MS 30000
#define BLE_DISC_TIMEOUT_MS 10000
#define BLE_WRITE_TIMEOUT_MS 3000

/* 0xFF00 service UUID, little-endian 16-bit */
static const app_sdp_service_uuid s_printer_svc_tab[] = {
    { .uuid_len = 2, .uuid = {0x00, 0xFF} },
};

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

static rt_sem_t         s_found_sem;   /* printer advertisement found */
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

/* ========================= SDP Callbacks ========================= */

/* Called for each characteristic found in registered services */
static void printer_sdp_char_cb(uint8_t conidx, uint16_t val_hdl,
                                 uint8_t uuid_len, uint8_t *uuid)
{
    if (uuid_len == 2 && uuid[0] == WRITE_UUID_B0 && uuid[1] == WRITE_UUID_B1) {
        s_write_hdl = val_hdl;
        rt_kprintf("[BLEPrinter] Write char found, handle=0x%04x\n", val_hdl);
    }
}

/* Called for SDP-level events */
static void printer_sdp_notice_cb(sdp_notice_t notice, void *param)
{
    switch (notice) {
    case SDP_DISCOVER_SVR_DONE:
        rt_kprintf("[BLEPrinter] Discovery done, write_hdl=0x%04x\n", s_write_hdl);
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
        break;

    default:
        break;
    }
}

static void printer_ble_cmd_cb(ble_cmd_t cmd, ble_cmd_param_t *param)
{
    (void)cmd; (void)param;
}

/* ========================= Public API ========================= */

int ble_printer_init(void)
{
    s_found_sem = rt_sem_create("prn_fnd", 0, RT_IPC_FLAG_FIFO);
    s_conn_sem  = rt_sem_create("prn_con", 0, RT_IPC_FLAG_FIFO);
    s_disc_sem  = rt_sem_create("prn_dsc", 0, RT_IPC_FLAG_FIFO);
    s_write_sem = rt_sem_create("prn_wrt", 0, RT_IPC_FLAG_FIFO);

    if (!s_found_sem || !s_conn_sem || !s_disc_sem || !s_write_sem) {
        rt_kprintf("[BLEPrinter] OOM creating semaphores!\n");
        return -1;
    }

    ble_set_notice_cb(printer_ble_notice_cb);
    register_app_sdp_characteristic_callback(printer_sdp_char_cb);
    register_app_sdp_service_tab(
        sizeof(s_printer_svc_tab) / sizeof(s_printer_svc_tab[0]),
        (app_sdp_service_uuid *)s_printer_svc_tab);
    sdp_set_notice_cb(printer_sdp_notice_cb);

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
    bk_ble_scan_stop(s_scan_idx, printer_ble_cmd_cb);

    /* Step 2: Connect */
    rt_kprintf("[BLEPrinter] Connecting...\n");
    uint8_t init_idx = app_ble_get_idle_actv_idx_handle(INIT_ACTV);
    struct bd_addr addr;
    memcpy(addr.addr, s_peer_addr, 6);

    bk_ble_create_init(init_idx, printer_ble_cmd_cb);
    bk_ble_init_set_connect_dev_addr(init_idx, &addr, s_peer_addr_type);
    bk_ble_init_start_conn(init_idx, 10000, printer_ble_cmd_cb);

    if (rt_sem_take(s_conn_sem,
                    rt_tick_from_millisecond(BLE_SCAN_TIMEOUT_MS)) != RT_EOK
        || s_conn_idx == 0xFF) {
        rt_kprintf("[BLEPrinter] Connect timeout!\n");
        s_state = BLE_PRN_IDLE;
        return -1;
    }

    /* Step 3: Service discovery (auto-triggered by registered service tab) */
    rt_kprintf("[BLEPrinter] Discovering services...\n");
    sdp_update_gatt_mtu(s_conn_idx);

    if (rt_sem_take(s_disc_sem,
                    rt_tick_from_millisecond(BLE_DISC_TIMEOUT_MS)) != RT_EOK) {
        rt_kprintf("[BLEPrinter] Discovery timeout!\n");
        return -1;
    }
    if (s_write_hdl == 0xFFFF) {
        rt_kprintf("[BLEPrinter] 0xFF02 characteristic not found!\n");
        return -1;
    }

    rt_kprintf("[BLEPrinter] Ready to print!\n");
    return 0;
}

int ble_printer_send(const uint8_t *data, int len)
{
    if (s_state != BLE_PRN_READY || s_write_hdl == 0xFFFF) {
        rt_kprintf("[BLEPrinter] Not ready!\n");
        return -1;
    }

    rt_kprintf("[BLEPrinter] Sending %d bytes in %d-byte chunks...\n",
               len, BLE_CHUNK_SIZE);

    int offset = 0;
    while (offset < len) {
        int chunk = len - offset;
        if (chunk > BLE_CHUNK_SIZE) chunk = BLE_CHUNK_SIZE;

        sdp_svc_write_characteristic(s_conn_idx, s_write_hdl,
                                     chunk, (uint8_t *)(data + offset));

        /* Wait for write done before sending next chunk */
        if (rt_sem_take(s_write_sem,
                        rt_tick_from_millisecond(BLE_WRITE_TIMEOUT_MS)) != RT_EOK) {
            rt_kprintf("[BLEPrinter] Write timeout at offset %d!\n", offset);
            return -1;
        }
        offset += chunk;
    }

    rt_kprintf("[BLEPrinter] Print data sent (%d bytes).\n", len);
    return 0;
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
