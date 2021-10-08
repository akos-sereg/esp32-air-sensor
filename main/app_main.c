#include "include/app_main.h"
#include "include/app_mhz19_init.h"
#include "include/app_mhz19.h"

void main_task(void * pvParameter)
{
    int tick_rate_ms = 5000;
    int co2_new;
    int temp_new;

    /**
     * https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
     * 250-400 ppm: Normal background concentration in outdoor ambient air
     * 400-1000 ppm: Concentrations typical of occupied indoor spaces with good air exchange
     * 1000-2000 ppm: Complaints of drowsiness and poor air.
     * 2000-5000 ppm: Headaches, sleepiness and stagnant, stale, stuffy air. Poor concentration, loss of attention, increased heart rate and slight nausea may also be present.
     * > 5000 ppm: Workplace exposure limit (as 8-hour TWA) in most jurisdictions.
     * > 40000 ppm: Exposure may lead to serious oxygen deprivation resulting in permanent brain damage, coma, even death.
     */
    //                       G  G    G     G     Y     Y     Y     R     R
    int co2_thresholds[] = { 0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500 };

    led_bars_demo();
    led_bars_demo();
    led_bars_demo();
    led_bars_reset();

    while(1) {
        co2_new = app_mhz19_get_co2();
        temp_new = app_mhz19_get_temp();
        printf("CO2: %d, Temp: %d\n", co2_new, temp_new);

        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);

        int co2_pos = 1;
        for (int i=0; i!=9; i++) {
            if (co2_new > co2_thresholds[i]) {
                co2_pos++;
            }
        }

        led_bars_set(co2_pos, 0, 0);
    }
}

void app_main()
{
    app_mhz19_init();
    led_bars_init_shift_registers();
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}

