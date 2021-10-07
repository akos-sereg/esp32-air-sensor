/*
sources from: https://github.com/danielealbano/esp32-air-quality-monitor/blob/master/main/app_mhz19_init.c
*/

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "include/mhz19.h"

#include "include/app_mhz19_init.h"
#include "include/app_mhz19.h"

#define UART_BAUD_RATE 9600
#define UART_PIN_RX 34  // TX on device
#define UART_PIN_TX 26 // RX on device

static const char* TAG = "APP/MHZ19/INIT";

const int uart_buffer_size = (1024 * 2);
QueueHandle_t uart_queue;
uart_port_t uart_num = UART_NUM_2;

void app_mhz19_init_uart()
{
	uart_config_t uart_config = {
        .baud_rate = 9600,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
		.use_ref_tick = false,
    };

    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_TX, UART_PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 100, &uart_queue, 0));
}

void app_mhz19_init_library()
{
	mhz19_init(uart_num);
	mhz19_set_auto_calibration(false);
	mhz19_set_range(2000);
}

void app_mhz19_init_task()
{
	xTaskCreate(app_mhz19_task, "app_mhz19_task", 2048, NULL, 25, NULL);
}

void app_mhz19_init()
{
	ESP_LOGI(TAG, "Initializing MHZ19");

	app_mhz19_init_uart();
	app_mhz19_init_library();
	app_mhz19_init_task();

	ESP_LOGI(TAG, "MHZ19 initialized");
}