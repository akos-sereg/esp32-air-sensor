#include "include/uconfy.h"

int UCONFIG_CONNECTED_BIT = BIT0;
int UCONFIG_IS_WIFI_CONNECTED = 0;
EventGroupHandle_t wifi_event_group;
void (*uconfy_wifi_connected_callback)();
int uconfy_is_fallback_wifi = 0;
int ufonfy_allow_wifi_fallback = 0;

esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch(event->event_id) {
	case SYSTEM_EVENT_STA_START:
	    ESP_LOGI(TAG_UCONFIG, "Starting up, connecting to WiFi");
	    esp_wifi_connect();
	    UCONFIG_IS_WIFI_CONNECTED = 0;
	    break;
	case SYSTEM_EVENT_STA_GOT_IP:
	    ESP_LOGI(TAG_UCONFIG, "Got IP Address, setting connected bit to TRUE");
	    UCONFIG_IS_WIFI_CONNECTED = 1;
	    xEventGroupSetBits(wifi_event_group, UCONFIG_CONNECTED_BIT);
	    if (uconfy_wifi_connected_callback != NULL) {
	        uconfy_wifi_connected_callback();
	    }
	    break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
	    ESP_LOGI(TAG_UCONFIG, "WiFi disconnected, trying to reconnect after 5 seconds, and clearing connected bit");
	    UCONFIG_IS_WIFI_CONNECTED = 0;

        if (ufonfy_allow_wifi_fallback) {
            if (!uconfy_is_fallback_wifi) {
                uconfy_is_fallback_wifi = 1;
                uconfy_configure_fallback_wifi();
            } else {
                uconfy_is_fallback_wifi = 0;
                uconfy_configure_primary_wifi();
            }
        }

	    vTaskDelay(5000 / portTICK_PERIOD_MS);
	    esp_wifi_connect();
	    xEventGroupClearBits(wifi_event_group, UCONFIG_CONNECTED_BIT);
	    break;
	default:
	    break;
    }

    return ESP_OK;
}

void uconfy_initialize_wifi(char *initial_wifi_ssid, char *initial_wifi_password, int allow_wifi_fallback, void (*wifi_connected_callback)()) {
    ufonfy_allow_wifi_fallback = allow_wifi_fallback;
    uconfy_wifi_connected_callback = wifi_connected_callback;
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    int wifi_configured = 0;
    if (allow_wifi_fallback) {
        // using wifi configuration from NVS store
        wifi_configured = uconfy_configure_primary_wifi();
    }

    if (!wifi_configured) {
        // using default configuration, passed by argument
        wifi_config_t wifi_config = { };

        for (int i=0; i!=strlen(initial_wifi_ssid); i++) {
            wifi_config.sta.ssid[i] = initial_wifi_ssid[i];
        }
        wifi_config.sta.ssid[strlen(initial_wifi_ssid)] = '\0';

        for (int i=0; i!=strlen(initial_wifi_password); i++) {
            wifi_config.sta.password[i] = initial_wifi_password[i];
        }
        wifi_config.sta.password[strlen(initial_wifi_password)] = '\0';

        ESP_LOGI(TAG_UCONFIG, "Setting WiFi configuration SSID %s ...", wifi_config.sta.ssid);
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_LOGI(TAG_UCONFIG, "esp_wifi_set_config call completed");
    }

    ESP_LOGI(TAG_UCONFIG, "esp_wifi_start");
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_UCONFIG, "esp_wifi_start completed");
}

int uconfy_configure_primary_wifi() {
    char *wifi = uconfy_get_string_param(UCONFY_PRIMARY_WIFI, "");
    char *pwd = uconfy_get_string_param(UCONFY_PRIMARY_PWD, "");

    if (strlen(wifi) != 0 && strlen(pwd) != 0) {
        ESP_LOGI(TAG_UCONFIG, "Setting WiFi (primary) configuration SSID %s ...", wifi);
        wifi_config_t wifi_config = { };

        for (int i=0; i!=strlen(wifi); i++) {
            wifi_config.sta.ssid[i] = wifi[i];
        }
        wifi_config.sta.ssid[strlen(wifi)] = '\0';

        for (int i=0; i!=strlen(pwd); i++) {
            wifi_config.sta.password[i] = pwd[i];
        }
        wifi_config.sta.password[strlen(pwd)] = '\0';
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        return 1;
    }

    return 0;
}

void uconfy_configure_fallback_wifi() {
    char *wifi = uconfy_get_string_param(UCONFY_FALLBACK_WIFI, "");
    char *pwd = uconfy_get_string_param(UCONFY_FALLBACK_PWD, "");

    if (strlen(wifi) != 0 && strlen(pwd) != 0) {
        ESP_LOGI(TAG_UCONFIG, "Setting WiFi (fallback) configuration SSID %s ...", wifi);
        wifi_config_t wifi_config = { };

        for (int i=0; i!=strlen(wifi); i++) {
            wifi_config.sta.ssid[i] = wifi[i];
        }
        wifi_config.sta.ssid[strlen(wifi)] = '\0';

        for (int i=0; i!=strlen(pwd); i++) {
            wifi_config.sta.password[i] = pwd[i];
        }
        wifi_config.sta.password[strlen(pwd)] = '\0';
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    }
}