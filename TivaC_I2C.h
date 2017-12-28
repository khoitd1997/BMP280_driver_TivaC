#ifndef _TIVAC_I2C_H
#define _TIVAC_I2C_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tm4c123gh6pm.h>

uint8_t  i2c0_open(uint32_t slave_address); /*initialize I2C registers*/
uint8_t  i2c0_close(void);                  /*Cleanup the I2C controller*/
uint32_t i2c0_data_read(void);
uint32_t i2c0_data_write(uint8_t data_byte);
#endif