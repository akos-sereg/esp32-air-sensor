#include "include/app_main.h"
#include "include/app_mhz19_init.h"
#include "include/app_mhz19.h"

void main_task(void * pvParameter)
{
    int tick_rate_ms = 100;
    int co2_new;
    int temp_new;

    led_bars_demo();
    led_bars_reset();

    while(1) {
        co2_new = app_mhz19_get_co2();
        temp_new = app_mhz19_get_temp();
        printf("CO2: %d, Temp: %d\n", co2_new, temp_new);

        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
    }
}

void app_main()
{
    app_mhz19_init();
    led_bars_init_shift_registers();
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}

