#ifndef _BT_PUB_H_
#define _BT_PUB_H_

#define BT_SUCCESS          (0)
#define BT_FAILURE          (1)
#define BT_DEV_NAME        "bt"

void ceva_main(void *arg);
void bt_activate(char *bt_name);
void bt_host_to_controller(uint8_t *msg, uint32_t type);
void bt_init(void);
void bt_exit(void);
void ble_host_to_controller(uint32_t cmd, uint8_t *buff,uint32_t len, uint32_t param);
#endif
