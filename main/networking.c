#include "include/networking.h"

int CONNECTED_BIT = BIT0;
int IS_WIFI_CONNECTED = 0;
EventGroupHandle_t wifi_event_group;
const char *TAG_N = "NETW";

esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch(event->event_id) {
	case SYSTEM_EVENT_STA_START:
	    ESP_LOGI(TAG_N, "Starting up, connecting to WiFi");
	    esp_wifi_connect();
	    IS_WIFI_CONNECTED = 0;
	    break;
	case SYSTEM_EVENT_STA_GOT_IP:
	    ESP_LOGI(TAG_N, "Got IP Address, setting connected bit to TRUE");
	    IS_WIFI_CONNECTED = 1;
	    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
	    break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
	    ESP_LOGI(TAG_N, "WiFi disconnected, trying to reconnect after 5 seconds, and clearing connected bit");
	    IS_WIFI_CONNECTED = 0;
	    vTaskDelay(5000 / portTICK_PERIOD_MS);
	    esp_wifi_connect();
	    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
	    break;
	default:
	    break;
    }

    return ESP_OK;
}

void networking_initialize_wifi() {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG_N, "Setting WiFi configuration SSID %s ...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}