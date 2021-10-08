/*
Ported from https://github.com/SebaFerraro/mq7_esp32_lib/blob/master/src/MQ7.cpp

*/

#include "include/mq7.h"

#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          // Multisampling
#define POTMETER_DEBUG_MODE 1

static esp_adc_cal_characteristics_t *adc_chars;

// Channel 3 -> GPIO 39
// Channel 0 -> GPIO 36
static const adc_channel_t channel_red = ADC_CHANNEL_3;
static const adc_channel_t channel_blue = ADC_CHANNEL_0;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;

static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
uint32_t voltage_mq7 = 0;
uint32_t voltage_mq6 = 0;

float mq7_getPpm() {
     return (float)(coefficient_A * pow(mq7_getRatio(), coefficient_B));
}

uint32_t mq6_getMilliVolts() {
    return voltage_mq6;
}

float mq7_getRatio() {
    int value = mq7_analogRead();
    float v_out = mq7_voltageConversion(value);
    return (MG7_V_IN - v_out) / v_out;
}

float mq7_voltageConversion(int value) {
  return (float) value * (MG7_V_IN / 4095.0);
}

int mq7_analogRead() {
    return (int)voltage_mq7;
}

static void mq7_print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void mq7_setup_internal(void *pvParameters) {
    uint32_t adc_reading_mq7 = 0;
    uint32_t adc_reading_mq6 = 0;

    // configure ADC
    if (unit == ADC_UNIT_1) {
        printf("Configured for ADC 1\n");
        adc1_config_width(width);
        adc1_config_channel_atten(channel_red, atten);
        adc1_config_channel_atten(channel_blue, atten);
    }

    // characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    mq7_print_char_val_type(val_type);

    // continuously sample ADC1
    while (1) {
        adc_reading_mq7 = 0;
        adc_reading_mq6 = 0;

        // multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading_mq7 += adc1_get_raw((adc1_channel_t)channel_red);
                adc_reading_mq6 += adc1_get_raw((adc1_channel_t)channel_blue);
            }
        }

        adc_reading_mq7 /= NO_OF_SAMPLES;
        adc_reading_mq6 /= NO_OF_SAMPLES;

        // convert adc_reading to voltage in mV
        voltage_mq7 = esp_adc_cal_raw_to_voltage(adc_reading_mq7, adc_chars);
        voltage_mq6 = esp_adc_cal_raw_to_voltage(adc_reading_mq6, adc_chars);

        if (POTMETER_DEBUG_MODE == 1) {
            printf("Raw MQ7: %d\tVoltage: %dmV\n", adc_reading_mq7, voltage_mq7);
            printf("Raw MQ6: %d\tVoltage: %dmV\n", adc_reading_mq6, voltage_mq6);
        }

        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void mq7_setup(void)
{
    xTaskCreate(&mq7_setup_internal, "mq7_setup", 4096, NULL, 5, NULL);
}