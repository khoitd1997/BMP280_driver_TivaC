#include "TivaC_I2C.h"
#include "tm4c123gh6pm.h"

/**
 * @brief
 * Use I2C0 controller and support single or multiple bytes transfer up to high
 * speed modes
 *
 */
// TODO: OPTIONAL MASTER OR SLAVE CONFIG, high speed, arbitation
// add checking if the port is open or not
uint8_t i2c0_open(void)
{
  // Enable RCGCI2C for i2c0
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;
  // Enable clock for PORTB
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

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
  I2C0_MCR_R &= I2C_MCR_SFE;
  I2C0_MCR_R = I2C_MCR_MFE;

  // calculate the clock cycle and input into I2CMTPR
  // rn assume 80MHz, later add support for variable frequency
  I2C0_MTPR_R &= ~(I2C_MTPR_TPR_M);
  I2C0_MTPR_R += 0x09 << I2C_MTPR_TPR_S;
  return 0;
}

uint8_t i2c0_close(void)
{
  // should really disable interrupt here
  i2c0_waitBusy();

  // turn off I2C0 clock first
  SYSCTL_RCGCI2C_R &= ~SYSCTL_RCGCI2C_R0;

  // lock the port
  GPIO_PORTB_LOCK_R = 0x4C4F434B;
  GPIO_PORTB_CR_R |= 0x0C;
  GPIO_PORTB_AFSEL_R &= ~(0x0C);
  GPIO_PORTB_LOCK_R = 0x00;  // lock the PORTB
  return 0;
}

uint8_t i2c0_single_data_read(const uint8_t slave_address,
                              const uint8_t no_ack,
                              const uint8_t no_stop,
                              const uint8_t repeat_start)
{
  uint32_t i2c0_mcs_temp = 0;
  i2c0_waitBusy();

  // go into receive mode
  I2C0_MSA_R |= I2C_MSA_RS;

  // set slave address
  I2C0_MSA_R &= ~I2C_MSA_SA_M;
  I2C0_MSA_R += slave_address << I2C_MSA_SA_S;

  i2c0_waitBusy();

  // option parsing
  if (repeat_start)
    {
      i2c0_mcs_temp |= I2C_MCS_START;
    }
  else
    {
      i2c0_mcs_temp &= ~I2C_MCS_START;
    }
  if (no_stop)
    {
      i2c0_mcs_temp &= ~I2C_MCS_STOP;
    }
  else
    {
      i2c0_mcs_temp |= I2C_MCS_STOP;
    }

  if (no_ack)
    {
      i2c0_mcs_temp &= ~I2C_MCS_ACK;
    }
  else
    {
      i2c0_mcs_temp |= I2C_MCS_ACK;
    }
  i2c0_mcs_temp |= I2C_MCS_RUN;
  I2C0_MCS_R = i2c0_mcs_temp;

  i2c0_waitBusy();

  return i2c0_error_handling() ? 1 : ((I2C0_MDR_R) & (I2C_MDR_DATA_M)) >> I2C_MDR_DATA_S;
}

uint8_t i2c0_single_data_write(const uint8_t slave_address,
                               const uint8_t data_byte,
                               const uint8_t no_end_stop)
{
  i2c0_waitBusy();

  // go into write mode
  I2C0_MSA_R &= ~(I2C_MSA_RS);

  I2C0_MSA_R = (slave_address << I2C_MSA_SA_S) & (I2C_MSA_SA_M);

  I2C0_MDR_R &= I2C_MDR_DATA_M;
  I2C0_MDR_R += data_byte << I2C_MDR_DATA_S;

  // write settings
  if (no_end_stop)
    {
      I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN) & (~I2C_MCS_STOP);
    }
  else
    {
      I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    }

  i2c0_waitBusy();

  // TODO: make error code
  i2c0_error_handling();
  return 0;
}

uint8_t i2c0_stop(void)
{
  i2c0_waitBusy();
  uint32_t i2c0_mcs_temp = 0;
  i2c0_mcs_temp &= ~I2C_MCS_RUN;
  i2c0_mcs_temp &= ~I2C_MCS_START;
  i2c0_mcs_temp |= I2C_MCS_STOP;
  I2C0_MCS_R = i2c0_mcs_temp;
  i2c0_waitBusy();
  i2c0_error_handling();
  return 0;
}

uint8_t i2c0_keep_state(void)
{
  i2c0_waitBusy();
  uint32_t i2c0_mcs_temp = 0;
  i2c0_mcs_temp |= I2C_MCS_RUN;
  i2c0_mcs_temp &= ~I2C_MCS_START;
  i2c0_mcs_temp &= ~I2C_MCS_STOP;
  I2C0_MCS_R = i2c0_mcs_temp;
  i2c0_waitBusy();
  return 0;
}

uint8_t i2c0_multiple_data_byte_write(const uint8_t  slave_address,
                                      const uint8_t* output_buffer,
                                      const uint8_t  output_buffer_length)
{
  // check if there are actually multiple data bytes
  if (output_buffer_length < 2 || output_buffer == NULL)
    {
      return 2;
    }

  i2c0_waitBusy();

  // go into write mode
  I2C0_MSA_R &= ~(I2C_MSA_RS);

  // prepare the Tiva for communications by setting the R/W bit to 0
  // and write the address to the register and make first transmit
  // after first transmit remain in transmit state
  I2C0_MSA_R = (slave_address << I2C_MSA_SA_S) & (I2C_MSA_SA_M);
  I2C0_MDR_R |= I2C_MDR_DATA_M & ((*output_buffer++) << I2C_MDR_DATA_S);
  I2C0_MCS_R =
      (I2C_MCS_START | I2C_MCS_RUN) & ((~I2C_MCS_STOP) & (~I2C_MCS_HS));

  i2c0_waitBusy();

  i2c0_error_handling();

  for (int buffer_index = 1; buffer_index < output_buffer_length -1;
       ++buffer_index)
    {
      // transmit until the element before the last one
      I2C0_MDR_R |= I2C_MDR_DATA_M & ((*output_buffer++) << I2C_MDR_DATA_S);
      I2C0_MSA_R &= ~(I2C_MSA_RS);
      I2C0_MCS_R =
          ((~I2C_MCS_START) & (~I2C_MCS_STOP) & (~I2C_MCS_HS)) | I2C_MCS_RUN;

      i2c0_waitBusy();

      i2c0_error_handling();
    }

  // transmit the last element and return to idle state
  I2C0_MDR_R |= I2C_MDR_DATA_M & ((*output_buffer) << I2C_MDR_DATA_S);
  I2C0_MSA_R &= ~(I2C_MSA_RS);
  I2C0_MCS_R =
      ((~I2C_MCS_START) & (~I2C_MCS_HS)) | ((I2C_MCS_STOP) | (I2C_MCS_RUN));

  i2c0_waitBusy();

  i2c0_error_handling();
  return 0;
}

uint8_t i2c0_multiple_data_byte_read(const uint8_t slave_address,
                                     uint8_t*      input_buffer,
                                     const uint8_t input_buffer_length)
{
  if (input_buffer == NULL || input_buffer_length < 2)
    {
      return 2;
    }

  i2c0_waitBusy();
  
  // go into receive mode
  I2C0_MSA_R |= I2C_MSA_RS;

  // load slave address and the first data element
  I2C0_MSA_R = (slave_address << I2C_MSA_SA_S) & (I2C_MSA_SA_M);

  i2c0_waitBusy();

  // initiate the first read

  I2C0_MCS_R = (I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_ACK) & (~I2C_MCS_STOP);

  i2c0_waitBusy();

  *input_buffer = (I2C0_MDR_R & I2C_MDR_DATA_M) << I2C_MDR_DATA_S;
  ++input_buffer;

  // read up till the data before the last one
  for (int buffer_index = 1; buffer_index < input_buffer_length -1;
       ++buffer_index, ++input_buffer)
    {
      I2C0_MSA_R |= I2C_MSA_RS;
      I2C0_MCS_R =
          (I2C_MCS_RUN | I2C_MCS_ACK) & (~I2C_MCS_STOP) & (~I2C_MCS_START);

      i2c0_waitBusy();

      i2c0_error_handling();

      *input_buffer = (I2C0_MDR_R & I2C_MDR_DATA_M) >> I2C_MDR_DATA_S;
    }

  // Read the last data byte and close the transaction
  I2C0_MSA_R |= I2C_MSA_RS;
  I2C0_MCS_R =
      (I2C_MCS_RUN | I2C_MCS_STOP) & ((~I2C_MCS_START) & (~I2C_MCS_ACK));

  i2c0_waitBusy();

  i2c0_error_handling();

  *input_buffer = (I2C0_MDR_R & I2C_MDR_DATA_M) << I2C_MDR_DATA_S;

  i2c0_waitBusy();

  i2c0_error_handling();

  return 0;
}

uint8_t i2c0_error_handling(void)
{
  if (I2C0_MCS_R & I2C_MCS_ERROR)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

uint8_t i2c0_check_master_enabled(void)
{
  return (I2C0_MCR_R & I2C_MCR_MFE);
}

// will wait until the bus stops being busy
// TODO: add safety options like timeout
void i2c0_waitBusy(void)
{
  while ((I2C0_MCS_R & I2C_MCS_BUSY))
    {
      // wait for the bus to stop being busy
    }
}
