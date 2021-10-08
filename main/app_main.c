#include "include/app_main.h"
#include "include/app_mhz19_init.h"
#include "include/app_mhz19.h"

void main_task(void * pvParameter)
 {
    int tick_rate_ms = 5000;
    int co2_new;
    int temp_new;
    float co_ppm;

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
    int co2_thresholds[] = { 0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000};

    /**
     * https://www.kidde.com/home-safety/en/us/support/help-center/browse-articles/articles/what_are_the_carbon_monoxide_levels_that_will_sound_the_alarm_.html
     * 50 ppm: None for healthy adults. According to the Occupational Safety & Health Administration (OSHA), this is the maximum allowable concentration for continuous exposure for healthy adults in any eight-hour period.
     * 200 ppm: Slight headache, fatigue, dizziness, and nausea after two to three hours.
     * 400 ppm: Frontal headaches with one to two hours. Life threatening after three hours.
     * 800 ppm: Dizziness, nausea, and convulsions within 45 minutes. Unconsciousness within two hours. Death within two to three hours.
     * 1600 ppm: Headache, dizziness and nausea within 20 minutes. Death within one hour.
     */
    //                      G  G   G   G   Y   Y   Y    R    R
    int co_thresholds[] = { 0, 15, 30, 45, 60, 80, 100, 200, 300};

    led_bars_demo();
    led_bars_demo();
    led_bars_demo();
    led_bars_reset();

    while(1) {
        co2_new = app_mhz19_get_co2();
        temp_new = app_mhz19_get_temp();
        co_ppm = mq7_getPpm();
        printf("CO2: %d, Temp: %d, CO: %f\n", co2_new, temp_new, co_ppm);

        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);

        int co2_pos = 1;
        for (int i=0; i!=9; i++) {
            if (co2_new > co2_thresholds[i]) {
                co2_pos++;
            }
        }

         int co_pos = 1;
        for (int i=0; i!=9; i++) {
            if (co_ppm > co_thresholds[i]) {
                co_pos++;
            }
        }

        led_bars_set(co2_pos, co_pos, 0);
    }
}

void app_main()
{
    app_mhz19_init();
    led_bars_init_shift_registers();
    mq7_setup();
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}

