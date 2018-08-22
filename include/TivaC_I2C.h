#ifndef _TIVAC_I2C_H
#define _TIVAC_I2C_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define REMAIN_TRANSMIT 2
#define NO_REMAIN_TRANSMIT 0

#define REMAIN_RECEIVE 3
#define NO_REMAIN_RECEIVE 0

#define NO_REPEAT_START 0
#define REPEAT_START 1

#define I2C0_TRY_FUNC(funcToExecute)                \
  do {                                              \
    I2c0ErrCode errCode = funcToExecute;            \
    if (errCode != I2C0_NO_ERR) { return errCode; } \
  } while (0)
typedef enum { I2C0_NO_ERR = 0, I2C0_TIMEOUT, I2C0_BUS_ERROR, I2C0_MASTER_DISABLED } I2c0ErrCode;

I2c0ErrCode i2c0_open(void);   // initialize i2c registers
I2c0ErrCode i2c0_close(void);  // cleanup the i2c controller

I2c0ErrCode i2c0_stop(void);  // generate i2c stop signal

I2c0ErrCode i2c0_keep_state(void);  // repeat previous i2c operation with same setting

// write multiple data bytes into the target i2c device
I2c0ErrCode i2c0_multiple_data_byte_write(const uint8_t  slave_address,
                                          const uint8_t* output_buffer,
                                          const uint8_t  output_buffer_length);
I2c0ErrCode i2c0_multiple_data_byte_read(const uint8_t slave_address,
                                         uint8_t*      input_buffer,
                                         const uint8_t input_buffer_length);

I2c0ErrCode i2c0_single_data_read(const uint8_t slave_address,
                                  const bool    no_ack,
                                  const bool    no_stop,
                                  const bool    no_start);

I2c0ErrCode i2c0_single_data_write(const uint8_t slave_address,
                                   const uint8_t data_byte,
                                   const bool    no_end_stop);

I2c0ErrCode i2c0_error_check(void);  // general error handling for i2c error

I2c0ErrCode i2c0_check_master_enabled(void);  // check if the master mode is enabled on tivaC

// wait until the i2c bus is not busy, do not call unless master/slave mode enabled
I2c0ErrCode i2c0_wait_bus(void);

#endif
