#ifndef __uconfy_networking_h_included__
#define __uconfy_networking_h_included__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#define UCONFY_PRIMARY_WIFI "primary_wifi"
#define UCONFY_PRIMARY_PWD  "primary_pwd"

#define UCONFY_FALLBACK_WIFI "fback_wifi"
#define UCONFY_FALLBACK_PWD  "fback_pwd"

extern EventGroupHandle_t wifi_event_group;
extern int UCONFIG_CONNECTED_BIT;
extern int uconfy_is_fallback_wifi;
extern int ufonfy_allow_wifi_fallback;

extern int UCONFIG_IS_WIFI_CONNECTED;
extern void uconfy_initialize_wifi(
    char *initial_wifi_ssid,
    char *initial_wifi_password,
    int allow_wifi_fallback,
    void (*wifi_connected_callback)());
extern void uconfy_configure_fallback_wifi();
extern int uconfy_configure_primary_wifi();

#endif