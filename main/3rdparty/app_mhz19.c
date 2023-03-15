/*
sources from: https://github.com/danielealbano/esp32-air-quality-monitor/blob/master/main/app_mhz19.c
*/

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/uart.h"

#include "include/3rdparty/mhz19.h"
#include "include/3rdparty/app_mhz19.h"

static const char* TAG = "APP/MHZ19";
static int co2 = 0;
static int temp = 0;

int app_mhz19_get_co2()
{
	return co2;
}

int app_mhz19_get_temp()
{
	return temp;
}

void app_mhz19_task(void* unused)
{
	mhz19_err_t err;

    int measure_count = 0;
    int measrure_success = 0;
	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(CO2_MEASUREMENT_INTERVAL_MS));

		// ESP_LOGI(TAG, "Reading data");
		err = mhz19_retrieve_data();
        measure_count++;
		if (err != MHZ19_ERR_OK)
		{
			switch(err)
			{
				case MHZ19_ERR_NO_DATA:
					ESP_LOGE(TAG, "MHZ19_ERR_NO_DATA");
					break;

				case MHZ19_ERR_TIMEOUT:
					ESP_LOGE(TAG, "MHZ19_ERR_TIMEOUT");
					break;

				case MHZ19_ERR_INVALID_RESPONSE:
					ESP_LOGE(TAG, "MHZ19_ERR_INVALID_RESPONSE");
					break;

				case MHZ19_ERR_UNEXPECTED_CMD:
					ESP_LOGE(TAG, "MHZ19_ERR_UNEXPECTED_CMD");
					break;

				case MHZ19_ERR_WRONG_CRC:
					ESP_LOGE(TAG, "MHZ19_ERR_WRONG_CRC");
					break;

				case MHZ19_ERR_OK:
					// do nothing
					break;
			}

			if (measure_count != 0) {
                printf("CO2 success rate: %f\n", (float)((measrure_success*100) / measure_count));
            }

			continue;
		} else {
		    measrure_success++;
		}

		co2 = mhz19_get_co2();
		temp = mhz19_get_temperature();

		if (measure_count == INT_MAX) {
		    measure_count = 0;
		    measrure_success = 0;
		}

		if (measure_count != 0) {
		    printf("CO2 success rate: %f\n", (float)((measrure_success*100) / measure_count));
		}


		// ESP_LOGI(TAG, "CO2: %d", co2);
		// ESP_LOGI(TAG, "Temperature: %d", temp);
	}
}