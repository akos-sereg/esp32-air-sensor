#include "include/uconfy.h"

char *uconfig_device_id;
char *uconfig_api_key;

void uconfy_init(char *device_id, char *api_key) {
    uconfig_device_id = device_id;
    uconfig_api_key = api_key;

    uconfig_tmp_log[0] = '\0';
    // xTaskCreate(uconfig_poller, "config_poll", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

/*
void uconfig_poller( void * pvParameters )
{
  for( ;; )
  {
    if (UCONFIG_IS_WIFI_CONNECTED) {
        uconfy_fetch_configs();
    }

    vTaskDelay((uconfig_poll_interval_seconds * 1000) / portTICK_RATE_MS);
  }
}*/
