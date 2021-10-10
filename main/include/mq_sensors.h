#ifndef __mq7_h_included__
#define __mq7_h_included__

#define coefficient_A 19.32
#define coefficient_B -0.64

//Load resistance 10 Kohms on the sensor potentiometer
//#define R_Load 10.0
// #define R_Load 1.0

#define MG7_V_IN    5.0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

extern void mq_sensors_setup_async(void);
extern void mq_sensors_setup_internal(void *pvParameters);
extern int mq_sensors_analog_read();
extern float mq_sensors_voltage_conversion(int value);
extern float mq_sensors_get_ratio();
extern float mq_sensors_get_mq7_ppm();
extern uint32_t mq_sensors_get_mq6_milli_volts();

#endif