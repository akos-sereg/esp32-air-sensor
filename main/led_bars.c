#include "include/led_bars.h"

void led_bars_init_shift_registers() {
    // init shift register gpios
    gpio_set_direction(LED_STRIP_LATCH_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_STRIP_CLOCK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_STRIP_DATA_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_STRIP_LAST_LED, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_STRIP_LAST_LED);
}

// each column can be a value from 0 to 10
void led_bars_set(int column1) {
    if (column1 < 0 || column1 > 10) {
        // invalid input
        return;
    }

    gpio_set_level(LED_STRIP_LATCH_GPIO, 1);
    gpio_set_level(LED_STRIP_LATCH_GPIO, 0);

    for (int i=0; i!=9; i++) {
        gpio_set_level(LED_STRIP_CLOCK_GPIO, 0);
        if (i == 8) {
            gpio_set_level(LED_STRIP_DATA_GPIO, (column1) >= 1 ? 1 : 0); // wiring fail on physical board
        } else {
            gpio_set_level(LED_STRIP_DATA_GPIO, (column1) > i ? 1 : 0);
        }


        gpio_set_level(LED_STRIP_CLOCK_GPIO, 1);
    }

    gpio_set_level(LED_STRIP_LATCH_GPIO, 1);
    gpio_set_level(LED_STRIP_LATCH_GPIO, 0);

    // handle last led (red on the top), as this one is not driven by shift register
    gpio_set_level(LED_STRIP_LAST_LED, column1 == 10 ? 1 : 0);
}

 void led_bars_demo() {
    int tick_rate_ms = 100;

    for (int i=0; i!=11; i++) {
        led_bars_set(i);
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }

    for (int i=0; i!=11; i++) {
        led_bars_set(10);
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }

    for (int i=0; i!=11; i++) {
        led_bars_set(10);
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }
 }

 void led_bars_reset() {
    led_bars_set(0);
 }