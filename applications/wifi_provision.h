/*
 * WiFi Provisioning — BK7252N / RT-Thread
 *
 * First-boot / no-credential flow:
 *   1. wifi_prov_start()  → device starts SoftAP "LuckyPod-XXXX" (pwd: luckypod)
 *   2. Phone scans QR code → auto-connects to AP
 *   3. Captive portal pops up automatically (DNS spoof + HTTP redirect)
 *   4. User picks home WiFi + enters password → submits form
 *   5. Credentials saved to EasyFlash
 *   6. wifi_prov_wait_done() returns 0
 *   7. Caller calls wifi_prov_stop(), loads credentials, connects to home WiFi
 *
 * Persistent storage keys (EasyFlash):
 *   "wifi_ssid" / "wifi_pwd"
 *
 * MSH commands:
 *   clearprov     — erase saved WiFi, force re-provision on next boot
 */

#ifndef WIFI_PROVISION_H
#define WIFI_PROVISION_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Query saved credentials ---- */

/** Returns 1 if a non-empty SSID is stored in flash. */
int wifi_prov_has_credentials(void);

/** Copy saved SSID/password into caller buffers. Returns 0 on success, -1 if empty. */
int wifi_prov_load(char *ssid, int ssid_max, char *pwd, int pwd_max);

/** Erase saved credentials (forces re-provision on next boot). */
void wifi_prov_clear(void);

/* ---- Provisioning lifecycle ---- */

/**
 * Start provisioning mode:
 *   - scans nearby APs (to populate HTML datalist)
 *   - starts SoftAP "LuckyPod-XXXX" with DHCP server
 *   - starts DNS spoof thread (port 53)
 *   - starts HTTP server thread (port 80, captive-portal aware)
 */
void wifi_prov_start(void);

/**
 * Stop provisioning mode (tear down AP, stop threads).
 * Call after wifi_prov_wait_done() returns.
 */
void wifi_prov_stop(void);

/**
 * Block until user submits credentials or timeout.
 * @param timeout_ms  milliseconds to wait (e.g. 300000 = 5 minutes)
 * @return  0 on success, -1 on timeout
 */
int wifi_prov_wait_done(int timeout_ms);

/* ---- Display helpers ---- */

/**
 * Fill buf with the QR code content for the SoftAP.
 * Format: "WIFI:S:<ssid>;T:WPA;P:<pass>;;"
 * Display this as a QR code so the phone camera can auto-connect.
 */
void wifi_prov_qr_content(char *buf, int buf_len);

/** Return the SoftAP SSID string (e.g. "LuckyPod-A3F2"). */
const char *wifi_prov_ap_ssid(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_PROVISION_H */
