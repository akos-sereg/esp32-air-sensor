#ifndef __fetch_configs_h_included__
#define __fetch_configs_h_included__


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "uconfy_networking.h"
#include "nvs.h"

extern char response_payload[1024];
extern void uconfy_fetch_configs(void (*configs_fetched_callback)());

#endif