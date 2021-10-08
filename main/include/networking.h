#ifndef __networking_h_included__
#define __networking_h_included__

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
#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "config.h"

extern EventGroupHandle_t wifi_event_group;
extern int CONNECTED_BIT;
extern const char *TAG;

extern int IS_WIFI_CONNECTED;
extern void networking_initialize_wifi();

extern char response_payload[1024];

#endif