#include "include/uconfy.h"

void uconfy_log(char *message) {
    printf("[--> Remote Log <--] %s\n", message);
    if ((strlen(message) + strlen(uconfig_tmp_log) + 1) > UCONFIG_TMP_LOG_MAX_SIZE) {
        ESP_LOGE(TAG_UCONFIG, "Unable to append log, buffer is full.");
        return;
    }

    sprintf(uconfig_tmp_log, "%s\n", message);

    uconfy_flush_logs();
}