#include "include/app_main.h"

static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
static const gpio_num_t dht_gpio = 4;

int last_co2_readings[5];
int last_co2_readings_pos = 0;

void push_last_co2_reading(int co2_ppm) {
    if (last_co2_readings_pos < 5) {
        last_co2_readings[last_co2_readings_pos] = co2_ppm;
        last_co2_readings_pos++;
        return;
    } else {
        for (int i=0; i!=4; i++) {
            last_co2_readings[i] = last_co2_readings[i+1];
        }

        last_co2_readings[4] = co2_ppm;
    }
}

bool are_co2_readings_constant() {
    if (last_co2_readings_pos < 5) {
        printf("[experiment] CO2 reading did not reach 5 yet\n");
        return false;
    }

    printf("[experiment] CO2 reading reached 5 \n");

    if (last_co2_readings[0] == last_co2_readings[1] &&
        last_co2_readings[1] == last_co2_readings[2] &&
        last_co2_readings[2] == last_co2_readings[3] &&
        last_co2_readings[3] == last_co2_readings[4]) {
        printf("[experiment] CO2 levels are the same, restarting\n");
        return true;
    }

    printf("[experiment] CO2 levels are not the same, OK\n");
    return false;
}

void main_task()
 {
    int elapsed_ms_since_last_report = 0;
    int warm_up_iterations = WARM_UP_ITERATIONS;
    int tick_rate_ms = 5000;
    int co2_new;
    int invalid_data_to_thingspeak_attempt = 0;
    int16_t temperature = 0;
    int16_t humidity = 0;
    float hum = 0.0;
    float temp = 0.0;
    float co_ppm = 0;

    int last_co2_pos = -1;

    /**
     * https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
     * 250-400 ppm: Normal background concentration in outdoor ambient air
     * 400-1000 ppm: Concentrations typical of occupied indoor spaces with good air exchange
     * 1000-2000 ppm: Complaints of drowsiness and poor air.
     * 2000-5000 ppm: Headaches, sleepiness and stagnant, stale, stuffy air. Poor concentration, loss of attention, increased heart rate and slight nausea may also be present.
     * > 5000 ppm: Workplace exposure limit (as 8-hour TWA) in most jurisdictions.
     * > 40000 ppm: Exposure may lead to serious oxygen deprivation resulting in permanent brain damage, coma, even death.
     */
    //                       G    G    G    G    Y     Y     Y     R     R
    int co2_thresholds[] = { 400, 550, 700, 850, 1000, 1333, 1666, 2000, 2300 };

    /**
     * https://www.kidde.com/home-safety/en/us/support/help-center/browse-articles/articles/what_are_the_carbon_monoxide_levels_that_will_sound_the_alarm_.html
     * 50 ppm: None for healthy adults. According to the Occupational Safety & Health Administration (OSHA), this is the maximum allowable concentration for continuous exposure for healthy adults in any eight-hour period.
     * 200 ppm: Slight headache, fatigue, dizziness, and nausea after two to three hours.
     * 400 ppm: Frontal headaches with one to two hours. Life threatening after three hours.
     * 800 ppm: Dizziness, nausea, and convulsions within 45 minutes. Unconsciousness within two hours. Death within two to three hours.
     * 1600 ppm: Headache, dizziness and nausea within 20 minutes. Death within one hour.
     */
    //                      G  G   G   G   Y   Y   Y    R    R
    // int co_thresholds[] = { 0, 15, 30, 45, 60, 80, 100, 200, 300};

    //                       G  G    G    G    Y    Y    Y    R    R
    // int lpg_thresholds[] = { 0, 200, 300, 400, 500, 600, 700, 800, 1085 };

    led_bars_demo();
    led_bars_demo();
    led_bars_demo();
    led_bars_reset();

    while(1) {
        // get measures from sensors
        // ------------------------------------------------------------------
        co2_new = app_mhz19_get_co2();
        // temp_new = app_mhz19_get_temp();
        co_ppm = mq_sensors_get_mq7_ppm();
        // lpg_mvolts = mq_sensors_get_mq6_milli_volts();


        if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK) {
            hum = (float)humidity / 10;
            temp = (float)temperature / 10;
        }

        // printf("CO2: %d, Hum: %.2f, Temp: %.2f\n", co2_new, hum, temp);

        // wait tick rate in every iteration
        // ------------------------------------------------------------------
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
        elapsed_ms_since_last_report += tick_rate_ms;

        // report data to ThingSpeak periodically
        // ------------------------------------------------------------------
        if (elapsed_ms_since_last_report > THINGSPEAK_REPORT_INTERVAL) {
            elapsed_ms_since_last_report = 0;

            if (co2_new == 0) {
                // sensor is measuring invalid CO2, avoid sending it to ThingSpeak
                invalid_data_to_thingspeak_attempt++;
            } else {
                invalid_data_to_thingspeak_attempt = 0;
            }

            // CO2 sensor needs some warm up time, so we do not send data in he first couple of iterations
            if (warm_up_iterations == 0) {
                printf("Reporting: CO2=%d hum=%f temp=%f co=%f\n", co2_new, hum, temp, co_ppm);
                http_get_task(co2_new, hum, temp, co_ppm);
            } else {
                warm_up_iterations--;
            }

            push_last_co2_reading(co2_new);

            /*if (co2_new == 0) {
                // sensor is measuring invalid CO2, avoid sending it to ThingSpeak
                invalid_data_to_thingspeak_attempt++;
            } else {
                // CO2 sensor needs some warm up time, so we do not send data in he first couple of iterations
                if (warm_up_iterations == 0) {
                    printf("Reporting: CO2=%d hum=%f temp=%f co=%f\n", co2_new, hum, temp, co_ppm);
                    http_get_task(co2_new, hum, temp, co_ppm);
                } else {
                    warm_up_iterations--;
                }
                push_last_co2_reading(co2_new);
                invalid_data_to_thingspeak_attempt = 0;
            }*/

            uconfy_flush_logs();
            uconfy_fetch_configs(NULL);
        }

        // restart device after a couple of unsuccessful attempts
        // (eg. sensor fails to report correct data)
        // ------------------------------------------------------------------
        if (invalid_data_to_thingspeak_attempt > 5) {
            esp_restart();
        }

        // restart device if readings got stuck
        if (are_co2_readings_constant()) {
            esp_restart();
        }

        // calculate led bar positions for each measure
        // ------------------------------------------------------------------
        int co2_pos = 0;
        for (int i=0; i!=9; i++) {
            if (co2_new > co2_thresholds[i]) {
                co2_pos++;
            }
        }

        // make sure that we are not touching the display if there is noting to be changed. this is to avoid
        // flickering.
        // ------------------------------------------------------------------
        if (last_co2_pos == co2_pos) {
            continue;
        }

        last_co2_pos = co2_pos;

        // update display
        // ------------------------------------------------------------------
        led_bars_set(co2_pos);
    }
}

void uconfy_configurations_fetched() {
    printf("Configurations successfully fetched from remote server\n");
    printf("WIFI: %s\n", uconfy_get_string_param("wifi", "fallback-value"));
}

void wifi_connected() {
    printf("Connected to Wifi network. Now we can fetch configs.\n");
    uconfy_fetch_configs(&uconfy_configurations_fetched);
}

void app_main()
{
    app_mhz19_init();
    led_bars_init_shift_registers();
    mq_sensors_setup_async();

    uconfy_load_from_nvs();
    uconfy_initialize_wifi(EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS, 1, &wifi_connected);
    uconfy_init("3a284b9b-d66f-48be-907f-84c2e6e40967", "7612354561234781256347123564");

    uconfy_log("Device started");

    // mq_sensors_setup_async();
    main_task();
}