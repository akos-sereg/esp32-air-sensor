/*
sources from: https://github.com/danielealbano/esp32-air-quality-monitor/blob/master/main/app_mhz19.h
*/

#ifndef APP_MHZ19_H
#define APP_MHZ19_H

int app_mhz19_get_co2();
int app_mhz19_get_temp();
void app_mhz19_task(void*);

#endif