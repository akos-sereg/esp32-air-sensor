#ifndef __led_bars_h_included__
#define __led_bars_h_included__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_STRIP_DATA_GPIO	25
#define LED_STRIP_CLOCK_GPIO	33
#define LED_STRIP_LATCH_GPIO	27
#define LED_STRIP_LAST_LED  14

extern void led_bars_demo();
extern void led_bars_reset();
extern void led_bars_init_shift_registers();

// each column can be a value from 0 to 10
extern void led_bars_set(int column1);

#endif