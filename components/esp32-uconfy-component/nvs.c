#include "include/uconfy.h"

#define UCONFIG_NVS_PARTITION   "storage"

void init_nvs_store() {
    nvs_error = nvs_flash_init();
    if (nvs_error == ESP_ERR_NVS_NO_FREE_PAGES || nvs_error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_error = nvs_flash_init();
    }
    ESP_ERROR_CHECK( nvs_error );
    nvs_error = nvs_open(UCONFIG_NVS_PARTITION, NVS_READWRITE, &app_nvs_handle);

    if (nvs_error != ESP_OK) {
        ESP_LOGE(TAG_UCONFIG, "Error (%s) opening NVS handle!", esp_err_to_name(nvs_error));
        nvs_flash_is_open = 0;
    } else {
        nvs_flash_is_open = 1;
    }
}

void set_nvs_value(char *key, char *value) {
    if (!nvs_flash_is_open) {
        return;
    }

    nvs_error = nvs_set_str(app_nvs_handle, key, value);
    if (nvs_error != ESP_OK) {
        ESP_LOGE(TAG_UCONFIG, "Failed to write into NVS store!");
    }

    if (nvs_error != ESP_OK) {
        ESP_LOGE(TAG_UCONFIG, "Error (%s) writing!", esp_err_to_name(nvs_error));
    }

    nvs_error = nvs_commit(app_nvs_handle);
    if (nvs_error != ESP_OK) {
        ESP_LOGE(TAG_UCONFIG, "Failed to commit NVS store changes!");
    }
}

char *get_nvs_value(char *key) {
    if (!nvs_flash_is_open) {
        return NULL;
    }

    char *p;
    char buf[256];
    size_t buf_len = sizeof(buf);
    nvs_error = nvs_get_str(app_nvs_handle, key, buf, &buf_len);
    switch (nvs_error) {
        case ESP_OK:
            p = malloc( sizeof(char) * ( buf_len + 1 ) );
            strcpy(p, buf);
            return p;
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG_UCONFIG, "The value for key '%s' is not initialized yet", key);
            break;
        default :
            ESP_LOGE(TAG_UCONFIG, "Error (%s) reading!", esp_err_to_name(nvs_error));
    }

    return NULL;
}

void uconfy_load_from_nvs() {
    init_nvs_store();

    if (uconfig_repo == NULL) {
        uconfig_repo = malloc(sizeof(struct uconfig_item) * UCONFIG_REPO_MAXITEMS);
    }

    uconfig_repo_len = 0;
    nvs_iterator_t it = nvs_entry_find("nvs", UCONFIG_NVS_PARTITION, NVS_TYPE_ANY);
    while (it != NULL) {
            nvs_entry_info_t info;
            nvs_entry_info(it, &info);
            it = nvs_entry_next(it);
            if (info.type == 33) {
                char *value = get_nvs_value(info.key);
                if (value != NULL) {
                    strcpy(uconfig_repo[uconfig_repo_len].key, info.key);
                    strcpy(uconfig_repo[uconfig_repo_len].value, value);
                    ESP_LOGI(TAG_UCONFIG, "Loaded uconfy param: '%s' = '%s'", uconfig_repo[uconfig_repo_len].key, uconfig_repo[uconfig_repo_len].value);
                    uconfig_repo_len++;
                }
                free(value);
            }
    };
}

void nvs_shutdown() {
    nvs_close(app_nvs_handle);
}