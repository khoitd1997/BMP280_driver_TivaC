/**
 * @brief Contain main I2C functions for TIvaC
 *
 * @file TivaC_I2C.c
 * @author Khoi Trinh
 * @date 2018-08-25
 */

#include "include/TivaC_I2C.h"

#include <stdbool.h>

#include "external/TivaC_Utils/include/tm4c123gh6pm.h"

#define SCL_LP 6
#define SCL_HP 4

#define I2C0_TIMEOUT_LIMIT 10000

/**
 * @brief calculate the timer period for I2C
 * @param i2cSclClockPeriodNs i2c clock period in nanoseconds
 * @param cpuClockPeriodNs cpu clock period in nanoseconds
 * @param tprOut pointer to the return tpr
 *
 */
static I2c0ErrCode i2c0_calculate_tpr(const float i2cSclClockPeriodNs,
                                      const float cpuClockPeriodNs,
                                      uint8_t*    tprOut) {
  *tprOut = (uint8_t)((i2cSclClockPeriodNs / (cpuClockPeriodNs * (SCL_LP + SCL_HP) * 2)) - 1);
  return I2C0_NO_ERR;
}

/**
 * @brief enable clocks, I2C pins and calculate the appropriate clock period
 * This function should be called first b4 any I2C operations
 */
I2c0ErrCode i2c0_open(void) {
  // Enable RCGCI2C for i2c0
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;
  // Enable clock for PORTB
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
  while (!(SYSCTL_PRI2C_R & SYSCTL_PRI2C_R0) || !(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)) {
    // wait until the peripherals are ready
  }

  // Enable Digital and I2C function and disable
  // the rest
  GPIO_PORTB_LOCK_R |= 0x4C4F434B;
  GPIO_PORTB_CR_R |= 0x0C;
  GPIO_PORTB_PUR_R &= 0xFFFFFFF3;
  GPIO_PORTB_AFSEL_R |= 0x0C;
  GPIO_PORTB_DEN_R |= 0x0C;
  GPIO_PORTB_PCTL_R &= ~0x0FF00;
  GPIO_PORTB_PCTL_R = (0x3 << 8) | (0x3 << 12);
  GPIO_PORTB_AMSEL_R &= ~0x0C;

  // Open drain on I2CSDA PB3 and none on PB2
  GPIO_PORTB_ODR_R |= 0x08;
  GPIO_PORTB_ODR_R &= ~(0x04);

  // I2CMCR0 init master
  I2C0_MCR_R = I2C_MCR_MFE;

  // calculate the clock cycle and input into I2CMTPR
  uint8_t tpr;
  i2c0_calculate_tpr(10000, 62.5, &tpr);
  I2C0_MTPR_R &= ~(I2C_MTPR_TPR_M);
  I2C0_MTPR_R += tpr << I2C_MTPR_TPR_S;
  return I2C0_NO_ERR;
}

/**
 * @brief Disable clock as well as the I2C pins
 *
 */
I2c0ErrCode i2c0_close(void) {
  I2C0_TRY_FUNC(i2c0_wait_bus());

  // turn off I2C0 clock first
  SYSCTL_RCGCI2C_R &= ~SYSCTL_RCGCI2C_R0;

  // lock the port
  GPIO_PORTB_LOCK_R = 0x4C4F434B;
  GPIO_PORTB_CR_R |= 0x0C;
  GPIO_PORTB_AFSEL_R &= ~(0x0C);
  GPIO_PORTB_LOCK_R = 0x00;  // lock the PORTB
  I2C0_TRY_FUNC(i2c0_error_check());
  return I2C0_NO_ERR;
}

/**
 * @brief read one byte of data
 * @param no_ack whether to generate ack signal
 * @param no_stop whether to generate stop signal
 * @param no_start whether to generate start(or repeated start) signal
 *
 */
I2c0ErrCode i2c0_single_data_read(const uint8_t slave_address,
                                  uint8_t*      returnData,
                                  const bool    no_ack,
                                  const bool    no_stop,
                                  const bool    no_start) {
  uint32_t i2c0_mcs_temp = 0;
  I2C0_TRY_FUNC(i2c0_wait_bus());

  // go into receive mode
  I2C0_MSA_R |= I2C_MSA_RS;

  // set slave address
  I2C0_MSA_R &= ~I2C_MSA_SA_M;
  I2C0_MSA_R += (slave_address << I2C_MSA_SA_S) & (I2C_MSA_SA_M);

  I2C0_TRY_FUNC(i2c0_wait_bus());

  // option parsing
  if (no_start) {
    i2c0_mcs_temp &= ~I2C_MCS_START;
  } else {
    i2c0_mcs_temp |= I2C_MCS_START;
  }

  if (no_stop) {
    i2c0_mcs_temp &= ~I2C_MCS_STOP;
  } else {
    i2c0_mcs_temp |= I2C_MCS_STOP;
  }

  if (no_ack) {
    i2c0_mcs_temp &= ~I2C_MCS_ACK;
  } else {
    i2c0_mcs_temp |= I2C_MCS_ACK;
  }
  i2c0_mcs_temp |= I2C_MCS_RUN;
  I2C0_MCS_R = i2c0_mcs_temp;

  I2C0_TRY_FUNC(i2c0_wait_bus());
  I2C0_TRY_FUNC(i2c0_error_check());

  *returnData = ((I2C0_MDR_R) & (I2C_MDR_DATA_M)) >> I2C_MDR_DATA_S;
  return I2C0_NO_ERR;
}

I2c0ErrCode i2c0_single_data_write(const uint8_t slave_address,
                                   const uint8_t data_byte,
                                   const bool    no_end_stop) {
  I2C0_TRY_FUNC(i2c0_wait_bus());

  I2C0_MSA_R &= ~(I2C_MSA_SA_M);
  I2C0_MSA_R += (slave_address << I2C_MSA_SA_S);

  // go into write mode
  I2C0_MSA_R &= ~(I2C_MSA_RS);

  I2C0_MDR_R |= I2C_MDR_DATA_M & (data_byte << I2C_MDR_DATA_S);

  I2C0_TRY_FUNC(i2c0_wait_bus());

  // write settings
  if (no_end_stop) {
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN) & (~I2C_MCS_STOP) & (~I2C_MCS_HS);
  } else {
    I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP) & (~I2C_MCS_HS);
  }

  I2C0_TRY_FUNC(i2c0_wait_bus());

  I2C0_TRY_FUNC(i2c0_error_check());
  return I2C0_NO_ERR;
}

/**
 * @brief used to generate I2C stop signal
 *
 */
I2c0ErrCode i2c0_stop(void) {
  I2C0_TRY_FUNC(i2c0_wait_bus());
  uint32_t i2c0_mcs_temp = 0;
  i2c0_mcs_temp &= ~I2C_MCS_RUN;
  i2c0_mcs_temp &= ~I2C_MCS_START;
  i2c0_mcs_temp |= I2C_MCS_STOP;
  I2C0_MCS_R = i2c0_mcs_temp;
  I2C0_TRY_FUNC(i2c0_wait_bus());
  I2C0_TRY_FUNC(i2c0_error_check());
  return I2C0_NO_ERR;
}

/**
 * @brief used to maintain current I2C state
 *
 */
I2c0ErrCode i2c0_keep_state(void) {
  I2C0_TRY_FUNC(i2c0_wait_bus());
  uint32_t i2c0_mcs_temp = 0;
  i2c0_mcs_temp |= I2C_MCS_RUN;
  i2c0_mcs_temp &= ~I2C_MCS_START;
  i2c0_mcs_temp &= ~I2C_MCS_STOP;
  I2C0_MCS_R = i2c0_mcs_temp;
  I2C0_TRY_FUNC(i2c0_wait_bus());
  return I2C0_NO_ERR;
}

/**
 * @brief write byte one by one until done
 *
 */
I2c0ErrCode i2c0_multiple_data_byte_write(const uint8_t  slave_address,
                                          const uint8_t* output_buffer,
                                          const uint8_t  output_buffer_length) {
  // check if there are actually multiple data bytes
  if (output_buffer_length < 2 || output_buffer == NULL) { return 2; }

  I2C0_TRY_FUNC(i2c0_wait_bus());

  // go into write mode
  I2C0_MSA_R &= ~(I2C_MSA_RS);

  // prepare the Tiva for communications by setting the R/W bit to 0
  // and write the address to the register and make first transmit
  // after first transmit remain in transmit state
  I2C0_MSA_R &= ~(I2C_MSA_SA_M);
  I2C0_MSA_R += (slave_address << I2C_MSA_SA_S);
  I2C0_MDR_R |= I2C_MDR_DATA_M & ((*output_buffer++) << I2C_MDR_DATA_S);
  I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN) & ((~I2C_MCS_STOP) & (~I2C_MCS_HS));

  I2C0_TRY_FUNC(i2c0_wait_bus());

  I2C0_TRY_FUNC(i2c0_error_check());

  for (int buffer_index = 1; buffer_index < output_buffer_length - 1; ++buffer_index) {
    // transmit until the element before the last one
    I2C0_MDR_R |= I2C_MDR_DATA_M & ((*output_buffer++) << I2C_MDR_DATA_S);
    I2C0_MSA_R &= ~(I2C_MSA_RS);
    I2C0_MCS_R = ((~I2C_MCS_START) & (~I2C_MCS_STOP) & (~I2C_MCS_HS)) | I2C_MCS_RUN;

    I2C0_TRY_FUNC(i2c0_wait_bus());

    I2C0_TRY_FUNC(i2c0_error_check());
  }

  // transmit the last element and return to idle state
  I2C0_MDR_R |= I2C_MDR_DATA_M & ((*output_buffer) << I2C_MDR_DATA_S);
  I2C0_MSA_R &= ~(I2C_MSA_RS);
  I2C0_MCS_R = ((~I2C_MCS_START) & (~I2C_MCS_HS)) | ((I2C_MCS_STOP) | (I2C_MCS_RUN));

  I2C0_TRY_FUNC(i2c0_wait_bus());

  I2C0_TRY_FUNC(i2c0_error_check());
  return I2C0_NO_ERR;
}

I2c0ErrCode i2c0_multiple_data_byte_read(const uint8_t slave_address,
                                         uint8_t*      input_buffer,
                                         const uint8_t input_buffer_length) {
  if (input_buffer == NULL || input_buffer_length < 2) { return 2; }

  I2C0_TRY_FUNC(i2c0_wait_bus());
  int buffer_index = 0;

  // go into receive mode
  I2C0_MSA_R |= I2C_MSA_RS;

  // load slave address and the first data element
  I2C0_MSA_R &= ~(I2C_MSA_SA_M);
  I2C0_MSA_R += (slave_address << I2C_MSA_SA_S) & (I2C_MSA_SA_M);

  I2C0_TRY_FUNC(i2c0_wait_bus());

  // initiate the first read

  I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_ACK) & (~I2C_MCS_STOP);

  I2C0_TRY_FUNC(i2c0_wait_bus());

  input_buffer[buffer_index] = (I2C0_MDR_R & I2C_MDR_DATA_M) << I2C_MDR_DATA_S;

  // read up till the data before the last one
  for (buffer_index = 1; buffer_index < input_buffer_length - 1; ++buffer_index) {
    I2C0_MSA_R |= I2C_MSA_RS;
    I2C0_MCS_R = (I2C_MCS_RUN | I2C_MCS_ACK) & (~I2C_MCS_STOP) & (~I2C_MCS_START);

    I2C0_TRY_FUNC(i2c0_wait_bus());

    I2C0_TRY_FUNC(i2c0_error_check());

    input_buffer[buffer_index] = (I2C0_MDR_R & I2C_MDR_DATA_M);
  }

  // Read the last data byte and close the transaction
  I2C0_MSA_R |= I2C_MSA_RS;
  I2C0_MCS_R = (I2C_MCS_RUN | I2C_MCS_STOP) & ((~I2C_MCS_START) & (~I2C_MCS_ACK));

  I2C0_TRY_FUNC(i2c0_wait_bus());

  I2C0_TRY_FUNC(i2c0_error_check());

  input_buffer[buffer_index] = (I2C0_MDR_R & I2C_MDR_DATA_M);

  I2C0_TRY_FUNC(i2c0_wait_bus());

  I2C0_TRY_FUNC(i2c0_error_check());

  return I2C0_NO_ERR;
}

/**
 * @brief check i2c error register for any problems
 *
 */
I2c0ErrCode i2c0_error_check(void) {
  if (I2C0_MCS_R & I2C_MCS_ERROR) {
    return I2C0_BUS_ERROR;
  } else {
    return I2C0_NO_ERR;
  }
}

/**
 * @brief check i2c config register to see if the master functionality is enabled
 *
 */
I2c0ErrCode i2c0_check_master_enabled(void) {
  return (I2C0_MCR_R & I2C_MCR_MFE) ? I2C0_NO_ERR : I2C0_MASTER_DISABLED;
}

/**
 * @brief used for waiting till the bus stops being busy
 * @return whether the bus is idle now or timeout happened
 */
I2c0ErrCode i2c0_wait_bus(void) {
  uint32_t timeoutCounter = 0;
  while ((I2C0_MCS_R & I2C_MCS_BUSY)) {
    if (timeoutCounter > I2C0_TIMEOUT_LIMIT) { return I2C0_TIMEOUT; }
    ++timeoutCounter;
  }
  return I2C0_NO_ERR;
}
