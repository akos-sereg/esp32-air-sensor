#ifndef __uconfy_h_included__
#define __uconfy_h_included__

#include "flush_logs.h"
#include "fetch_configs.h"
#include "uconfy_networking.h"
#include "nvs.h"
#include "config_repo.h"

#define TAG_UCONFIG "uconfig"
// #define WEB_SERVER "192.168.1.10"
// #define WEB_PORT "8080"
#define WEB_SERVER "uconfy.herokuapp.com"
#define WEB_PORT "80"

#define REQUEST "Host: "WEB_SERVER":"WEB_PORT"\r\nUser-Agent: esp-idf/1.0 esp32\r\n"

extern char *uconfig_device_id;
extern char *uconfig_api_key;

extern void uconfy_init(char *device_id, char *api_key);

// to be internal
// extern void uconfig_poller(void * pvParameters);

#endif