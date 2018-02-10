#ifndef _TIVAC_I2C_H
#define _TIVAC_I2C_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
 
#define REMAIN_TRANSMIT 2
#define NO_REMAIN_TRANSMIT 0

#define REMAIN_RECEIVE 3
#define NO_REMAIN_RECEIVE 0

#define NO_REPEAT_START 0
#define REPEAT_START 1

uint8_t i2c0_open(void);  /*initialize I2C registers*/
uint8_t i2c0_close(void); /*Cleanup the I2C controller*/

uint8_t i2c0_multiple_data_byte_write(const uint8_t  slave_address,
                                      const uint8_t* output_buffer,
                                      const uint8_t  output_buffer_length);
uint8_t i2c0_multiple_data_byte_read(const uint8_t slave_address,
                                     uint8_t*      input_buffer,
                                     const uint8_t input_buffer_length);

uint8_t i2c0_single_data_read(const uint8_t slave_address,
                              const uint8_t remain_receive,
                              const uint8_t repeat_start);
uint8_t i2c0_single_data_write(const uint8_t slave_address,
                               const uint8_t data_byte,
                               const uint8_t no_end_stop);

uint8_t i2c0_error_handling(void);

#endif
