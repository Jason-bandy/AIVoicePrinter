#ifndef __BLE_PRINTER_H__
#define __BLE_PRINTER_H__

#include <stdint.h>

/* Initialize BLE printer module (call once at startup) */
int ble_printer_init(void);

/* Scan + connect + discover service (blocks until ready or timeout) */
int ble_printer_connect(void);

/* Send raw ESC/POS bytes to printer in BLE chunks */
int ble_printer_send(const uint8_t *data, int len);

/* Decode base64 then send - use this with server printDataBase64 */
int ble_printer_send_base64(const char *b64_str);

/* Disconnect from printer */
void ble_printer_disconnect(void);

/* RT-Thread task entry: connect with up to 3 retries (use for rt_thread_create) */
void ble_printer_connect_task(void *arg);

#endif /* __BLE_PRINTER_H__ */
