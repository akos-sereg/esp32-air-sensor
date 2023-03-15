#ifndef __logging_h_included__
#define __logging_h_included__

#include <string.h>
#include "esp_log.h"
#include "nvs.h"

#define UCONFIG_TMP_LOG_MAX_SIZE    5000

char uconfig_tmp_log[UCONFIG_TMP_LOG_MAX_SIZE];
extern void uconfy_log(char *message);

#endif