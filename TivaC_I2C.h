#ifndef _TIVAC_I2C_H
#define _TIVAC_I2C_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "TM4C123.h"
#include "TM4C123GH6PM.h"
uint8_t i2c0_open(void);  /*initialize I2C registers*/
uint8_t i2c0_close(void); /*Cleanup the I2C controller*/
uint8_t i2c0_multiple_data_byte_write(const uint8_t  slave_address,
                                      const uint8_t* output_buffer,
                                      const uint8_t  output_buffer_length);
uint8_t i2c0_multiple_data_byte_read(const uint8_t slave_address,
                                     uint8_t*      input_buffer,
                                     const uint8_t input_buffer_length);

uint8_t i2c0_single_data_read(const uint8_t slave_address);
uint8_t i2c0_single_data_write(const uint8_t slave_address,
                               const uint8_t data_byte);
{
#endif
