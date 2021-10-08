#include "include/app_main.h"
#include "include/app_mhz19_init.h"
#include "include/app_mhz19.h"

void main_task()
 {
    //int report_interval_ms = 1800000; // 1 800 000 milliseconds: 30 mins
    int report_interval_ms = 60000;
    int elapsed_ms = 0;
    int tick_rate_ms = 5000;
    int co2_new;
    int temp_new;
    float co_ppm;
    int lpg_mvolts;
    int wifi_initiated = 0;

    int last_co_pos = -1;
    int last_co2_pos = -1;
    int last_lpg_pos = -1;

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

    //                       G  G    G    G    Y    Y    Y    R    R
    int lpg_thresholds[] = { 0, 200, 300, 400, 500, 600, 700, 800, 1085 };

    led_bars_demo();
    led_bars_demo();
    led_bars_demo();
    led_bars_reset();

    while(1) {
        co2_new = app_mhz19_get_co2();
        temp_new = app_mhz19_get_temp();
        co_ppm = mq7_getPpm();
        lpg_mvolts = mq6_getMilliVolts();
        printf("CO2: %d, Temp: %d, CO: %f\n", co2_new, temp_new, co_ppm);

        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
        elapsed_ms += tick_rate_ms;

        // report data to ThingSpeak
        if (elapsed_ms > report_interval_ms) {
            elapsed_ms = 0;
            http_get_task(co2_new, co_ppm, lpg_mvolts);
        }

        if (elapsed_ms == 10000 && !wifi_initiated) {
            networking_initialize_wifi();
            wifi_initiated = 1;
        }

        // calculate led bar positions for each measure
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

        int lpg_pos = 1;
        for (int i=0; i!=9; i++) {
            if (lpg_mvolts > lpg_thresholds[i]) {
                lpg_pos++;
            }
        }

        // make sure that we are not touching the display if there is noting to be changed. this is to avoid
        // flickering.
        if (last_co2_pos == co2_pos && last_co_pos == co_pos && last_lpg_pos == lpg_pos) {
            continue;
        }

        last_co2_pos = co2_pos;
        last_co_pos = co_pos;
        last_lpg_pos = lpg_pos;

        led_bars_set(co2_pos, co_pos, lpg_pos);
    }
}

void app_main()
{
    app_mhz19_init();
    led_bars_init_shift_registers();
    mq7_setup();

    /// xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
    main_task();
}