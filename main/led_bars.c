#include "include/led_bars.h"

void led_bars_init_shift_registers() {
    // init shift register gpios
    gpio_set_direction(LED_STRIP_LATCH_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_STRIP_CLOCK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_STRIP_DATA_GPIO, GPIO_MODE_OUTPUT);
}

// each column can be a value from 0 to 10
void led_bars_set(int column1, int column2, int column3) {
    if (column1 < 0 || column2 < 0 || column3 < 0 ||
        column1 > 10 || column2 > 10 || column3 > 10) {
        // invalid input
        return;
    }

    for (int i=0; i!=10; i++) {
        gpio_set_level(LED_STRIP_CLOCK_GPIO, 0);
        gpio_set_level(LED_STRIP_DATA_GPIO, (9 - column3) < i ? 1 : 0);
	    gpio_set_level(LED_STRIP_CLOCK_GPIO, 1);
    }

    for (int i=0; i!=10; i++) {
        gpio_set_level(LED_STRIP_CLOCK_GPIO, 0);
        gpio_set_level(LED_STRIP_DATA_GPIO, (9 - column2) < i ? 1 : 0);
        gpio_set_level(LED_STRIP_CLOCK_GPIO, 1);
    }

    for (int i=0; i!=10; i++) {
        gpio_set_level(LED_STRIP_CLOCK_GPIO, 0);
        gpio_set_level(LED_STRIP_DATA_GPIO, (9 - column1) < i ? 1 : 0);
        gpio_set_level(LED_STRIP_CLOCK_GPIO, 1);
    }

    gpio_set_level(LED_STRIP_LATCH_GPIO, 1);
    gpio_set_level(LED_STRIP_LATCH_GPIO, 0);
}

 void led_bars_demo() {
    int tick_rate_ms = 100;

    for (int i=0; i!=11; i++) {
        led_bars_set(i, 0, 0);
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }

    for (int i=0; i!=11; i++) {
        led_bars_set(10, i, 0);
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }

    for (int i=0; i!=11; i++) {
        led_bars_set(10, 10, i);
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }
 }

 void led_bars_reset() {
    led_bars_set(0, 0, 0);
 }