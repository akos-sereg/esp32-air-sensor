#ifndef MHZ19_H
#define MHZ19_H

#include "include/config.h"

#define MHZ19_PRINT_BUFFER 1

enum mhz19_err
{
    MHZ19_ERR_OK                =  0,
    MHZ19_ERR_NO_DATA           = -1,
    MHZ19_ERR_TIMEOUT           = -2,
    MHZ19_ERR_INVALID_RESPONSE  = -3,
    MHZ19_ERR_UNEXPECTED_CMD    = -4,
    MHZ19_ERR_WRONG_CRC         = -5,
};

typedef enum mhz19_err mhz19_err_t;

void mhz19_init(uart_port_t uart_num);
void mhz19_set_auto_calibration(bool mode);
void mhz19_set_range(int range);
int mhz19_get_co2();
int mhz19_get_temperature();
size_t mhz19_read(uint8_t data[], uint8_t len);
void mhz19_prepare_for_write_discard_data_in_uart();
size_t mhz19_write(uint8_t data[], uint8_t len);
void mhz19_print_buffer(uint8_t is_request, unsigned char* buffer);
void mhz19_send_command(uint8_t command, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7);
mhz19_err_t mhz19_receive_response(uint8_t cmd[9]);
mhz19_err_t mhz19_retrieve_data();
uint8_t mhz19_calc_crc(uint8_t data[]);
int mhz19_bytes2int(uint8_t h, uint8_t l);

#endif