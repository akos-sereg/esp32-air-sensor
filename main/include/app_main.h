#ifndef __app_main_h_included__
#define __app_main_h_included__

#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "rom/uart.h"

#include "3rdparty/app_mhz19_init.h"
#include "3rdparty/app_mhz19.h"
#include "3rdparty/dht.h"
#include "led_bars.h"
#include "http.h"
#include "networking.h"
#include "config.h"

#endif