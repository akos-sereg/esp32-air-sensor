#ifndef __mq7_h_included__
#define __mq7_h_included__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "config.h"

#define coefficient_A 19.32
#define coefficient_B -0.64
#define MG7_V_IN    5.0
#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          // Multisampling
#define POTMETER_DEBUG_MODE 1

extern void mq_sensors_setup_async(void);
extern void mq_sensors_setup_internal(void *pvParameters);
extern int mq_sensors_analog_read();
extern float mq_sensors_voltage_conversion(int value);
extern float mq_sensors_get_ratio();
extern float mq_sensors_get_mq7_ppm();
extern uint32_t mq_sensors_get_mq6_milli_volts();

#endif