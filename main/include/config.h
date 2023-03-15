#ifndef __sensor_config_h_included__
#define __sensor_config_h_included__

#define EXAMPLE_WIFI_SSID ""
#define EXAMPLE_WIFI_PASS ""

#define THINGSPEAK_API_KEY ""

#define CO2_REQUEST_TIMEOUT_PERIOD 500
#define MQ_MEASUREMENT_INTERVAL_MS 4000
#define CO2_MEASUREMENT_INTERVAL_MS 4000
#define THINGSPEAK_REPORT_INTERVAL 60000

/**
 * CO2 sensor has to warm up, so data should not be sent to thingspeak in the very first minutes. Sensor data
 * in the first few minutes are not reliable.
 *
 * WARM_UP_ITERATIONS=3 means that we skip the first 3 thingspeak cycles (eg. 3 minutes, if THINGSPEAK_REPORT_INTERVAL
 * is one minute)
 */
#define WARM_UP_ITERATIONS 3

#endif